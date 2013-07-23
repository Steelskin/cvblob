// Copyright (C) 2007 by Cristóbal Carnero Liñán
// grendel.ccl@gmail.com
//
// Copyright (C) 2013 by Fabrice de Gans-Riberi for ProViSys Engineering
// fabrice.degans@gmail.com
//
// This file is part of cvBlob.
//
// cvBlob is free software: you can redistribute it and/or modify
// it under the terms of the Lesser GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// cvBlob is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// Lesser GNU General Public License for more details.
//
// You should have received a copy of the Lesser GNU General Public License
// along with cvBlob.  If not, see <http://www.gnu.org/licenses/>.
//

#include <cmath>
#include <iostream>
#include <sstream>

#include <opencv2/highgui/highgui.hpp>

#include "cvb_blob_list.h"

using namespace cvb;

const std::tuple<cv::Point, unsigned char, ChainCode> movesE[4][3] =
{
    { std::make_tuple(cv::Point(-1, -1), 3, ChainCode_up_left),    std::make_tuple(cv::Point( 0, -1), 0, ChainCode_up),    std::make_tuple(cv::Point( 1, -1), 0, ChainCode_up_right)   },
    { std::make_tuple(cv::Point( 1, -1), 0, ChainCode_up_right),   std::make_tuple(cv::Point( 1,  0), 1, ChainCode_right), std::make_tuple(cv::Point( 1,  1), 1, ChainCode_down_right) },
    { std::make_tuple(cv::Point( 1,  1), 1, ChainCode_down_right), std::make_tuple(cv::Point( 0,  1), 2, ChainCode_down),  std::make_tuple(cv::Point(-1,  1), 2, ChainCode_down_left)  },
    { std::make_tuple(cv::Point(-1,  1), 2, ChainCode_down_left),  std::make_tuple(cv::Point(-1,  0), 3, ChainCode_left),  std::make_tuple(cv::Point(-1, -1), 3, ChainCode_up_left)    }
};

const std::tuple<cv::Point, unsigned char, ChainCode> movesI[4][3] =
{ 
    { std::make_tuple(cv::Point( 1, -1), 3, ChainCode_up_right),   std::make_tuple(cv::Point( 0, -1), 0, ChainCode_up),    std::make_tuple(cv::Point(-1, -1), 0, ChainCode_up_left)    },
    { std::make_tuple(cv::Point(-1, -1), 0, ChainCode_up_left),    std::make_tuple(cv::Point(-1,  0), 1, ChainCode_left),  std::make_tuple(cv::Point(-1,  1), 1, ChainCode_down_left)  },
    { std::make_tuple(cv::Point(-1,  1), 1, ChainCode_down_left),  std::make_tuple(cv::Point( 0,  1), 2, ChainCode_down),  std::make_tuple(cv::Point(1 ,  1), 2, ChainCode_down_right) },
    { std::make_tuple(cv::Point( 1,  1), 2, ChainCode_down_right), std::make_tuple(cv::Point( 1,  0), 3, ChainCode_right), std::make_tuple(cv::Point( 1, -1), 3, ChainCode_up_right)   }
};

BlobList::BlobList() {
}

