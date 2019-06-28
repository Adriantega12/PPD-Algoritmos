#include <iostream>
#include <fstream>

#include <QDir>

#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>

#include "imageprocessing.h"
#include "foot.h"

int main(int argc, char *argv[]) {
    Foot right(argv[1], false), left(argv[1]);
    const int COLOR_INDEX = 4;
    const int MARGIN = 100;
    const char* fileName;
    std::vector<std::string> args;
    std::vector<cv::Mat> scaleColorMats;
    std::vector<std::vector<cv::Point>> polyPointsLeft, polyPointsRight;
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

        lHCMats = ImageProcessing::hernandezCorvo( bigLeft, true, polyPointsLeft, left );
        rHCMats = ImageProcessing::hernandezCorvo( bigRight, false, polyPointsRight, right );

        cv::imwrite( "rHC.jpg", rHCMats );
        cv::imwrite( "lHC.jpg", lHCMats );
        }

    // Enmascarado para contar pixeles en cada área del pie relevante
    int totalLeft = 0;
    std::vector<int> pixelsLeft;
    for (int i = 0; i < polyPointsLeft.size(); ++i) {
        int pixels;
        cv::Mat leftMask = ImageProcessing::polygonMask(source, leftRect, MARGIN, polyPointsLeft[i]);
        pixels = ImageProcessing::countReds(source, leftMask);
        pixelsLeft.push_back(pixels);
        totalLeft += pixels;
        cv::imwrite(std::to_string(i) + "mask_left.bmp", leftMask);
        }

    for (int i = 0; i < pixelsLeft.size(); ++i) {
        double percent = (double) pixelsLeft[i] / totalLeft * 100.0;
        left.setZonePressure(i, percent);
        }

    int totalRight = 0;
    std::vector<int> pixelsRight;
    for (int i = 0; i < polyPointsRight.size(); ++i) {
        int pixels;
        cv::Mat rightMask = ImageProcessing::polygonMask(source, rightRect, MARGIN, polyPointsRight[i]);
        pixels = ImageProcessing::countReds(source, rightMask);
        pixelsRight.push_back(pixels);
        totalRight += pixels;
        cv::imwrite(std::to_string(i) + "mask_right.bmp", rightMask);
        }

    for (int i = 0; i < pixelsRight.size(); ++i) {
        double percent = (double) pixelsRight[i] / totalRight * 100.0;
        right.setZonePressure(i, percent);
        }

    std::cout << "{"
              << "\"original\": \"" << fileName << "\", "
              << left.toJSON() << ","
              << right.toJSON()
              << "}";

    return 0;
    }
