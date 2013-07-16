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

#include <climits>

#ifdef _USE_MATH_DEFINES
# include <cmath>
#else
# define _USE_MATH_DEFINES
# include <cmath>
# undef _USE_MATH_DEFINES
#endif

#include <deque>
#include <iostream>
#include <fstream>
#include <sstream>

#include "cvb_contour.h"
#include "cvb_aux.h"

using namespace cvb;

#ifdef M_PI
const double pi = M_PI;
#else
const double pi = std::atan(1.) * 4.;
#endif // M_PI


/// \brief Move vectors of chain codes.
const cv::Point ChainCodeMoves[8] = {
    cv::Point(0, -1),
    cv::Point(1, -1),
    cv::Point(1,  0),
    cv::Point(1,  1),
    cv::Point(0,  1),
    cv::Point(-1,  1),
    cv::Point(-1,  0),
    cv::Point(-1, -1)
};

Contour::Contour(cv::Point startingPoint, ChainCodes &chainCodes) {
    reset(startingPoint, chainCodes);
}

// TODO Update to delegating constructore in VC2013
Contour::Contour(unsigned int x, unsigned int y, ChainCodes &chainCodes) {
    reset(cv::Point(x, y), chainCodes);
}

void Contour::reset(cv::Point startingPoint, ChainCodes &chainCodes) {
    starting_point = startingPoint;
    chain_codes = chainCodes;
    polygon.clear();

    // Initializes polygon structure

    // Push first point
    cv::Point a_point = starting_point;
    polygon.push_back(a_point);

    if (chain_codes.size() != 0) {
        ChainCode lastCode = chain_codes.front();
        for (auto &a_chain_code : chain_codes) {
            if (lastCode != a_chain_code) {
                // Direction change, add new point
                polygon.push_back(a_point);
                lastCode = a_chain_code;
            }
            // Update point position
            a_point += ChainCodeMoves[a_chain_code];
        }
    }

}

void Contour::add_ChainCode(ChainCode chainCode) {
    // Update polygon structure
    if (chain_codes.empty()) {
        // First element added, add one end point
        polygon.push_back(starting_point + ChainCodeMoves[chainCode]);
    } else {
        ChainCode lastCode = chain_codes.back();
        if (lastCode == chainCode)
            // Same direction, change end point
            polygon.back() += ChainCodeMoves[chainCode];
        else
            // Different direction, add a new point
            polygon.push_back(polygon.back() + ChainCodeMoves[chainCode]);
    }

    // Add chainCode to list
    this->chain_codes.push_back(chainCode);
}

cv::Point Contour::get_StartingPoint() const {
    return starting_point;
}

ChainCodes Contour::get_ChainCodes() const {
    return chain_codes;
}

ContourPolygon Contour::get_ContourPolygon() const {
    return polygon;
}

double Contour::get_Area() const {
    if (polygon.size() <= 2)
        return 1.;

    cv::Point lastPoint = polygon.back();
    double a = 0.;
    for (auto &a_point : polygon) {
        a += lastPoint.x * a_point.y - lastPoint.y * a_point.x;
        lastPoint = a_point;
    }

    return a * 0.5;
}

double Contour::get_Perimeter() const {
    double perimeter = 0.;
    for (auto &a_chain_code : chain_codes) {
        if (a_chain_code % 2)
            perimeter += std::sqrt(1. + 1.);
        else
            perimeter += 1.;
    }

    return perimeter;
}

double Contour::get_Circularity() const {
    double l = get_Perimeter();
    double c = (l * l / get_Area()) - 4. * pi;

    if (c >= 0.)
        return c;
    else // This could happen if the blob is only a pixel: the perimeter will be 0. Another solution would be to force "Perimeter" to be 1 or greater.
        return 0.;
}

// Utility function
void simplifyPolygonRecursive(const ContourPolygon &p, int const i1, int const i2, std::vector<bool> &pnUseFlag, const double delta) {
    int endIndex = (i2 < 0) ? (int) p.size() : i2;

    if (std::abs((int)(i1 - endIndex))<=1)
        return;

    cv::Point firstPoint = p[i1];
    cv::Point lastPoint = (i2<0)?p.front():p[i2];

    double furtherDistance = 0.;
    int furtherIndex = 0;

    for (int i = i1 + 1; i < endIndex; i++) {
        double d = DistanceLinePoint(firstPoint, lastPoint, p[i]);

        if ((d >= delta) && (d > furtherDistance)) {
            furtherDistance = d;
            furtherIndex = i;
        }
    }

    if (furtherIndex) {
        pnUseFlag[furtherIndex]=true;

        simplifyPolygonRecursive(p, i1, furtherIndex, pnUseFlag, delta);
        simplifyPolygonRecursive(p, furtherIndex, i2, pnUseFlag, delta);
    }
}

