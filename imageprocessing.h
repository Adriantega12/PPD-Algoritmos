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
#include <cmath>

#include "line.h"

class ImageProcessing {
    public:
        static const int NUMBER_OF_COLORS = 14;     // Número de colores en la escala

    private:
        static cv::Vec3b SCALE[NUMBER_OF_COLORS];   // Vector de escala de colores
        static const int MARGIN = 5;              // Pixeles de sobra

    public:

        ImageProcessing();

        // Initialize
        static void initializeImageProcessing();

        // Filters
        static cv::Mat cannyEdgeDetector( const cv::Mat& );
        static cv::Mat binarize(const cv::Mat& );
        static cv::Mat erode( const cv::Mat& );

        // Colors
        static std::vector<cv::Mat> scaleColors( const cv::Mat& );

        // Processing
        static cv::Mat hernandezCorvo( const cv::Mat&, bool );

        // Feet detection
        static bool separateFeet(const cv::Mat&,
                                    cv::Mat&, cv::Mat&,
                                    cv::Rect&,
                                    cv::Rect& );
    };

#endif // IMAGEPROCESSING_H
