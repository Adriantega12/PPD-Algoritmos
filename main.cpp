#include <iostream>
#include <fstream>

#include <QDir>

#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>

#include "imageprocessing.h"

int main(int argc, char *argv[]) {
    const int COLOR_INDEX = 4;
    const char* fileName;
    std::vector<std::string> args;
    std::vector<cv::Mat> scaleColorMats;
    cv::Mat source, binary, erode, separateFeet, rightFoot, leftFoot, markedLeft, markedRight,
            binMats, erodedMats, rMats, lMats,rHCMats, lHCMats, rightImg, leftImg;
    cv::Rect rightRect, leftRect;

    // Inicializar IP
    ImageProcessing::initializeImageProcessing();

    // Leer parámetros de entrada
    if (argc > 1) {
        fileName = argv[1];
        }
    if (argc > 2) {
        for (int i = 2; i < argc; ++i) {
            args.push_back(std::string(argv[i]));
            }
        }

    // Cargar imagen
    source = cv::imread(fileName, cv::IMREAD_COLOR );

    // Binarizar imagen
    binary = ImageProcessing::binarize( source );

    // Erosionar imagen
    erode = ImageProcessing::erode( binary );
    erode.copyTo( separateFeet );
    cv::imwrite( "eroded.jpg", separateFeet);

    // Generar las distintas imágenes de toda la escala de colores
    scaleColorMats = ImageProcessing::scaleColors( source );

    // Separar pies
    if ( std::find(args.begin(), args.end(), "-s") != args.end() ) {
        ImageProcessing::separateFeet( separateFeet, rightFoot, leftFoot, rightRect, leftRect );
        rightImg = source( rightRect ).clone(),
        leftImg = source( leftRect ).clone();
        cv::imwrite( "right.jpg", rightImg );
        cv::imwrite( "left.jpg", leftImg );
        }

    // Hernández-Corvo
    if ( std::find(args.begin(), args.end(), "-hc") != args.end() ) {
        const int MARGIN = 100;
        cv::Mat bigRight, bigLeft;

        binMats = ImageProcessing::binarize( scaleColorMats.at(COLOR_INDEX) );
        erodedMats = ImageProcessing::erode( binMats );

        ImageProcessing::separateFeet( erodedMats, rMats, lMats, rightRect, leftRect );

        bigRight = cv::Mat(rMats.rows + MARGIN, rMats.cols + MARGIN, CV_8UC3, cv::Scalar(0, 0, 0));
        bigLeft = cv::Mat(lMats.rows + MARGIN, lMats.cols + MARGIN, CV_8UC3, cv::Scalar(0, 0, 0));

        rMats.copyTo(bigRight(cv::Rect(MARGIN/2 - 1, MARGIN/2 - 1, rMats.cols, rMats.rows)));
        lMats.copyTo(bigLeft(cv::Rect(MARGIN/2 - 1, MARGIN/2 - 1, lMats.cols, lMats.rows)));

        std::vector<std::string>::iterator it;
        if ( (it = std::find(args.begin(), args.end(), "-r")) != args.end() ) {
            it = std::next(it, 1);
            int angle = std::stoi(*it);
            if (angle != 0) {
                bigRight = ImageProcessing::rotate(bigRight, -angle);
                bigLeft = ImageProcessing::rotate(bigLeft, -angle);
                }
            }

        lHCMats = ImageProcessing::hernandezCorvo( bigLeft, true );
        rHCMats = ImageProcessing::hernandezCorvo( bigRight, false );

        cv::imwrite(  "rHC.jpg", rHCMats );
        cv::imwrite(  "lHC.jpg", lHCMats );
        }

    // Pixel Count
    if ( std::find(args.begin(), args.end(), "-pc") != args.end() ) {
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
        }

    return 0;
    }
