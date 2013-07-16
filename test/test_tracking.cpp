// Copyright (C) 2007 by Cristóbal Carnero Liñán
// grendel.ccl@gmail.com
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

#include <iostream>

#if (defined(_WIN32) || defined(__WIN32__) || defined(__TOS_WIN__) || defined(__WINDOWS__) || (defined(__APPLE__) & defined(__MACH__)))
#include <cv.h>
#include <highgui.h>
#else
#include <opencv/cv.h>
#include <opencv/highgui.h>
#endif

#include <cvblob.h>
using namespace cvb;

int main()
{
  CvTracks tracks;

  cvNamedWindow("test_tracking", CV_WINDOW_AUTOSIZE);

  CvCapture *capture=cvCreateFileCapture("EnterExitCrossingPaths2front_blobs.mpeg");

  cvGrabFrame(capture);
  IplImage *img = cvRetrieveFrame(capture);

  IplImage *frame = cvCreateImage(cvGetSize(img), img->depth, img->nChannels);

  while (cvGrabFrame(capture))
  {
    IplImage *img = cvRetrieveFrame(capture);

    cvResetImageROI(frame);
    cvConvertScale(img, frame, 1, 0);
    cvThreshold(frame, frame, 100, 200, CV_THRESH_BINARY);

    cvSetImageROI(frame, cvRect(0, 25, 383, 287));

    IplImage *chB=cvCreateImage(cvGetSize(frame),8,1);
    cvSplit(frame,chB,NULL,NULL,NULL);

    IplImage *labelImg = cvCreateImage(cvGetSize(frame), IPL_DEPTH_LABEL, 1);

    CvBlobs blobs;
    unsigned int result = cvLabel(chB, labelImg, blobs);

    cvFilterByArea(blobs, 500, 1000);

    cvUpdateTracks(blobs, tracks, 5., 10);
    //cvUpdateTracks(blobs, tracks, 10., 5);

    cvRenderBlobs(labelImg, blobs, frame, frame, CV_BLOB_RENDER_CENTROID|CV_BLOB_RENDER_BOUNDING_BOX);
    cvRenderTracks(tracks, frame, frame, CV_TRACK_RENDER_ID|CV_TRACK_RENDER_BOUNDING_BOX|CV_TRACK_RENDER_TO_LOG);

    cvShowImage("test_tracking", frame);

    cvReleaseBlobs(blobs);

    cvReleaseImage(&chB);
    cvReleaseImage(&labelImg);

    if ((cvWaitKey(10)&0xff)==27)
      break;
  }

  cvReleaseImage(&frame);

  cvDestroyWindow("test_tracking");

  return 0;
}
