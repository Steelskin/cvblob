﻿// Copyright (C) 2007 by Cristóbal Carnero Liñán
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
#include <opencv2/imgproc.hpp>

#include "cvb_track.h"

using namespace cvb;

Track::Track(TrackID id, SharedBlob blob) {
	this->id = id;
	this->label = blob->label;
	this->minx = blob->minx;
	this->miny = blob->miny;
	this->maxx = blob->maxx;
	this->maxy = blob->maxy;
	this->centroid = blob->centroid;
	this->lifetime = 0;
	this->active = 0;
	this->inactive = 0;
}

TrackID Track::get_ID() const {
	return this->id;
}

double Track::DistanceFromBlob(const Blob &b) const {
    double d1;
    cv::Point blob_centroid = b.get_Centroid();

    if (blob_centroid.x < this->minx) {
        if (blob_centroid.y < this->miny)
            d1 = std::max(this->minx - blob_centroid.x, this->miny - blob_centroid.y);
        else if (blob_centroid.y > this->maxy)
            d1 = std::max(this->minx - blob_centroid.x, blob_centroid.y - this->maxy);
        else // if (this->miny < blob_centroid.y)&&(blob_centroid.y < this->maxy)
            d1 = this->minx - blob_centroid.x;
    } else if (blob_centroid.x > this->maxx) {
        if (blob_centroid.y < this->miny)
            d1 = std::max(blob_centroid.x - this->maxx, this->miny - blob_centroid.y);
        else if (blob_centroid.y > this->maxy)
            d1 = std::max(blob_centroid.x - this->maxx, blob_centroid.y - this->maxy);
        else
            d1 = blob_centroid.x - this->maxx;
    } else { //if (this->minx =< blob_centroid.x) && (blob_centroid.x =< this->maxx)
        if (blob_centroid.y < this->miny)
            d1 = this->miny - blob_centroid.y;
        else if (blob_centroid.y > this->maxy)
            d1 = blob_centroid.y - this->maxy;
        else 
            return 0.;
    }

    double d2;
    cv::Rect bounding_box = b.get_BoundingBox();
    int blob_minx = bounding_box.x;
    int blob_miny = bounding_box.y;
    int blob_maxx = blob_minx + bounding_box.width;
    int blob_maxy = blob_miny + bounding_box.height;

    if (this->centroid.x < blob_minx) {
        if (this->centroid.y < blob_miny)
            d2 = std::max(blob_minx - this->centroid.x, blob_miny - this->centroid.y);
        else if (this->centroid.y > blob_maxy)
            d2 = std::max(blob_minx - this->centroid.x, this->centroid.y - blob_maxy);
        else // if (blob_miny < this->centroid.y)&&(this->centroid.y < blob_maxy)
            d2 = minx - this->centroid.x;
    } else if (this->centroid.x > blob_maxx) {
        if (this->centroid.y < blob_miny)
            d2 = std::max(this->centroid.x - blob_maxx, blob_miny - this->centroid.y);
        else if (this->centroid.y > blob_maxy)
            d2 = std::max(this->centroid.x - blob_maxx, this->centroid.y - blob_maxy);
        else
            d2 = this->centroid.x - blob_maxx;
    } else { // if (blob_minx =< this->centroid.x) && (this->centroid.x =< blob_maxx)
        if (this->centroid.y < blob_miny)
            d2 = blob_miny - this->centroid.y;
        else if (this->centroid.y > blob_maxy)
            d2 = this->centroid.y - blob_maxy;
        else 
            return 0.;
    }

    return std::min(d1, d2);
}

void Track::update_Blob(const SharedBlob &blob) {
	this->label = blob->label;
	this->centroid = blob->centroid;
	this->minx = blob->minx;
	this->miny = blob->miny;
	this->maxx = blob->maxx;
	this->maxy = blob->maxy;
	if (this->inactive)
		this->active = 0;
	this->inactive = 0;
}

void Track::IncreaseInactive() {
	this->inactive++;
	this->label = 0;
}

bool Track::IsTooOld(unsigned int thInactive, unsigned int thActive) const {
	return (this->inactive >= thInactive) || ((this->inactive) && (thActive) && (this->active < thActive));
}

void Track::IncreaseLifetime() {
	this->lifetime++;
	if (!this->inactive)
		this->active++;
}

// Access to matrix
//#define C(blob, track) close[((blob) + (track)*(nBlobs+2))]

// Access to accumulators
//#define AB(label) C((label), (nTracks))
//#define AT(id) C((nBlobs), (id))

// Access to identifications
#define IB(label) C((label), (nTracks)+1, close, nBlobs)
#define IT(id) C((nBlobs)+1, (id), close, nBlobs)

