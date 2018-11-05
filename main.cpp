#include <iostream>
#include <fstream>

#include <QDir>

#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>

#include "imageprocessing.h"

int main(int argc, char *argv[]) {
    const int COLOR_INDEX = 5;
    std::vector<cv::Mat> scaleColorMats;
    cv::Mat source, binary, erode, separateFeet, rightFoot, leftFoot, markedLeft, markedRight,
            binMats, erodedMats, rMats, lMats,rHCMats, lHCMats, rightImg, leftImg;

    // Inicializar IP
    ImageProcessing::initializeImageProcessing();

    // Leer parámetros de entrada
    cv::CommandLineParser parser( argc, argv,
                                  "{ @input | Input feet image }"
                                  //"{ -s     | Output separate feet }"
                                  //"{ -hc    | Output Hernandez-Corvo algorithm }"
                                  //"{ -pc    | Output pixel count }"
                                  );

    // Cargar imagen
    source = cv::imread( parser.get<cv::String>("@input"), cv::IMREAD_COLOR );

    // Binarizar imagen
    binary = ImageProcessing::binarize( source );

    // Erosionar imagen
    erode = ImageProcessing::erode( binary );
    erode.copyTo( separateFeet );
    cv::imwrite( "eroded.jpg", separateFeet);

    // Generar las distintas imágenes de toda la escala de colores
    scaleColorMats = ImageProcessing::scaleColors( source );

    // Separar pies
    cv::Rect rightRect, leftRect;
    ImageProcessing::separateFeet( separateFeet, rightFoot, leftFoot, rightRect, leftRect );
    rightImg = source( rightRect ).clone(),
    leftImg = source( leftRect ).clone();
    cv::imwrite("test.jpg", separateFeet);
    cv::imwrite( "right.jpg", rightImg );
    cv::imwrite( "left.jpg", leftImg );

    // Hernández-Corvo
    cv::Mat bigRight, bigLeft;
    binMats = ImageProcessing::binarize( scaleColorMats.at(COLOR_INDEX) );
    erodedMats = ImageProcessing::erode( binMats );
    ImageProcessing::separateFeet( erodedMats, rMats, lMats, rightRect, leftRect );
    bigRight = cv::Mat(rMats.rows + 10, rMats.cols + 10, CV_8UC3, cv::Scalar(0, 0, 0));
    bigLeft = cv::Mat(lMats.rows + 10, lMats.cols + 10, CV_8UC3, cv::Scalar(0, 0, 0));
    rMats.copyTo(bigRight(cv::Rect(4, 4, rMats.cols, rMats.rows)));
    lMats.copyTo(bigLeft(cv::Rect(4, 4, lMats.cols, lMats.rows)));
    lHCMats = ImageProcessing::hernandezCorvo( bigLeft, true );
    rHCMats = ImageProcessing::hernandezCorvo( bigRight, false );
    cv::imwrite(  "rHC.jpg", rHCMats );
    cv::imwrite(  "lHC.jpg", lHCMats );

    // Pixel Count
    rightImg = source( rightRect ).clone(),
    leftImg = source( leftRect ).clone();
    ImageProcessing::countPixels(rightImg, ImageProcessing::rightPixelCount);
    ImageProcessing::countPixels(leftImg, ImageProcessing::leftPixelCount);
    std::ofstream rightFile("rData.txt", std::ios::out | std::ios::app);
    std::ofstream leftFile("lData.txt", std::ios::out | std::ios::app);
    for (int i = 0; i < ImageProcessing::rightPixelCount.size(); ++i) {
        rightFile << ImageProcessing::rightPixelCount[i] << std::endl;
        leftFile << ImageProcessing::leftPixelCount[i] << std::endl;
        }
    rightFile.close();
    leftFile.close();


    return 0;
    }
