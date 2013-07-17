// TestDLL.c : test of CvBlob DLL
//

#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/features2d/features2d.hpp>

#include <cvb_blob_list.h>

#include <Windows.h>

int main(int argc, char* argv[]) {
    cv::Mat img = cv::imread("test.png", 1);
    cv::Mat grey;
    grey.create(img.size(), CV_8UC1);
    cv::cvtColor(img, grey, CV_BGR2GRAY, 1);
    cv::threshold(grey, grey, 0, 255, CV_THRESH_BINARY_INV + CV_THRESH_OTSU);

    LARGE_INTEGER freq;
    LARGE_INTEGER t0, tF, tDiff;
    double elapsedTime;

    // cvblob
    QueryPerformanceFrequency(&freq);
    QueryPerformanceCounter(&t0);

    cvb::BlobList blobs;
    blobs.SimpleLabel(grey);
    blobs.FilterByArea(50, img.cols * img.rows / 4);

    QueryPerformanceCounter(&tF);
    tDiff.QuadPart = tF.QuadPart - t0.QuadPart;
    elapsedTime = tDiff.QuadPart / (double) freq.QuadPart;
    std::cout << "cvblob:" << elapsedTime << std::endl;

    for (auto &a_blob : blobs.get_BlobsList()) {
        // extract and draws blob contour
        auto contours = a_blob->get_Contour().get_ContourPolygon();
        if (contours.size() != 1) {
            for (auto &iter = contours.begin(); iter != contours.end(); iter++) {
                auto next_iter = iter;
                next_iter++;
                if (next_iter == contours.end())
                    cv::line(img, *iter, contours[0], cv::Scalar(0, 0, 255), 1, 8);
                else
                    cv::line(img, *iter, *next_iter, cv::Scalar(0, 0, 255), 1, 8);
            }
        }

        // extract and draws every blob internal hole contour
        auto internal_contours = a_blob->get_InternalContours();
        for (auto &a_contour : internal_contours) {
            auto contour_dot = a_contour->get_ContourPolygon();
            if (contour_dot.size() == 1)
                continue;
            for (auto &iter = contour_dot.begin(); iter != contour_dot.end(); iter++) {
                auto next_iter = iter;
                next_iter++;
                if (next_iter == contour_dot.end())
                    cv::line(img, *iter, contour_dot[0], cv::Scalar(0, 255, 0), 1, 8);
                else
                    cv::line(img, *iter, *next_iter, cv::Scalar(0, 255, 0), 1, 8);
            }
        }

        // draws bounding box
        cv::rectangle(img, a_blob->get_BoundingBox(), cv::Scalar(255, 0, 255));

        // draws centroid
        cv::circle(img, a_blob->get_Centroid(), 2, cv::Scalar(255, 0, 0), 2);
    };

    cv::imwrite("imggray.png", grey);
    cv::imwrite("imgout.png", img);
    cv::imwrite("imglabel.png", blobs.get_ImageLabel());
    return 0;
}