// Access to registers
#define B(label) blob_map.find(IB(label))->second
#define T(id) tracks.find(IT(id))->second

inline TrackID &C(size_t blob, size_t track, std::vector<TrackID> &close, size_t nBlobs) {
    return close[blob + track * (nBlobs + 2)];
}

inline TrackID &AB(unsigned int label, size_t nTracks, std::vector<TrackID> &close, size_t nBlobs) {
    return C(label, nTracks, close, nBlobs);
}

inline TrackID &AT(unsigned int id, size_t nTracks, std::vector<TrackID> &close, size_t nBlobs) {
    return C(nBlobs, id, close, nBlobs);
}


void TrackList::getClusterForBlob(unsigned int blobPos, std::vector<TrackID> &close, unsigned int nBlobs, unsigned int nTracks, const BlobsMap &blob_map, std::list<SharedBlob> &bb, std::list<SharedTrack> &tt) const {
    for (unsigned int j = 0; j < nTracks; j++) {
        if (C(blobPos, j, close, nBlobs)) {
            tt.push_back(T(j));

            unsigned int c = AT(j, nTracks, close, nBlobs);

            C(blobPos, j, close, nBlobs) = 0;
            AB(blobPos, nTracks, close, nBlobs)--;
            AT(j, nTracks, close, nBlobs)--;

            if (c>1) {
                getClusterForTrack(j, close, nBlobs, nTracks, blob_map, bb, tt);
            }
        }
    }
}

void TrackList::getClusterForTrack(unsigned int trackPos, std::vector<TrackID> &close, unsigned int nBlobs, unsigned int nTracks, const BlobsMap &blob_map, std::list<SharedBlob> &bb, std::list<SharedTrack> &tt) const {
    for (unsigned int i=0; i<nBlobs; i++) {
        if (C(i, trackPos, close, nBlobs)) {
            bb.push_back(B(i));

            unsigned int c = AB(i, nTracks, close, nBlobs);

            C(i, trackPos, close, nBlobs) = 0;
            AB(i, nTracks, close, nBlobs)--;
            AT(trackPos, nTracks, close, nBlobs)--;

            if (c>1) {
                getClusterForBlob(i, close, nBlobs, nTracks, blob_map, bb, tt);
            }
        }
    }
}

void TrackList::UpdateTracks(const BlobList &blobs, const double thDistance, const unsigned int thInactive, const unsigned int thActive) {
    auto blob_map = blobs.get_BlobsMap();
    size_t nBlobs = blob_map.size();
    size_t nTracks = tracks.size();

    // Proximity matrix:
    // Last row/column is for ID/label.
    // Last-1 "/" is for accumulation.
    std::vector<TrackID> close((nBlobs + 2) * (nTracks + 2));

    try {
        // Initialization
        unsigned int i=0;
        for (auto & a_blob : blob_map) {
            AB(i, nTracks, close, nBlobs) = 0;
            IB(i) = a_blob.second->label;
            i++;
        }

        TrackID maxTrackID = 0;

        unsigned int j = 0;
        for (auto &a_track : tracks) {
            AT(j, nTracks, close, nBlobs) = 0;
            IT(j) = a_track.second->get_ID();
            if (a_track.second->get_ID() > maxTrackID)
                maxTrackID = a_track.second->get_ID();
            j++;
        }

        // Proximity matrix calculation and "used blob" std::list inicialization:
        for (i = 0; i < nBlobs; i++)
            for (j = 0; j < nTracks; j++)
                if (C(i, j, close, nBlobs) = (T(j)->DistanceFromBlob(*B(i)) < thDistance)) {
                    AB(i, nTracks, close, nBlobs)++;
                    AT(j, nTracks, close, nBlobs)++;
                }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // Detect inactive tracks
        for (j=0; j<nTracks; j++) {
            unsigned int c = AT(j, nTracks, close, nBlobs);

            if (c==0) {
                //std::cout << "Inactive track: " << j << std::endl;

                // Inactive track.
                SharedTrack track = T(j);
                track->inactive++;
                track->label = 0;
            }
        }

        // Detect new tracks
        for (i=0; i<nBlobs; i++) {
            unsigned int c = AB(i, nTracks, close, nBlobs);

            if (c==0) {
                //std::cout << "Blob (new track): " << maxTrackID+1 << std::endl;
                //std::cout << *B(i) << std::endl;

                // New track.
                maxTrackID++;
                SharedBlob blob = B(i);
                SharedTrack track(new Track(maxTrackID, blob));
                tracks.insert(IDTrackPair(maxTrackID, track));
            }
        }

        // Clustering
        for (j = 0; j < nTracks; j++) {
            unsigned int c = AT(j, nTracks, close, nBlobs);

            if (c) {
                std::list<SharedTrack> tt; tt.push_back(T(j));
                std::list<SharedBlob> bb;

                getClusterForTrack(j, close, (unsigned int) nBlobs, (unsigned int) nTracks, blob_map, bb, tt);

                // Select track
                SharedTrack track;
                unsigned int area = 0;
                for (auto &a_track : tt) {
                    // Get area
                    unsigned int a = (a_track->maxx - a_track->minx) * (a_track->maxy - a_track->miny);
                    if (a > area) {
                        area = a;
                        track = a_track;
                    }
                }

                // Select blob
                SharedBlob blob;
                area = 0;
                //std::cout << "Matching blobs: ";
                for (auto &a_blob : bb) {
                    //std::cout << a_blob->label << " ";

                    if (a_blob->get_Area() > area) {
                        area = a_blob->get_Area();
                        blob = a_blob;
                    }
                }
                //std::cout << std::endl;

                // Update track
                //std::cout << "Matching: track=" << track->get_ID() << ", blob=" << blob->label << std::endl;
                track->update_Blob(blob);

                // Others to inactive
                for (auto &a_track : tt) {
                    if (a_track != track) {
                        //std::cout << "Inactive: track=" << t->get_ID() << std::endl;
                        a_track->IncreaseInactive();
                    }
                }
            }
        }
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        for (auto &jt = tracks.begin(); jt != tracks.end();)
            if (jt->second->IsTooOld(thInactive, thActive)) {
                tracks.erase(jt++);
            } else {
                jt->second->IncreaseLifetime();
                ++jt;
            }
    } catch (...) {
        throw; // TODO: OpenCV style.
    }
}

