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

/// \file cvb_contour.h
/// \brief cvBlob contour header file.

#ifdef SWIG
%module cvblob
    %{
#include "cvb_contour.h"
        %}
#endif

#ifndef _CVBLOB_CONTOUR_H_
#define _CVBLOB_CONTOUR_H_

#include <list>
#include <memory>
#include <atomic>
#include <mutex>

#include <opencv2/core/core.hpp>

#include "cvb_defines.h"

namespace cvb {

    // Chain code:
    //        7 0 1
    //        6   2
    //        5 4 3
    CVBLOB_EXPORT enum ChainCode {
        ChainCode_up         = 0, ///< Up.
        ChainCode_up_right   = 1, ///< Up and right.
        ChainCode_right      = 2, ///< Right.
        ChainCode_down_right = 3, ///< Down and right.
        ChainCode_down       = 4, ///< Down.
        ChainCode_down_left  = 5, ///< Down and left.
        ChainCode_left       = 6, ///< Left.
        ChainCode_up_left    = 7, ///< Up and left.
    };

    /// \brief Chain code list.
    typedef std::list<ChainCode> ChainCodes;

    /// \brief Polygon based contour.
    typedef std::vector<cv::Point> ContourPolygon;

    /// \brief Contour class.
    class CVBLOB_EXPORT Contour {
    public:

        /// \brief Constructor.
        /// \param startingPoint  The contour starting point.
        /// \param chainCodes     Initial chain codes.
        Contour(cv::Point startingPoint = cv::Point(0, 0), ChainCodes &chainCodes = ChainCodes());

        /// \brief Constructor.
        /// \param x              The contour starting point X-coordinate.
        /// \param y              The contour starting point Y-coordinate.
        /// \param chainCodes     Initial chain codes.
        Contour(unsigned int x, unsigned int y, ChainCodes &chainCodes = ChainCodes());

        /// \brief  Clears this object and resets all internal data structures.
        /// \param startingPoint  The new starting point.
        /// \param chainCodes     The new initial chain codes.
        void reset(cv::Point startingPoint = cv::Point(0, 0), ChainCodes &chainCodes = ChainCodes());

        // \brief Adds a chain code to this object.
        // \param chainCode       The chain code to be added.
        void add_ChainCode(ChainCode chainCode);

        /// \brief Gets the contour starting point.
        /// \return The contour starting point.
        cv::Point get_StartingPoint() const;

        /// \brief Gets the list of chain codes.
        /// \return The list of chain codes
        ChainCodes get_ChainCodes() const;

        /// \brief Returns a polygon structure.
        /// \return A polygon.
        ContourPolygon get_ContourPolygon() const;

        /// \brief Calculates area of a contour.
        /// \return Area of the contour.
        double get_Area() const;

        /// \brief Calculates perimeter of a contour.
        /// \return Perimeter of the contour.
        double get_Perimeter() const;

        /// \brief Calculates the circularity of a contour (compactness measure).
        /// \return Circularity: a non-negative value, where 0 correspond with a circumference.
        double get_Circularity() const;

        /// \brief Calculates a simplified polygon structure reducing the number of vertex according the distance "delta".
        /// Uses a version of the Ramer-Douglas-Peucker algorithm (http://en.wikipedia.org/wiki/Ramer-Douglas-Peucker_algorithm).
        /// \param delta Minimun distance.
        /// \return A simplify version of the original polygon.
        ContourPolygon get_SimplifiedPolygon(const double delta = 1.) const;

        /// \brief Calculates convex hull of a contour.
        /// Uses the Melkman Algorithm. Code based on the version in http://w3.impa.br/~rdcastan/Cgeometry/.
        /// \return Convex hull.
        ContourPolygon get_ConvexHull() const;

        /// \brief Draw a contour.
        /// \param img Image to draw on, must be of type CV_8UC3, and continuous.
        /// \param color Color to draw with (default, white).
        void RenderContour(cv::Mat &img, const cv::Scalar &color = cv::Scalar(255, 255, 255)) const;

        /// \brief Write a contour to a CSV (Comma-separated values) file.
        /// \param filename File name.
        void WriteCSV(const std::string &filename) const;

        /// \brief Write a contour to a SVG file (http://en.wikipedia.org/wiki/Scalable_Vector_Graphics).
        /// \param filename File name.
        /// \param stroke Stroke color (black by default).
        /// \param fill Fill color (white by default).
        void WriteSVG(const std::string &filename, const cv::Scalar &stroke = cv::Scalar(0,0,0), const cv::Scalar &fill = cv::Scalar(255,255,255)) const;

        /// \brief Prints the data to the out stream in CSV format.
        /// \param out The output stream.
        void Print(std::ostream &out) const;

    protected:
        cv::Point starting_point;  ///< Point where contour begins.
        ChainCodes chain_codes;    ///< Polygon description based on chain codes.
        ContourPolygon polygon;    ///< Polygon description based on cv::Point.
    };

    typedef std::shared_ptr<Contour> SharedContour; ///< Shared contour.
    typedef std::list<SharedContour> ContoursList;  ///< List of contours (chain codes type).

    /// \brief Overload operator "<<" for printing polygons in CSV format.
    /// \return Stream.
    CVBLOB_EXPORT std::ostream &operator<< (std::ostream &output, const Contour &p);

} // Namespace

#endif // _CVBLOB_CONTOUR_H_