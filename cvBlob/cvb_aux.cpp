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

#include "cvb_aux.h"

using namespace cvb;

// http://www.topcoder.com/tc?module=Static&d1=tutorials&d2=geometry1
double cvb::DotProductPoints(const cv::Point &a, const cv::Point &b, const cv::Point &c) {
    double abx = b.x-a.x;
    double aby = b.y-a.y;
    double bcx = c.x-b.x;
    double bcy = c.y-b.y;

    return abx*bcx + aby*bcy;
}

double cvb::CrossProductPoints(const cv::Point &a, const cv::Point &b, const cv::Point &c) {
    double abx = b.x-a.x;
    double aby = b.y-a.y;
    double acx = c.x-a.x;
    double acy = c.y-a.y;

    return abx*acy - aby*acx;
}

double cvb::DistancePointPoint(const cv::Point &a, const cv::Point &b) {
    double abx = a.x-b.x;
    double aby = a.y-b.y;

    return sqrt(abx*abx + aby*aby);
}

double cvb::DistanceLinePoint(const cv::Point &a, const cv::Point &b, const cv::Point &c, bool isSegment) {
    if (isSegment) {
        double dot1 = cvb::DotProductPoints(a, b, c);
        if (dot1>0)
            return cvb::DistancePointPoint(b, c);

        double dot2 = cvb::DotProductPoints(b, a, c);
        if(dot2>0)
            return cvb::DistancePointPoint(a, c);
    }

    return fabs(cvb::CrossProductPoints(a,b,c) / cvb::DistancePointPoint(a,b));
}
