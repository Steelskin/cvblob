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

/// \file cvb_aux.h
/// \brief cvBlob auxiliary functions file.

#ifdef SWIG
%module cvblob
    %{
#include "cvb_aux.h"
        %}
#endif

#ifndef _CVBLOB_AUX_H_
#define _CVBLOB_AUX_H_

#include <opencv2/core/core.hpp>

#include "cvb_defines.h"

namespace cvb {

    /// \brief Dot product of the vectors ab and bc.
    /// \param a First point.
    /// \param b Middle point.
    /// \param c Last point.
    /// \return Dot product of ab and bc.
    CVBLOB_EXPORT double DotProductPoints(const cv::Point &a, const cv::Point &b, const cv::Point &c);

    /// \brief Cross product of the vectors ab and bc.
    /// \param a Point.
    /// \param b Point.
    /// \param c Point.
    /// \return Cross product of ab and bc.
    CVBLOB_EXPORT double CrossProductPoints(const cv::Point &a, const cv::Point &b, const cv::Point &c);

    /// \brief Distance between two points.
    /// \param a Point.
    /// \param b Point.
    /// \return Distance.
    CVBLOB_EXPORT double DistancePointPoint(const cv::Point &a, const cv::Point &b);

    /// \brief Distance between line ab and point c.
    /// \param a First point of the segment.
    /// \param b Second point of the segment.
    /// \param c Point.
    /// \param isSegment If false then the distance will be calculated from the line defined by the points a and b, to the point c.
    /// \return Distance between ab and c.
    CVBLOB_EXPORT double DistanceLinePoint(const cv::Point &a, const cv::Point &b, const cv::Point &c, bool isSegment=true);

} // Namespace

#endif // _CVBLOB_AUX_H_