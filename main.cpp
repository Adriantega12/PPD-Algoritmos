#include <iostream>

#include <QDir>

#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>

#include "imageprocessing.h"

int main(int argc, char *argv[]) {
    std::vector<cv::Mat> scaleColorMats;
    cv::Mat source, binary, erode, separateFeet, rightFoot, leftFoot, markedLeft, markedRight,
            binMats, erodedMats, rMats, lMats,rHCMats, lHCMats;

    // Inicializar IP
    ImageProcessing::initializeImageProcessing();
    // QDir::mkdir( "img" );

    // Leer parámetros de entrada
    cv::CommandLineParser parser( argc, argv, "{ @input | input image}");

    // Cargar imagen
    source = cv::imread( parser.get<cv::String>("@input"), cv::IMREAD_COLOR );

    // Procesarla
    scaleColorMats = ImageProcessing::scaleColors( source );
    binary = ImageProcessing::binarize( source );
    erode = ImageProcessing::erode( binary );
    erode.copyTo( separateFeet );
    ImageProcessing::separateFeet( separateFeet, rightFoot, leftFoot );
    markedLeft = ImageProcessing::hernandezCorvo( leftFoot, true );
    markedRight = ImageProcessing::hernandezCorvo( rightFoot, false );

    // Guardar imágenes
    for ( int i = 0; i < ImageProcessing::NUMBER_OF_COLORS - 2; ++i ) {
        binMats = ImageProcessing::binarize( scaleColorMats.at(i) );
        erodedMats = ImageProcessing::erode( binMats );
        ImageProcessing::separateFeet( erodedMats, rMats, lMats );
        lHCMats = ImageProcessing::hernandezCorvo( lMats, true );
        rHCMats = ImageProcessing::hernandezCorvo( rMats, false );

        QString name = QString::number( i ) + ".jpg";
        cv::imwrite( name.toStdString(), scaleColorMats.at(i) );
        QString bName = QString::number( i ) + "_binary.jpg";
        cv::imwrite( bName.toStdString(), binMats );
        QString eName = QString::number( i ) + "_eroded.jpg";
        cv::imwrite( eName.toStdString(), erodedMats );
        QString lName = QString::number( i ) + "_left.jpg";
        cv::imwrite( lName.toStdString(), lMats );
        QString rName = QString::number( i ) + "_right.jpg";
        cv::imwrite( rName.toStdString(), rMats );
        QString lHCName = QString::number( i ) + "_leftHC.jpg";
        cv::imwrite( lHCName.toStdString(), lHCMats );
        QString rHCName = QString::number( i ) + "_rightHC.jpg";
        cv::imwrite( rHCName.toStdString(), rHCMats );
        }
    cv::imwrite( "binary.jpg", binary );
    cv::imwrite( "erode.jpg", erode );
    cv::imwrite( "separateFeet.jpg", separateFeet );
    cv::imwrite( "right.jpg", rightFoot );
    cv::imwrite( "right_hc.jpg", markedRight );
    cv::imwrite( "left.jpg", leftFoot );
    cv::imwrite( "left_hc.jpg", markedLeft );

    return 0;
    }
