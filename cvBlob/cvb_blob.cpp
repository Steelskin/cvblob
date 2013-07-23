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
#include <opencv2/highgui/highgui.hpp>

#include "cvb_blob.h"

using namespace cvb;

Blob::Blob(cv::Point point, Label label) {
    this->label = label;
    this->m00 = 1;
    this->minx = point.x;
    this->maxx = point.x;
    this->miny = point.y;
    this->maxy = point.y;
    this->m10 = point.x;
    this->m01 = point.y;
    this->m11 = point.x * point.y;
    this->m20 = point.x * point.x;
    this->m02 = point.y * point.y;
    this->contour = Contour(point);
}

Blob::Blob(unsigned int min_x, unsigned int max_x, unsigned int y, Label label) {
    this->label = label;
    unsigned int n = max_x - min_x + 1;           // Number of items
    double x_sum = n * (min_x + max_x) / 2; // Sum from min_x to max_x

    this->m00 = n;
    this->minx = min_x;
    this->maxx = max_x;
    this->miny = y;
    this->maxy = y;

    this->m10 = x_sum;
    this->m01 = y * n;
    this->m11 = y * x_sum;
    this->m02 = y * y * n;

    double sum_min_sq; // sum of squares from 1 to (min_x - 1)
    if (min_x == 0)
        sum_min_sq = 0;
    else
        sum_min_sq = (min_x - 1) * (min_x) * (2 * (min_x - 1) + 1) / 6;
    double sum_max_sq = max_x * (max_x + 1) * (2 * max_x + 1) / 6; // sum of squares from 1 to max_x
    this->m20 = sum_max_sq - sum_min_sq; // sum of squares from min_x to max_x
}

Contour Blob::get_Contour() const {
    return this->contour;
}

ContoursList Blob::get_InternalContours() const {
    return this->internalContours;
}

cv::Point2d Blob::get_Centroid() const {
    return centroid;
}

double Blob::get_Angle() const {
    return .5 * atan2(2. * u11, u20 - u02);
}

unsigned int Blob::get_Area() const {
    return m00;
}

void Blob::SaveImage(const std::string &filename, const cv::Mat &img) const {
    cv::imwrite(filename, img(get_BoundingBox()));
}

cv::Rect Blob::get_BoundingBox() const {
    return cv::Rect(minx, miny, maxx - minx + 1, maxy - miny + 1);
}

void Blob::add_ChainCode(ChainCode chainCode) {
    this->contour.add_ChainCode(chainCode);
}

void Blob::add_Moment(unsigned int x, unsigned int y) {
    minx = std::min(x, minx);
    maxx = std::max(x, maxx);
    miny = std::min(y, miny);
    maxy = std::max(y, maxy);

    m00++;
    m10 += x;
    m01 += y;
    m11 += x * y;
    m20 += x * x;
    m02 += y * y;
}

void Blob::add_Moment(unsigned int min_x, unsigned int max_x, unsigned int y) {
    minx = std::min(min_x, minx);
    maxx = std::max(max_x, maxx);
    miny = std::min(y, miny);
    maxy = std::max(y, maxy);

    unsigned int n = max_x - min_x + 1;           // Number of items
    double x_sum = n * (min_x + max_x) / 2; // Sum from min_x to max_x

    m00 += n;
    m10 += x_sum;
    m01 += y * n;
    m11 += y * x_sum;
    m02 += y * y * n;

    double sum_min_sq; // sum of squares from 1 to (min_x - 1)
    if (min_x == 0)
        sum_min_sq = 0;
    else
        sum_min_sq = (min_x - 1) * (min_x) * (2 * (min_x - 1) + 1) / 6;
    double sum_max_sq = max_x * (max_x + 1) * (2 * max_x + 1) / 6; // sum of squares from 1 to max_x
    this->m20 = sum_max_sq - sum_min_sq; // sum of squares from min_x to max_x
}

void Blob::add_InternalContour(SharedContour contour) {
    this->internalContours.push_back(contour);
}

