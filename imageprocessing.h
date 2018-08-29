#ifndef IMAGEPROCESSING_H
#define IMAGEPROCESSING_H

// OpenCV
#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>

// STD
#include <iostream>
#include <vector>
#include <algorithm>
#include <functional>

class ImageProcessing {
    private:
        static const int NUMBER_OF_COLORS = 14;
        static cv::Vec3b SCALE[NUMBER_OF_COLORS];

    public:
        ImageProcessing();

        // Initialize
        static void initializeImageProcessing();

        // Filters
        static cv::Mat cannyEdgeDetector( const cv::Mat& );
        static cv::Mat binarize(const cv::Mat& );
        static cv::Mat erode( const cv::Mat& );

        // Colors


        // Feet detection
        static bool separateFeet( const cv::Mat&, cv::Mat&, cv::Mat& );
    };

#endif // IMAGEPROCESSING_H
