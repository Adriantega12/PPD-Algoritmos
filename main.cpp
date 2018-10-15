#include <iostream>

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
                                  );

    // Cargar imagen
    source = cv::imread( parser.get<cv::String>("@input"), cv::IMREAD_COLOR );

    // Binarizar imagen
    binary = ImageProcessing::binarize( source );

    // Erosionar imagen
    erode = ImageProcessing::erode( binary );
    erode.copyTo( separateFeet );

    // Generar las distintas imágenes de toda la escala de colores
    scaleColorMats = ImageProcessing::scaleColors( source );

    // Separar pies
    if ( parser.has( "-s" ) ) {
        cv::Rect rightRect, leftRect;
        ImageProcessing::separateFeet( separateFeet, rightFoot, leftFoot, rightRect, leftRect );
        cv::Mat rightImg = source( rightRect ).clone(),
                leftImg = source( leftRect ).clone();
        cv::imwrite(  "r.jpg", rightImg );
        cv::imwrite(  "l.jpg", leftImg );
        }

    // Hernández-Corvo
    if ( parser.has( "-hc" ) ) {
        cv::Rect rightRect, leftRect;
        binMats = ImageProcessing::binarize( scaleColorMats.at(COLOR_INDEX) );
        erodedMats = ImageProcessing::erode( binMats );
        ImageProcessing::separateFeet( erodedMats, rMats, lMats, rightRect, leftRect );
        lHCMats = ImageProcessing::hernandezCorvo( lMats, true );
        rHCMats = ImageProcessing::hernandezCorvo( rMats, false );
        cv::imwrite(  "rHC.jpg", rHCMats );
        cv::imwrite(  "lHC.jpg", lHCMats );
        }

    return 0;
    }