void Blob::ComputeMoments() {
    centroid = cv::Point2d(m10 / m00, m01 / m00);

    u11 = m11 - (m10 * m01) / m00;
    u20 = m20 - (m10 * m10) / m00;
    u02 = m02 - (m01 * m01) / m00;

    double m00_2 = m00 * m00;

    n11 = u11 / m00_2;
    n20 = u20 / m00_2;
    n02 = u02 / m00_2;

    p1 = n20 + n02;

    double nn = n20 - n02;
    p2 = nn * nn + 4. * (n11 * n11);
}


cv::Scalar Blob::get_MeanColor(const cv::Mat &imgLabel, const cv::Mat &img) const {
    CV_Assert(imgLabel.type() == CVB_LABEL && imgLabel.isContinuous());
    CV_Assert(img.type() == CV_8UC3);

    // Ensure image is continuous
    cv::Mat imgBlob;
    img(get_BoundingBox()).copyTo(imgBlob);

    size_t stepLbl = imgLabel.step1();;
    size_t stepImg = imgBlob.step1();;
    unsigned int imgLabel_width = imgLabel.cols;
    unsigned int imgLabel_height = imgLabel.rows;
    unsigned int img_width = imgBlob.cols;
    unsigned int img_height = imgBlob.rows;
    unsigned int imgChan = imgBlob.channels();

    Label *labels = (Label *)imgLabel.ptr();
    unsigned char *imgData = (unsigned char *)imgBlob.ptr();

    double mb = 0;
    double mg = 0;
    double mr = 0;
    double pixels = (double)get_Area();

    for (unsigned int r = 0; r < (unsigned int)imgLabel_height; r++, labels += stepLbl, imgData += stepImg) {
        for (unsigned int c = 0; c < imgLabel_width; c++) {
            if (labels[c] == label) {
                mb += ((double)imgData[imgChan*c+0]) / pixels; // B
                mg += ((double)imgData[imgChan*c+1]) / pixels; // G
                mr += ((double)imgData[imgChan*c+2]) / pixels; // R
            }
        }
    }

    /*double mb = 0;
    double mg = 0;
    double mr = 0;
    double pixels = (double)blob->get_Area();
    for (unsigned int y=0; y<imgLabel->height; y++)
        for (unsigned int x=0; x<imgLabel->width; x++) {
            if (cvGetLabel(imgLabel, x, y)==blob->label) {
                CvScalar color = cvGet2D(imgBlob, y, x);
                mb += color.val[0]/pixels;
                mg += color.val[1]/pixels;
                mr += color.val[2]/pixels;
            }
        }*/

    return cv::Scalar(mr, mg, mb);
}