void BlobList::SimpleLabel(const cv::Mat &img, Label max_label) {
    CV_Assert(img.type() == CV_8UC1);

    // Reset
    blobs.clear();
    imgLabel = cv::Mat::zeros(img.size(), CVB_LABEL);

    // Do nothing if image is empty
    if (img.rows == 0)
        return;

    // Ensure matrix is continuous
    cv::Mat imgInCont;
    if (img.isContinuous())
        imgInCont = img;
    else
        img.copyTo(imgInCont);

    Label label = 0;
    BlobsMap blob_map;

    unsigned char *imgInBuff = (unsigned char*) imgInCont.ptr();
    size_t stepIn = imgInCont.step1();
    auto imageIn = [&imgInBuff, &stepIn] (int x, int y) -> unsigned char& {
        return imgInBuff[x + y * stepIn];
    };

    Label *imgOutBuff = (Label *) imgLabel.ptr();
    size_t stepOut = imgLabel.step1();
    auto imageOut = [&imgOutBuff, &stepOut] (int x, int y) -> Label& {
        return imgOutBuff[x + y * stepOut];
    };

    for (int y = 0; y < img.rows; y++) {
        for (int x = 0; x < img.cols; x++) {
            // Nothing to label
            if (!imageIn(x, y))
                continue;

            Label l = 0;
            SharedBlob blob;

            // Get the whole line
            int begin_x = x;     // Beginning of internal blob line
            int end_x = begin_x; // End of internal blob line
            BlobsMap prev_blobs; // Blobs on previous line
            while (end_x != img.cols && imageIn(end_x, y)) {
                if (y != 0 && (l = imageOut(end_x, y -1))) {
                    if (prev_blobs.find(l) == prev_blobs.end())
                        prev_blobs.insert(*blob_map.find(l));
                }
                end_x++;
            }
            x = end_x;

            if (prev_blobs.empty()) {
                // New blob
                label++;
                if (label >= max_label)
                    goto SimpleBlobEnd;
                l = label;
                blob = SharedBlob(new Blob(begin_x, end_x - 1, y, label));
                blob_map.insert(blob_map.end(), LabelBlob(label, blob));
            } else {
                SharedBlob blob = prev_blobs.begin()->second;

                if (prev_blobs.size() > 1)  {
                    // Merge blobs
                    for (auto &a_blob : prev_blobs) {
                        if (blob == a_blob.second)
                            continue;
                        blob->Merge(*a_blob.second.get());
                        blob_map.at(a_blob.second->label) = blob;
                    }
                }
                l = blob->label;
                blob->add_Moment(begin_x, end_x - 1, y);
            }

            // Label line
            for (int i_x = begin_x; i_x < end_x; i_x++) {
                imageOut(i_x, y) = l;
            }
        }
    }

SimpleBlobEnd:
    // Remove duplicates
    BlobsMap new_map;
    for (auto &a_blob : blob_map) {
        if (new_map.find(a_blob.second->label) != new_map.end())
            continue;
        new_map.insert(LabelBlob(a_blob.second->label, a_blob.second));
    }

    // Generate final list
    for (auto &a_blob : new_map) {
        blobs.push_back(a_blob.second);
        a_blob.second->ComputeMoments();
    }
}

