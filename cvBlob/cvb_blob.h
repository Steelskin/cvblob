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

    /// \brief Class that contains information abotu one blob.
    class CVBLOB_EXPORT Blob {
    public:
        Blob(cv::Point point, Label label);
        Blob(unsigned int min_x, unsigned int max_x, unsigned int y, Label label);

        Label label; ///< Label

        Contour get_Contour() const;
        ContoursList get_InternalContours() const;

        cv::Point2d get_Centroid() const;

        /// \fn double get_Angle() const
        /// \brief Calculates angle orientation of a blob.
        /// \return Angle orientation in radians.
        /// \see CvBlob
        double get_Angle() const;

        unsigned int get_Area() const;

        /// \fn void get_BoundingBox() const
        /// \brief Returns the blob bounding box
        cv::Rect get_BoundingBox() const;

        void add_ChainCode(ChainCode chainCode);
        void add_Moment(unsigned int x, unsigned int y);
        void add_Moments(unsigned int min_x, unsigned int max_x, unsigned int y);
        void add_InternalContour(SharedContour contour);

        /// \fn void ComputeMoments()
        /// \brief Computes the central, normalized central and hu moments, along with the centroid
        void ComputeMoments();

        /// \fn cv::Scalar get_BlobMeanColor(const cv::Mat &imgLabel, const cv::Mat &img) const
        /// \brief Calculates mean color of a blob in an image.
        /// \param imgLabel Image of labels.
        /// \param img Original image.
        /// \return Average color.
        cv::Scalar get_MeanColor(const cv::Mat &imgLabel, const cv::Mat &img) const;


        /// \fn void RenderBlob(const cv::Mat &imgLabel, cv::Mat &imgSource, cv::Mat &imgDest, unsigned short mode = 0x000f, cv::Scalar const &color = cv::Scalar(255, 255, 255), double alpha = 1.) const
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

        /// \fn void SaveImage(std::string &filename, cv::Mat &img) const
        /// \brief Save the image of a blob to a file.
        /// The function uses an image (that can be the original pre-processed image or a processed one, or even the result of cvRenderBlobs, for example).
        /// Then the function saves a copy of the part of the image where the blob is.
        /// \param filename Name of the file.
        /// \param img Image.
        void SaveImage(const std::string &filename, const cv::Mat &img) const;

        /// \fn void Print(std::ostream &out)
        /// \brief Prints the data to the out stream
        /// \param out The output stream
        void Print(std::ostream &out) const;

        /// \fn void Merge(Blob &a_blob);
        /// \brief Merges this blob with a_blob. </summary>
        ///
        /// \param The blob to merge with. </param>
        void Merge(Blob &a_blob);

#pragma warning(push)
#pragma warning(disable : 4251)
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
#pragma warning(pop)
    };

    typedef std::shared_ptr<Blob> SharedBlob; ///< Shared Blob type

    /// \fn std::ostream &operator<< (std::ostream &output, const Blob &b)
    /// \brief Overload operator "<<" for printing blob structure.
    /// \return Stream.
    CVBLOB_EXPORT std::ostream &operator<< (std::ostream &output, const Blob &b);

} // Namespace

#endif // _CVBLOB_BLOB_H_