void TrackList::RenderTracks(cv::Mat &imgSource, cv::Mat &imgDest, unsigned short mode, int font) const {
    CV_Assert(imgDest.type() == CV_8UC3);

    if (mode) {
        for (auto &a_track : tracks) {
            if (mode & CV_TRACK_RENDER_ID) {
                if (!a_track.second->inactive) {
                    std::stringstream buffer;
                    buffer << a_track.first;
                    cv::putText(imgDest, buffer.str(), cv::Point((int) a_track.second->centroid.x, (int) a_track.second->centroid.y), font, 1, cv::Scalar(0., 255., 0.));
                }
            }

            if (mode & CV_TRACK_RENDER_BOUNDING_BOX) {
                if (a_track.second->inactive)
                    cv::rectangle(imgDest, cv::Point(a_track.second->minx, a_track.second->miny), cv::Point(a_track.second->maxx - 1, a_track.second->maxy - 1), cv::Scalar(0., 0., 50.));
                else
                    cv::rectangle(imgDest, cv::Point(a_track.second->minx, a_track.second->miny), cv::Point(a_track.second->maxx - 1, a_track.second->maxy - 1), cv::Scalar(0., 0., 255.));
            }

            if (mode & CV_TRACK_RENDER_TO_LOG) {
                std::clog << "Track " << a_track.second->get_ID() << std::endl;
                if (a_track.second->inactive)
                    std::clog << " - Inactive for " << a_track.second->inactive << " frames" << std::endl;
                else
                    std::clog << " - Associated with blob " << a_track.second->label << std::endl;
                std::clog << " - Lifetime " << a_track.second->lifetime << std::endl;
                std::clog << " - Active " << a_track.second->active << std::endl;
                std::clog << " - Bounding box: (" << a_track.second->minx << ", " << a_track.second->miny << ") - (" << a_track.second->maxx << ", " << a_track.second->maxy << ")" << std::endl;
                std::clog << " - Centroid: (" << a_track.second->centroid.x << ", " << a_track.second->centroid.y << ")" << std::endl;
                std::clog << std::endl;
            }

            if (mode & CV_TRACK_RENDER_TO_STD) {
                std::cout << "Track " << a_track.second->get_ID() << std::endl;
                if (a_track.second->inactive)
                    std::cout << " - Inactive for " << a_track.second->inactive << " frames" << std::endl;
                else
                    std::cout << " - Associated with blobs " << a_track.second->label << std::endl;
                std::cout << " - Lifetime " << a_track.second->lifetime << std::endl;
                std::cout << " - Active " << a_track.second->active << std::endl;
                std::cout << " - Bounding box: (" << a_track.second->minx << ", " << a_track.second->miny << ") - (" << a_track.second->maxx << ", " << a_track.second->maxy << ")" << std::endl;
                std::cout << " - Centroid: (" << a_track.second->centroid.x << ", " << a_track.second->centroid.y << ")" << std::endl;
                std::cout << std::endl;
            }
        }
    }
}