void Blob::RenderBlob(const cv::Mat &imgLabel, const cv::Mat &imgSource, cv::Mat &imgDest, unsigned short mode, const cv::Scalar &color, double alpha) const {
    CV_Assert(imgLabel.type() == CVB_LABEL && imgLabel.isContinuous());
    CV_Assert(imgSource.type() == CV_8UC3);
    CV_Assert(imgDest.type() == CV_8UC3 && imgDest.size() == imgSource.size() && imgDest.isContinuous());

    cv::Mat imgSourceCont;
    if (imgSource.isContinuous())
        imgSourceCont = imgSource;
    else
        imgSource.copyTo(imgSourceCont);

    if (mode & CV_BLOB_RENDER_COLOR) {
        size_t stepLbl = imgLabel.step1();;
        size_t stepSrc = imgSourceCont.step1();;
        size_t stepDst = imgDest.step1();;
        unsigned int nbchanSrc = imgSourceCont.channels();
        unsigned int nbchanDst = imgDest.channels();

        Label *labels = (Label *)imgLabel.ptr() + (miny * stepLbl);
        unsigned char *source = (unsigned char *)imgSourceCont.ptr() + (miny * stepSrc);
        unsigned char *imgData = (unsigned char *)imgDest.ptr() + (miny * stepDst);

        for (unsigned int r = miny; r < maxy; r++, labels += stepLbl, source += stepSrc, imgData += stepDst)
            for (unsigned int c = minx; c < maxx; c++) {
                if (labels[c] == label) {
                    imgData[nbchanDst*c+0] = (unsigned char)((1.-alpha)*source[nbchanSrc*c+0]+alpha*color.val[0]);
                    imgData[nbchanDst*c+1] = (unsigned char)((1.-alpha)*source[nbchanSrc*c+1]+alpha*color.val[1]);
                    imgData[nbchanDst*c+2] = (unsigned char)((1.-alpha)*source[nbchanSrc*c+2]+alpha*color.val[2]);
                }
            }
    }

    if (mode) {
        if (mode & CV_BLOB_RENDER_TO_LOG) {
            std::clog << "Blob " << label << std::endl;
            std::clog << " - Bounding box: (" << minx << ", " << miny << ") - (" << maxx << ", " << maxy << ")" << std::endl;
            std::clog << " - Bounding box get_Area(): " << (1 + maxx - minx) * (1 + maxy - miny) << std::endl;
            std::clog << " - Area: " << get_Area() << std::endl;
            std::clog << " - Centroid: (" << centroid.x << ", " << centroid.y << ")" << std::endl;
            std::clog << std::endl;
        }

        if (mode & CV_BLOB_RENDER_TO_STD) {
            std::cout << "Blob " << label << std::endl;
            std::cout << " - Bounding box: (" << minx << ", " << miny << ") - (" << maxx << ", " << maxy << ")" << std::endl;
            std::cout << " - Bounding box get_Area(): " << (1 + maxx - minx) * (1 + maxy - miny) << std::endl;
            std::cout << " - Area: " << get_Area() << std::endl;
            std::cout << " - Centroid: (" << centroid.x << ", " << centroid.y << ")" << std::endl;
            std::cout << std::endl;
        }

        if (mode & CV_BLOB_RENDER_BOUNDING_BOX)
            cv::rectangle(imgDest, cv::Point(minx, miny), cv::Point(maxx-1, maxy-1), CV_RGB(255., 0., 0.));

        if (mode & CV_BLOB_RENDER_ANGLE) {
            double angle = get_Angle();

            double x1,y1,x2,y2;
            double lengthLine = std::max(maxx-minx, maxy - miny) / 2.;

            x1=centroid.x-lengthLine*cos(angle);
            y1=centroid.y-lengthLine*sin(angle);
            x2=centroid.x+lengthLine*cos(angle);
            y2=centroid.y+lengthLine*sin(angle);
            cv::line(imgDest, cv::Point(int(x1), int(y1)), cv::Point(int(x2), int(y2)), CV_RGB(0.,255.,0.));
        }

        if (mode & CV_BLOB_RENDER_CENTROID) {
            cv::line(imgDest,
                cv::Point(int(centroid.x) - 3, int(centroid.y)),
                cv::Point(int(centroid.x) + 3, int(centroid.y)),
                CV_RGB(0.,0.,255.));
            cv::line(imgDest,
                cv::Point(int(centroid.x), int(centroid.y) - 3),
                cv::Point(int(centroid.x), int(centroid.y) + 3),
                CV_RGB(0.,0.,255.));
        }
    }

}

void Blob::Print(std::ostream &out) const {
    out << label << ": " << get_Area() << ", (" << centroid.x << ", " << centroid.y << "), [(" << minx << ", " << miny << ") - (" << maxx << ", " << maxy << ")]";
}

void Blob::Merge(Blob &a_blob) {
    label = std::min(label, a_blob.label);
    minx = std::min(a_blob.minx, minx);
    maxx = std::max(a_blob.maxx, maxx);
    miny = std::min(a_blob.miny, miny);
    maxy = std::max(a_blob.maxy, maxy);

    m00 += a_blob.m00;
    m10 += a_blob.m10;
    m01 += a_blob.m01;
    m11 += a_blob.m11;
    m20 += a_blob.m20;
    m02 += a_blob.m02;
}

std::ostream &operator<< (std::ostream &output, const Blob &b) {
    b.Print(output);
    return output;
}
