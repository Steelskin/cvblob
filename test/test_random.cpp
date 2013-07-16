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
using namespace std;

#if (defined(_WIN32) || defined(__WIN32__) || defined(__TOS_WIN__) || defined(__WINDOWS__) || (defined(__APPLE__) & defined(__MACH__)))
#include <cv.h>
#include <highgui.h>
#else
#include <opencv/cv.h>
#include <opencv/highgui.h>
#endif

#ifndef _WIN32
#include <sys/time.h>
#else
#include <ctime>
#include <windows.h>

int gettimeofday (struct timeval *tv, void* tz)
{
  union
  {
    long long ns100;
    FILETIME ft;
  } now;

  GetSystemTimeAsFileTime (&now.ft);
  tv->tv_usec = (long) ((now.ns100 / 10LL) % 1000000LL);
  tv->tv_sec = (long) ((now.ns100 - 116444736000000000LL) / 10000000LL);
  return (0);
}
#endif

int diff_ms(timeval t1, timeval t2)
{
  return (((t1.tv_sec - t2.tv_sec) * 1000000) + (t1.tv_usec - t2.tv_usec) + 500)/1000;
}

#include <cvblob.h>
using namespace cvb;

int main()
{
  const unsigned int size = 500;
  const unsigned int numPixels = 100000;
  const unsigned int max_iter = 100;

  srand(time(NULL));

  IplImage *randomImg = cvCreateImage(cvSize(size, size), 8, 3);
  IplImage *randomBinImg = cvCreateImage(cvSize(size, size), 8, 1);

  IplImage *labelImg = cvCreateImage(cvGetSize(randomBinImg), IPL_DEPTH_LABEL, 1);

  cvNamedWindow("test", CV_WINDOW_AUTOSIZE);

  unsigned int iter = 0;

  double averTime = 0.;
  do
  {
    cvSet(randomBinImg, cvScalar(0));

    for (unsigned int i=1; i<numPixels; i++)
      cvSet2D(randomBinImg, rand() % size, rand() % size, cvScalar(255));

    cvSaveImage("test_random.png", randomBinImg);

    CvBlobs blobs;

    timeval tim1;
    gettimeofday(&tim1, NULL);
    unsigned int result = cvLabel(randomBinImg, labelImg, blobs);
    timeval tim2;
    gettimeofday(&tim2, NULL);

    unsigned int elapseTime = diff_ms(tim2, tim1);

    cout << " - " << blobs.size() << " blobs and " << result << " pixels: " << elapseTime << " miliseconds elapsed." << endl;

    iter++;
    averTime = (1. - 1/((double)iter))*averTime + (1/((double)iter))*elapseTime;

    cvZero(randomImg);

    cvRenderBlobs(labelImg, blobs, randomImg, randomImg, CV_BLOB_RENDER_COLOR);
    cvShowImage("test", randomImg);

    cvReleaseBlobs(blobs);

    if (((cvWaitKey(10)&0xff)==27)||(iter>=max_iter))
      break;
  }
  while(true);

  cvDestroyWindow("test");

  cvReleaseImage(&randomBinImg);
  cvReleaseImage(&randomImg);
  cvReleaseImage(&labelImg);

  cout << endl;
  cout << iter << " iterations." << endl;
  cout << "Average cvLabel time: " << averTime << " miliseconds." << endl;

  return 0;
}