void BlobList::LabelImage (const cv::Mat &img, Label max_label) {
    CV_Assert(img.type() == CV_8UC1);


    // Reset
    blobs.clear();
    imgLabel = cv::Mat::zeros(img.size(), CVB_LABEL);
    // Ensure matrix is continuous
    cv::Mat imgInCont;
    if (img.isContinuous())
        imgInCont = img;
    else
        img.copyTo(imgInCont);

    Label label = 0;

    unsigned int imgIn_width = imgInCont.cols;
    unsigned int imgIn_height = imgInCont.rows;
    unsigned int imgOut_width = imgLabel.cols;
    unsigned int imgOut_height = imgLabel.rows;

    Label lastLabel = 0;
    SharedBlob lastBlob;
    BlobsMap blob_map;

    unsigned char *imgInBuff = (unsigned char*) imgInCont.ptr();
    size_t stepIn = imgInCont.step1();
    auto imageIn = [&imgInBuff, &stepIn] (int x, int y) -> unsigned char& {
        return imgInBuff[x + y * stepIn];
    };

    Label *imgOutBuff = (Label *) imgLabel.ptr();
    size_t stepOut = imgLabel.step1();
    auto imageOut = [&imgOutBuff, &stepOut] (int x, int y) -> Label& {
        return imgOutBuff[x + y * stepOut];
    };

    for (unsigned int y = 0; y < imgIn_height; y++) {
        for (unsigned int x = 0; x < imgIn_width; x++) {
            // Ignore if input is 0
            if (!imageIn(x, y))
                continue;

            bool labelled = imageOut(x, y) != 0;
            if (labelled)
                continue;

            if (y == 0 || !imageIn(x, y - 1)) {
                // Not labelled and previous element wasn't 0
                // Label contour.
                labelled = true;
                label++;

                if (label >= max_label)
                    goto LabelEnd;
                imageOut(x, y) = label;

                // XXX This is not necessary at all. I only do this for consistency.
                if (y > 0)
                    imageOut(x, y - 1) = MaxLabel;

                // Create new blob.
                SharedBlob blob(new Blob(cv::Point(x, y), label));
                blob_map.insert(LabelBlob(label, blob));
                lastLabel = label;
                lastBlob = blob;

                // Now, to find the contour.
                unsigned char direction = 1;
                unsigned int xx = x;
                unsigned int yy = y;
                bool contourEnd = false;

                while (!contourEnd) {
                    bool found = false;

                    for (unsigned char i = 0; i < 3; i++) {
                        // Move to the direction
                        int nx = xx + std::get<0>(movesE[direction][i]).x;
                        int ny = yy + std::get<0>(movesE[direction][i]).y;

                        // Boundaries check
                        if ((nx >= imgIn_width) || (nx < 0) || (ny >= imgIn_height) || (ny < 0))
                            continue;

                        if (!imageIn(nx, ny)) {
                            imageOut(nx, ny) = MaxLabel;
                            continue;
                        }

                        // Found the next part of the blob contour
                        found = true;
                        blob->add_ChainCode(std::get<2>(movesE[direction][i]));
                        xx = nx;
                        yy = ny;
                        if (imageOut(xx, yy) != label) {
                            imageOut(xx, yy) = label;
                            blob->add_Moment(xx, yy);
                        }
                        direction = std::get<1>(movesE[direction][i]);
                        break;
                    }


                    if (!found)
                        // New direction
                        direction = (direction + 1) % 4;
                    // Check if we went full circle
                    contourEnd = ((xx == x) && (yy == y) && (direction == 1));
                } // while (!ContourEnd)
                continue;
            } // if ((!labelled) && ((y == 0) || (!imageIn(x, y-1))))


            if ((y + 1 < imgIn_height) && (!imageIn(x, y + 1)) && (!imageOut(x, y + 1))) {
                // We're in a "hole" inside the blob
                labelled = true;

                // Label internal contour
                Label l;
                SharedBlob blob;

                l = imageOut(x - 1, y);
                imageOut(x, y) = l;

                if (l == lastLabel)
                    blob = lastBlob;
                else {
                    blob = blob_map.find(l)->second;
                    lastLabel = l;
                    lastBlob = blob;
                }

                if (!imageOut(x, y))
                    blob->add_Moment(x, y);


                // XXX This is not necessary (I believe). I only do this for consistency.
                imageOut(x, y + 1) = MaxLabel;

                SharedContour contour(new Contour(cv::Point(x, y)));

                unsigned char direction = 3;
                unsigned int xx = x;
                unsigned int yy = y;
                bool contourEnd = false;

                while (!contourEnd) {
                    bool found = false;

                    for (unsigned char i = 0; i < 3; i++) {
                        // Move to the direction
                        int nx = xx + std::get<0>(movesI[direction][i]).x;
                        int ny = yy + std::get<0>(movesI[direction][i]).y;

                        // Boundaries check
                        if ((nx >= imgIn_width) || (nx < 0) || (ny >= imgIn_height) || (ny < 0))
                            continue;

                        if (!imageIn(nx, ny)) {
                            imageOut(nx, ny) = MaxLabel;
                            continue;
                        }

                        // Found the next part of the internal contour
                        found = true;
                        contour->add_ChainCode(std::get<2>(movesI[direction][i]));
                        xx = nx;
                        yy = ny;
                        if (!imageOut(xx, yy)) {
                            imageOut(xx, yy) = l;
                            blob->add_Moment(xx, yy);
                        }
                        direction = std::get<1>(movesI[direction][i]);
                        // Check if we went full circle
                        contourEnd = (xx == x) && (yy == y);
                        break;
                    }

                    if (!found)
                        // New direction
                        direction = (direction + 1) % 4;
                } // while (!contourEnd)

                // Finally, add the internal contour
                blob->add_InternalContour(contour);
                continue;
            } // if ((y + 1 < imgIn_height) && (!imageIn(x, y + 1)) && (!imageOut(x, y + 1)))

            //else, element is not labelled
            // Internal pixel
            Label l = imageOut(x - 1, y);

            imageOut(x, y) = l;

            SharedBlob blob;
            if (l == lastLabel)
                blob = lastBlob;
            else {
                blob = blob_map.find(l)->second;
                lastLabel = l;
                lastBlob = blob;
            }
            blob->add_Moment(x, y);
        }
    }

LabelEnd:
    // Populate list
    for (auto &a_blob : blob_map) {
        blobs.push_back(a_blob.second);
        a_blob.second->ComputeMoments();
    }

}

