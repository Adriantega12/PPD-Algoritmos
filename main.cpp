#include <iostream>
#include <fstream>

#include <QDir>

#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>

#include "imageprocessing.h"

int main(int argc, char *argv[]) {
    const int COLOR_INDEX = 3;
    std::vector<cv::Mat> scaleColorMats;
    cv::Mat source, binary, erode, separateFeet, rightFoot, leftFoot, markedLeft, markedRight,
            binMats, erodedMats, rMats, lMats,rHCMats, lHCMats;

    // Inicializar IP
    ImageProcessing::initializeImageProcessing();

    // Leer parámetros de entrada
    cv::CommandLineParser parser( argc, argv,
                                  "{ @input | Input feet image }"
                                  "{ -s     | Output separate feet }"
                                  "{ -hc    | Output Hernandez-Corvo algorithm }"
                                  "{ -pc    | Output pixel count }"
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
    if ( parser.has( "-s" ) ) {
        cv::Mat rightImg = source( rightRect ).clone(),
                leftImg = source( leftRect ).clone();
        cv::imwrite( "right.jpg", rightImg );
        cv::imwrite( "left.jpg", leftImg );
        }

    // Hernández-Corvo
    if ( parser.has( "-hc" ) ) {
        //cv::Rect rightRect, leftRect;
        binMats = ImageProcessing::binarize( scaleColorMats.at(COLOR_INDEX) );
        erodedMats = ImageProcessing::erode( binMats );
        ImageProcessing::separateFeet( erodedMats, rMats, lMats, rightRect, leftRect );
        lHCMats = ImageProcessing::hernandezCorvo( lMats, true );
        rHCMats = ImageProcessing::hernandezCorvo( rMats, false );
        cv::imwrite(  "rHC.jpg", rHCMats );
        cv::imwrite(  "lHC.jpg", lHCMats );
        }

    if ( parser.has("-pc") ) {
        cv::Mat rightImg = source( rightRect ).clone(),
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
