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

/// \file cvb_track.h
/// \brief OpenCV Blob header file.

#ifdef SWIG
%module cvblob
    %{
#include "cvb_track.h"
        %}
#endif

#ifndef _CVBLOB_TRACK_H_
#define _CVBLOB_TRACK_H_

#include <cstdint>
#include <memory>

#include "cvb_blob_list.h"

namespace cvb {
#define CV_TRACK_RENDER_ID            0x0001 ///< Print the ID of each track in the image. \see cvRenderTracks
#define CV_TRACK_RENDER_BOUNDING_BOX  0x0002 ///< Draw bounding box of each track in the image. \see cvRenderTracks
#define CV_TRACK_RENDER_TO_LOG        0x0010 ///< Print track info to log out. \see cvRenderTracks
#define CV_TRACK_RENDER_TO_STD        0x0020 ///< Print track info to log out. \see cvRenderTracks

    /// \brief Type of identification numbers.
    typedef uint32_t TrackID;

    /// \brief Struct that contain information about one track.
    class Track {
    public:
        Track(SharedBlob blob);

        TrackID get_ID();
        double DistanceFromBlob(const Blob &b) const;

    protected:
        TrackID id; ///< Track identification number.
        Label label; ///< Label assigned to the blob related to this track.

        unsigned int minx; ///< X min.
        unsigned int maxx; ///< X max.
        unsigned int miny; ///< Y min.
        unsigned int maxy; ///< y max.

        cv::Point centroid; ///< Centroid.

        unsigned int lifetime; ///< Indicates how much frames the object has been in scene.
        unsigned int active; ///< Indicates number of frames that has been active from last inactive period.
        unsigned int inactive; ///< Indicates number of frames that has been missing.
    };

    typedef std::shared_ptr<Track> SharedTrack;

    /// \var typedef std::map<TrackID, SharedTrack> TrackList
    /// \brief List of tracks.
    /// \see TrackID
    /// \see Track
    typedef std::map<TrackID, SharedTrack> TracksMap;

    /// \var typedef std::pair<TrackID, SharedTrack> CvIDTrack
    /// \brief Pair (identification number, track).
    /// \see TrackID
    /// \see Track
    typedef std::pair<TrackID, SharedTrack> IDTrackPair;

    // TODO MOve me to cvb_track_list
    class TrackList {
    public:
        /// \fn UpdateTracks(const BlobList &b, const double thDistance, const unsigned int thInactive, const unsigned int thActive = 0)
        /// \brief Updates list of tracks based on current blobs.
        /// Tracking based on:
        /// A. Senior, A. Hampapur, Y-L Tian, L. Brown, S. Pankanti, R. Bolle. Appearance Models for
        /// Occlusion Handling. Second International workshop on Performance Evaluation of Tracking and
        /// Surveillance Systems & CVPR'01. December, 2001.
        /// (http://www.research.ibm.com/peoplevision/PETS2001.pdf)
        /// \param b List of blobs.
        /// \param thDistance Max distance to determine when a track and a blob match.
        /// \param thInactive Max number of frames a track can be inactive.
        /// \param thActive If a track becomes inactive but it has been active less than thActive frames, the track will be deleted.
        /// \see BlobList
        /// \see Tracks
        void UpdateTracks(const BlobList &b, const double thDistance, const unsigned int thInactive, const unsigned int thActive = 0);

        /// \fn void RenderTracks(cv::Mat &imgSource, cv::Mat &imgDest, unsigned short mode=0x00ff, int font = cv::FONT_HERSHEY_DUPLEX) const
        /// \brief Prints tracks information.
        /// \param imgSource Input image (depth=IPL_DEPTH_8U and num. channels=3).
        /// \param imgDest Output image (depth=IPL_DEPTH_8U and num. channels=3).
        /// \param mode Render mode. By default is CV_TRACK_RENDER_ID|CV_TRACK_RENDER_BOUNDING_BOX.
        /// \param font OpenCV font for print on the image.
        /// \see CV_TRACK_RENDER_ID
        /// \see CV_TRACK_RENDER_BOUNDING_BOX
        /// \see CV_TRACK_RENDER_TO_LOG
        /// \see CV_TRACK_RENDER_TO_STD
        void RenderTracks(cv::Mat &imgSource, cv::Mat &imgDest, unsigned short mode=0x000f, int font = cv::FONT_HERSHEY_DUPLEX) const;

    protected:
        TracksMap tracks;

    };



    /// \fn std::ostream &operator<< (std::ostream &output, const cvb::Track &t)
    /// \brief Overload operator "<<" for printing track structure.
    /// \return Stream.
    std::ostream &operator<< (std::ostream &output, const Track &t);


} // Namespace

#endif // _CVBLOB_TRACK_H_