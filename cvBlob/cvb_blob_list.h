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

/// \file cvb_blob_list.h
/// \brief OpenCV Blob List header file.

#ifdef SWIG
%module cvblob
    %{
#include "cvb_blob_list.h"
        %}
#endif

#ifndef _CVBLOB_BLOB_LIST_H_
#define _CVBLOB_BLOB_LIST_H_

#include <map>
#include <list>
#include <vector>
#include <limits>

#include "cvb_blob.h"
#include "cvb_defines.h"

namespace cvb {

    /// \brief List of blobs.
    /// A map is used to access each blob from its label number.
    /// \see Label
    /// \see CvBlob
    typedef std::map<Label, SharedBlob> BlobsMap;

    /// \brief Pair (label, blob).
    /// \see Label
    /// \see CvBlob
    typedef std::pair<Label, SharedBlob> LabelBlob;

    /// \brief Class defining a list of blobs, along with labelling
    class CVBLOB_EXPORT BlobList {
    public:
        BlobList(); ///< Default constructor.

        /// \brief Label the connected parts of a binary image.
        /// Simple, fast algorithm. Does not compute contours.
        /// \param img Input binary image (type = CV_8UC1).
        void SimpleLabel(const cv::Mat &img, Label max_label = MaxLabel);

        /// \brief Label the connected parts of a binary image.
        /// Algorithm based on paper "A linear-time component-labeling algorithm using contour tracing technique" of Fu Chang, Chun-Jen Chen and Chi-Jen Lu.
        /// \param img Input binary image (type = CV_8UC1).
        void LabelImage (const cv::Mat &img, Label max_label = MaxLabel);

        /// \brief Draw a binary image with the blobs.
        /// \param imgOut Output binary image (type = CV8UC1 and continuous).
        void FilterLabels(cv::Mat &imgOut) const;

        /// \brief Gets a copy of the blobs list.
        /// \return The blobs list.
        std::list<SharedBlob> get_BlobsList() const;

        /// \brief Gets the labelled picture.
        /// \return The labelled picture.
        cv::Mat get_ImageLabel() const;

        /// \brief Gets the label at coordinates (x, y)
        /// \param x X coordinate.
        /// \param y Y coordinate.
        /// \return Label value.
        Label GetLabel(unsigned int x, unsigned int y) const;

        /// \brief Find largest blob (biggest area).
        /// \return Largest blob, empty if no blob computed.
        SharedBlob get_LargestBlob() const;

        /// \brief Filter blobs by area.
        /// Those blobs whose areas are not in range will be erased from the internal list of blobs.
        /// \param minArea Minimun area.
        /// \param maxArea Maximun area.
        void FilterByArea(unsigned int minArea, unsigned int maxArea = std::numeric_limits<unsigned int>::max());

        /// \brief Filter blobs by label.
        /// Delete all blobs except those with label l.
        /// \param label Label to leave.
        void FilterByLabel(Label label);

        /// \brief Draws or prints information about blobs.
        /// \param imgSource Input image (type = CV_8UC3).
        /// \param imgDest Output image (type = CV_8UC3 and is continuous).
        /// \param mode Render mode. By default is CV_BLOB_RENDER_COLOR|CV_BLOB_RENDER_CENTROID|CV_BLOB_RENDER_BOUNDING_BOX|CV_BLOB_RENDER_ANGLE.
        /// \param alpha If mode CV_BLOB_RENDER_COLOR is used. 1.0 indicates opaque and 0.0 translucent (1.0 by default).
        /// \see CV_BLOB_RENDER_COLOR
        /// \see CV_BLOB_RENDER_CENTROID
        /// \see CV_BLOB_RENDER_BOUNDING_BOX
        /// \see CV_BLOB_RENDER_ANGLE
        /// \see CV_BLOB_RENDER_TO_LOG
        /// \see CV_BLOB_RENDER_TO_STD
        void RenderBlobs(const cv::Mat &imgSource, cv::Mat &imgDest, unsigned short mode = 0x000f, double alpha = 1.) const;

    protected:
        cv::Mat imgLabel;            ///< Labelled image
        std::list<SharedBlob> blobs; ///< Blobs list
    };


} // Namespace

#endif // _CVBLOB_LIST_H_