ContourPolygon Contour::get_SimplifiedPolygon(const double delta) const {
    double furtherDistance = 0.;
    unsigned int furtherIndex = 0;

    unsigned int i = 1;
    for (auto &a_point : polygon) {
        double d = DistancePointPoint(a_point, polygon.front());

        if (d>furtherDistance) {
            furtherDistance = d;
            furtherIndex = i;
        }
    }

    if (furtherDistance<delta) {
        ContourPolygon result;
        result.push_back(polygon.front());
        return result;
    }

    std::vector<bool> pnUseFlag(polygon.size(), false);

    pnUseFlag[0] = pnUseFlag[furtherIndex] = true;

    simplifyPolygonRecursive(polygon, 0, furtherIndex, pnUseFlag, delta);
    simplifyPolygonRecursive(polygon, furtherIndex, -1, pnUseFlag, delta);

    ContourPolygon result;

    for (unsigned int i = 0; i < polygon.size(); i++) {
        if (pnUseFlag[i])
            result.push_back(polygon[i]);
    }

    return result;
}

ContourPolygon Contour::get_ConvexHull() const {
    if (polygon.size() <= 3) {
        return ContourPolygon(polygon.begin(), polygon.end());
    }

    std::deque<cv::Point> dq;

    if (CrossProductPoints(polygon[0], polygon[1], polygon[2])>0) {
        dq.push_back(polygon[0]);
        dq.push_back(polygon[1]);
    } else {
        dq.push_back(polygon[1]);
        dq.push_back(polygon[0]);
    }

    dq.push_back(polygon[2]);
    dq.push_front(polygon[2]);

    for (unsigned int i = 3; i < polygon.size(); i++) {
        size_t s = dq.size();

        if ((CrossProductPoints(polygon[i], dq.at(0), dq.at(1))>=0) && (CrossProductPoints(dq.at(s-2), dq.at(s-1), polygon[i])>=0))
            continue; // TODO Optimize.

        while (CrossProductPoints(dq.at(s-2), dq.at(s-1), polygon[i]) < 0) {
            dq.pop_back();
            s = dq.size();
        }

        dq.push_back(polygon[i]);

        while (CrossProductPoints(polygon[i], dq.at(0), dq.at(1))<0)
            dq.pop_front();

        dq.push_front(polygon[i]);
    }

    return ContourPolygon(dq.begin(), dq.end());
}


void Contour::RenderContour(cv::Mat &img, const cv::Scalar &color) const {
    CV_Assert(img.type() == CV_8UC3 && img.isContinuous());

    size_t stepDst = img.step;
    unsigned int img_width = img.cols;
    unsigned int img_height = img.rows;

    unsigned char *imgData = (unsigned char *)img.ptr();

    cv::Point point = starting_point;
    for (auto &a_chain_code : chain_codes) {
        imgData[point.y * stepDst + point.x * 3 + 0] = (unsigned char)(color.val[0]); // Blue
        imgData[point.y * stepDst + point.x * 3 + 1] = (unsigned char)(color.val[1]); // Green
        imgData[point.y * stepDst + point.x * 3 + 2] = (unsigned char)(color.val[1]); // Red

        point += ChainCodeMoves[a_chain_code];
    }
}

void Contour::WriteCSV(const std::string &filename) const {
    std::ofstream f(filename);
    f << this << std::endl;

    f.close();
}

void Contour::WriteSVG(const std::string &filename, const cv::Scalar &stroke, const cv::Scalar &fill) const {
    int minx=INT_MAX;
    int miny=INT_MAX;
    int maxx=INT_MIN;
    int maxy=INT_MIN;

    std::stringstream buffer("");

    for (auto &a_point : polygon) {
        if (a_point.x > maxx)
            maxx = a_point.x;
        if (a_point.x < minx)
            minx = a_point.x;

        if (a_point.y > maxy)
            maxy = a_point.y;
        if (a_point.y < miny)
            miny = a_point.y;

        buffer << a_point.x << "," << a_point.y << " ";
    }

    std::ofstream f (filename);

    f << "<?xml version=\"1.0\" encoding=\"ISO-8859-1\" standalone=\"no\"?>" << std::endl;
    f << "<!DOCTYPE svg PUBLIC \"-//W3C//DTD SVG 20010904//EN\" \"http://www.w3.org/TR/2001/REC-SVG-20010904/DTD/svg10.dtd\">" << std::endl;
    f << "<svg xmlns=\"http://www.w3.org/2000/svg\" xmlns:xlink=\"http://www.w3.org/1999/xlink\" xml:space=\"preserve\" width=\"" << maxx-minx << "px\" height=\"" << maxy-miny << "px\" viewBox=\"" << minx << " " << miny << " " << maxx << " " << maxy << "\" zoomAndPan=\"disable\" >" << std::endl;

    f << "<polygon fill=\"rgb(" << fill.val[0] << "," << fill.val[1] << "," << fill.val[2] << ")\" stroke=\"rgb(" << stroke.val[0] << "," << stroke.val[1] << "," << stroke.val[2] << ")\" stroke-width=\"1\" points=\"" << buffer.str() << "\"/>" << std::endl;

    f << "</svg>" << std::endl;

    f.close();
}

void Contour::Print(std::ostream &out) const {
    for (auto &a_point : polygon)
        out << a_point.x << ", " << a_point.y << std::endl;
}

std::ostream &operator<< (std::ostream &output, const Contour &p) {
    p.Print(output);
    return output;
}
