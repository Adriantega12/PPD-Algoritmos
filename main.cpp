#include <iostream>

#include <QDir>

#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>

#include "imageprocessing.h"

int main(int argc, char *argv[]) {
    cv::Mat source, binary, erode, separateFeet, rightFoot, leftFoot, markedLeft, markedRight;

    // Inicializar IP
    ImageProcessing::initializeImageProcessing();

    // Leer parámetros de entrada
    cv::CommandLineParser parser( argc, argv, "{ @input | input image}");

    // Cargar imagen
    source = cv::imread( parser.get<cv::String>("@input"), cv::IMREAD_COLOR );

    binary = ImageProcessing::binarize( source );
    erode = ImageProcessing::erode( binary );
    erode.copyTo( separateFeet );
    if (ImageProcessing::separateFeet( separateFeet, rightFoot, leftFoot )) {
        std::cout << "Se pudo separar los pies efectivamente" << std::endl;
        }
    markedLeft = ImageProcessing::hernandezCorvo( leftFoot, true );
    markedRight = ImageProcessing::hernandezCorvo( rightFoot, false );

    // Guardar imágenes
    cv::imwrite( "binary.jpg", binary );
    cv::imwrite( "erode.jpg", erode );
    cv::imwrite( "separateFeet.jpg", separateFeet );
    cv::imwrite( "right.jpg", rightFoot );
    cv::imwrite( "right_hc.jpg", markedRight );
    cv::imwrite( "left.jpg", leftFoot );
    cv::imwrite( "left_hc.jpg", markedLeft );

    return 0;
    }
