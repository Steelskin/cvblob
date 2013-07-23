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

/// \file cvb_blob.h
/// \brief OpenCV Blob header file.

#ifdef SWIG
%module cvblob
    %{
#include "cvb_blob.h"
        %}
#endif

#ifndef _CVBLOB_BLOB_H_
#define _CVBLOB_BLOB_H_

#include <iostream>
#include <cstdint>
#include <memory>

#include <opencv2/core/core.hpp>

#include "cvb_contour.h"
#include "cvb_defines.h"

#define CV_BLOB_RENDER_COLOR            0x0001 ///< Render each blog with a different color. \see RenderBlob
#define CV_BLOB_RENDER_CENTROID         0x0002 ///< Render centroid. \see RenderBlob
#define CV_BLOB_RENDER_BOUNDING_BOX     0x0004 ///< Render bounding box. \see RenderBlob
#define CV_BLOB_RENDER_ANGLE            0x0008 ///< Render angle. \see RenderBlob
#define CV_BLOB_RENDER_TO_LOG           0x0010 ///< Print blob data to log out. \see RenderBlob
#define CV_BLOB_RENDER_TO_STD           0x0020 ///< Print blob data to std out. \see RenderBlob

namespace cvb {

    /// \brief Type of Label.
    typedef uint8_t Label;
#define CVB_LABEL CV_8UC1
const Label MaxLabel = std::numeric_limits<Label>::max(); ///< Max Label value


    /// \brief Class that contains information about one blob.
    class CVBLOB_EXPORT Blob {
    public:
        /// \brief Constructor, with one dot.
        /// \param point   The blob first point.
        /// \param name    The blob label.
        Blob(cv::Point point, Label label);

        /// \brief Constructor, with one line.
        /// \param min_x   The line first point X-coordinate.
        /// \param max_x   The line last point X-coordinate.
        /// \param y       The line Y-coordinate.
        /// \param name    The blob label.
        Blob(unsigned int min_x, unsigned int max_x, unsigned int y, Label label);

        Label label; ///< Label.

        /// \brief  Gets the contour.
        /// \returns   The contour.
        Contour get_Contour() const;

        /// \brief Gets a copy of the internal contours.
        /// \return The internal contours.
        ContoursList get_InternalContours() const;

        /// \brief Gets the centroid.
        /// \return The Centroid.
        cv::Point2d get_Centroid() const;

        /// \brief Calculates angle orientation of a blob.
        /// \return Angle orientation in radians.
        double get_Angle() const;

        /// \brief Gets the blob area.
        /// \return The blob area.
        unsigned int get_Area() const;

        /// \brief Gets the blob bounding box.
        /// \return The blob bounding box.
        cv::Rect get_BoundingBox() const;

        /// \brief Adds a chain code to the blob contour.
        /// \param chainCode The chain code to be added.
        void add_ChainCode(ChainCode chainCode);

        /// \brief Adds a point to the blob moments.
        /// \param x X-coordinate of the point.
        /// \param y Y-coordinate of the point.
        void add_Moment(unsigned int x, unsigned int y);

        /// \brief Adds a line the the blob moments.
        /// \param min_x   The line first point X-coordinate.
        /// \param max_x   The line last point X-coordinate.
        /// \param y       The line Y-coordinate.
        void add_Moment(unsigned int min_x, unsigned int max_x, unsigned int y);

        /// \brief Adds a contour to the internal contours list.
        /// \param contour The contour to be added.
        void add_InternalContour(SharedContour contour);

        /// \brief Computes the central, normalized central and hu moments,
        /// along with the centroid.
        void ComputeMoments();

        /// \brief Calculates mean color of a blob in an image.
        /// \param imgLabel Image of labels.
        /// \param img Original image.
        /// \return Average color.
        cv::Scalar get_MeanColor(const cv::Mat &imgLabel, const cv::Mat &img) const;

        /// \brief Draws or prints information about a blob.
        /// \param imgLabel Label image (type = CV_16UC3 and continuous).
        /// \param imgSource Input image (type = CV_8UC3).
        /// \param imgDest Output image (type = CV_8UC3 and size identical to imgSource and continuous ).
        /// \param mode Render mode. By default is CV_BLOB_RENDER_COLOR|CV_BLOB_RENDER_CENTROID|CV_BLOB_RENDER_BOUNDING_BOX|CV_BLOB_RENDER_ANGLE.
        /// \param color Color to render (if CV_BLOB_RENDER_COLOR is used).
        /// \param alpha If mode CV_BLOB_RENDER_COLOR is used. 1.0 indicates opaque and 0.0 translucent (1.0 by default).
        /// \see CV_BLOB_RENDER_COLOR
        /// \see CV_BLOB_RENDER_CENTROID
        /// \see CV_BLOB_RENDER_BOUNDING_BOX
        /// \see CV_BLOB_RENDER_ANGLE
        /// \see CV_BLOB_RENDER_TO_LOG
        /// \see CV_BLOB_RENDER_TO_STD
        void RenderBlob(const cv::Mat &imgLabel, const cv::Mat &imgSource, cv::Mat &imgDest, unsigned short mode = 0x000f, cv::Scalar const &color = cv::Scalar(255, 255, 255), double alpha = 1.) const;

        /// \brief Save the image of a blob to a file.
        /// The function uses an image (that can be the original pre-processed image or a processed one, or even the result of cvRenderBlobs, for example).
        /// Then the function saves a copy of the part of the image where the blob is.
        /// \param filename Name of the file.
        /// \param img Image.
        void SaveImage(const std::string &filename, const cv::Mat &img) const;

        /// \brief Prints the data to the out stream
        /// \param out The output stream
        void Print(std::ostream &out) const;

        /// \brief Merges this blob with a_blob. </summary>
        /// \param The blob to merge with. </param>
        void Merge(Blob &a_blob);

    protected:
        unsigned int minx; ///< X min.
        unsigned int maxx; ///< X max.
        unsigned int miny; ///< Y min.
        unsigned int maxy; ///< Y max.

        unsigned int m00; ///< Moment 00 (area)
        double m01; ///< Moment 01.
        double m10; ///< Moment 10.
        double m11; ///< Moment 11.
        double m02; ///< Moment 02.
        double m20; ///< Moment 20.

        double u11; ///< Central moment 11.
        double u20; ///< Central moment 20.
        double u02; ///< Central moment 02.

        double n11; ///< Normalized central moment 11.
        double n20; ///< Normalized central moment 20.
        double n02; ///< Normalized central moment 02.

        double p1; ///< Hu moment 1.
        double p2; ///< Hu moment 2.

        cv::Point2d centroid;          ///< Centroid.
        Contour contour;               ///< Contour.
        ContoursList internalContours; ///< Internal contours.
    };

    typedef std::shared_ptr<Blob> SharedBlob; ///< Shared Blob type

    /// \brief Overload operator "<<" for printing blob structure.
    /// \return Stream.
    CVBLOB_EXPORT std::ostream &operator<< (std::ostream &output, const Blob &b);

} // Namespace

#endif // _CVBLOB_BLOB_H_