void BlobList::FilterLabels(cv::Mat &imgOut) const {
    CV_Assert(imgOut.type() == CV_8UC1 && imgOut.isContinuous());

    size_t stepIn = imgLabel.step1();
    size_t stepOut = imgOut.step1();
    int imgIn_width = imgLabel.cols;
    int imgIn_height = imgLabel.rows;
    int imgOut_width = imgOut.cols;
    int imgOut_height = imgOut.rows;

    char *imgDataOut = (char *) imgOut.ptr();
    Label *imgDataIn=(Label *)imgLabel.ptr();

    // FIXME This is inefficient.
    // We should first zero out the matrix
    // Then get a list of the labels currently in the blobs list and draw them
    // 
    // FIXME This won't work with simplelabel, due to different labelling for one single blob

    for (unsigned int r = 0; r < (unsigned int)imgIn_height; r++, imgDataIn += stepIn, imgDataOut += stepOut) {
        for (unsigned int c = 0; c < (unsigned int)imgIn_width; c++) {
            Label l = imgDataIn[c];
            if (l) {
                // Find the blob and draw it
                SharedBlob the_blob;
                for (auto &a_blob : blobs)
                    if (a_blob->label == l)
                        the_blob = a_blob;
                if (the_blob.get() == nullptr)
                    imgDataOut[c] = 0x00;
                else
                    imgDataOut[c] = (char)0xff;
            }
            else
                imgDataOut[c] = 0x00;
        }
    }
}

std::list<SharedBlob> BlobList::get_BlobsList() const {
    return this->blobs;
}

cv::Mat BlobList::get_ImageLabel() const {
    return this->imgLabel;
}

Label BlobList::GetLabel(unsigned int x, unsigned int y) const {
    size_t step = imgLabel.step1();
    unsigned int img_width = imgLabel.cols;
    unsigned int img_height= imgLabel.rows;

    CV_Assert((x < img_width) && (y < img_height));

    return ((Label *)(imgLabel.ptr()))[x + y * step];
}


SharedBlob BlobList::get_LargestBlob() const {
    unsigned int maxArea = 0;
    SharedBlob outBlob;

    for (auto &a_blob : blobs) {
        if (a_blob->get_Area() > maxArea) {
            maxArea = a_blob->get_Area();
            outBlob = a_blob;
        }
    }

    return outBlob;
}

void BlobList::FilterByArea(unsigned int minArea, unsigned int maxArea) {
    auto it = blobs.begin();
    while(it != blobs.end()) {
        auto &blob = *it;

        if ((blob->get_Area() < minArea) || (blob->get_Area() > maxArea)) {
            auto tmp = it;
            ++it;
            blobs.erase(tmp);
        } else
            ++it;
    }
}

void BlobList::FilterByLabel(Label label) {
    auto &it = blobs.begin();
    while(it != blobs.end()) {
        auto &blob = *it;

        if (blob->label != label) {
            auto &tmp = it;
            ++it;
            blobs.erase(tmp);
        } else
            ++it;
    }
}

/*void cvCentralMoments(CvBlob *blob, const cv::Mat &img) {
if (!blob->centralMoments)
{
CV_Assert(img&&(img->depth()==IPL_DEPTH_LABEL)&&(img->nChannels==1));

//cvCentroid(blob); // Here?

blob->u11=blob->u20=blob->u02=0.;

// Only in the bounding box
int stepIn = img->widthStep / (img->depth() / 8);
int img_width = img.cols;
int img_height = img.rows;
int img_offset = 0;
if(0 != img->roi) {
img_width = img->roi.cols;
img_height = img->roi.rows;
img_offset = img->roi->xOffset + (img->roi->yOffset * stepIn);
}

Label *imgData=(Label *)img.ptr() + (blob->miny * stepIn) + img_offset;
for (unsigned int r=blob->miny;
r<blob->maxy;
r++,imgData+=stepIn)
for (unsigned int c=blob->minx;c<blob->maxx;c++)
if (imgData[c]==blob->label) {
double tx=(c-blob->centroid.x);
double ty=(r-blob->centroid.y);
blob->u11+=tx*ty;
blob->u20+=tx*tx;
blob->u02+=ty*ty;
}

blob->centralMoments = true;
}
}*/


///////////////////////////////////////////////////////////////////////////////////////////////////
// Based on http://en.wikipedia.org/wiki/HSL_and_HSV

/// \def HSV2RGB(H, S, V, R, G, B)
/// \brief Color translation between HSV and RGB.
inline void HSV2RGB (double H, double S, double V, double &R, double &G, double &B) {
    double _h = H/60.;
    int _hf = (int)floor(_h);
    int _hi = ((int)_h)%6;
    double _f = _h - _hf;

    double _p = V * (1. - S);
    double _q = V * (1. - _f * S);
    double _t = V * (1. - (1. - _f) * S);

    switch (_hi) {
    case 0:
        R = 255.*V; G = 255.*_t; B = 255.*_p;
        break;
    case 1:
        R = 255.*_q; G = 255.*V; B = 255.*_p;
        break;
    case 2:
        R = 255.*_p; G = 255.*V; B = 255.*_t;
        break;
    case 3:
        R = 255.*_p; G = 255.*_q; B = 255.*V;
        break;
    case 4:
        R = 255.*_t; G = 255.*_p; B = 255.*V;
        break;
    case 5:
        R = 255.*V; G = 255.*_p; B = 255.*_q;
        break;
    }
}

void BlobList::RenderBlobs(const cv::Mat &imgSource, cv::Mat &imgDest, unsigned short mode, double alpha) const {
    typedef std::map<Label, cv::Scalar> Palete;

    CV_Assert(imgSource.type() == CV_8UC3);
    CV_Assert(imgDest.type() == CV_8UC3 && imgDest.size() == imgSource.size() && imgDest.isContinuous());

    Palete pal;
    if (mode & CV_BLOB_RENDER_COLOR) {

        unsigned int colorCount = 0;
        for (auto &a_blob : blobs) {
            auto &label = a_blob->label;

            double r, g, b;

            HSV2RGB((double)((colorCount*77)%360), .5, 1., r, g, b);
            colorCount++;

            pal[label] = cv::Scalar(r, g, b);
        }
    }

    for (auto &a_blob : blobs)
        a_blob->RenderBlob(imgLabel, imgSource, imgDest, mode, pal[a_blob->label], alpha);
}


