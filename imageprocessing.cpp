#include "imageprocessing.h"

cv::Vec3b ImageProcessing::SCALE[ImageProcessing::NUMBER_OF_COLORS] = { cv::Vec3b(0, 0, 0) };

ImageProcessing::ImageProcessing() { }

void ImageProcessing::initializeImageProcessing() {
    int scaleArray[][3] = {
            {158, 153, 164},
            {175, 122, 169},
            {219, 79, 191},
            {216, 35, 148},
            {228, 33, 22},
            {205, 193, 50},
            {28, 227, 78},
            {48, 230, 225},
            {46, 193, 231},
            {45, 140, 247},
            {22, 107, 251},
            {0, 10, 250},
            {0, 0, 204},
            {0, 0, 255}
            };

    for ( int i = 0; i < ImageProcessing::NUMBER_OF_COLORS; ++i ) {
        ImageProcessing::SCALE[i] = cv::Vec3b(scaleArray[i][0], scaleArray[i][1], scaleArray[i][2]);
        }
    }

// Filters
cv::Mat ImageProcessing::cannyEdgeDetector( const cv::Mat& source ) {
    cv::Mat borders, edges, grayScale;

    borders.create( source.size(), source.type() );

    // Conversión a escala de grises
    cv::cvtColor( source, grayScale, cv::COLOR_BGR2GRAY );

    // Difuminado
    cv::blur( grayScale, edges, cv::Size(3, 3) );

    // Filtrado
    cv::Canny( edges, edges, 100, 100 * 3, 3 );

    // Copiado final
    borders = cv::Scalar::all( 0 );
    source.copyTo( borders, edges );

    return borders;
    }

cv::Mat ImageProcessing::binarize(const cv::Mat& source ) {
    cv::Mat binary( source.size(), source.type(), cv::Scalar(0) );

    for ( int y = 0; y < source.rows; ++y ) {
        for ( int x = 0; x < source.cols; ++x ) {
            if ( source.at<cv::Vec3b>( cv::Point(x, y) ) == cv::Vec3b(255, 255, 255) ) {
                binary.at<cv::Vec3b>( cv::Point(x, y) ) = cv::Vec3b(0, 0, 0);
                }
            else {
                binary.at<cv::Vec3b>( cv::Point(x, y) ) = cv::Vec3b(255, 255, 255);
                }
            }
        }

    return binary;
    }

cv::Mat ImageProcessing::erode( const cv::Mat& source ) {
    const int EROSION_SIZE = 1;
    const int EROSION_TYPE = cv::MORPH_CROSS;
    cv::Mat dest( source.size(), source.type() );
    cv::Mat element = cv::getStructuringElement( EROSION_TYPE,
                                                 cv::Size( 2 * EROSION_SIZE + 1, 2 * EROSION_SIZE + 1 ),
                                                 cv::Point( EROSION_SIZE, EROSION_SIZE ) );
    cv::erode( source, dest, element );
    return dest;
    }

cv::Mat ImageProcessing::hernandezCorvoLeft( const cv::Mat& source ) {
    cv::Mat marked;
    marked = source.clone();

    cv::Point pt1, pt1P, pt2, pt2P, ptExt, interMF, ptExt4, ptExt5, ptInt4, ptInt5, ptInt9,
                interX1, interX2, interY1, interY2, interAY, interTA;

    Line *primeLine, *perPrimePt2, *perPrimePt2P, *l3, *l4, *l5, *l6, *l7, *l8, *l9;

    return marked;
    }

// Feet detection
bool ImageProcessing::separateFeet( const cv::Mat& source, cv::Mat& rightFoot, cv::Mat& leftFoot ) {
    /*
     * La notación se torna un tanto complicada aquí porque hay 8 puntos que localizar, 4 de cada pie al menos.
     * Entonces, las variables se nombraran de acuerdo a la siguiente notación:
     * <PIE><LADO>Point
     * Donde:
     *  - <PIE>:
     *      - L = Izquierdo
     *      - R = Derecho
     *  - <LADO>:
     *      - U = Superior
     *      - B = Inferior
     *      - L = Izquierdo (viendo la planta de frente, como se muestra en la imagen)
     *      - R = Derecho (viendo la planta de frente, como se muestra en la imagen)
     */

    // Puntos correspondientes al pie derecho
    cv::Point rlBorder, rrBorder, ruBorder, rbBorder;
    // Puntos correspondientes al pie izquierdo
    cv::Point llBorder, lrBorder, luBorder, lbBorder;
    int x, y;
    bool isntFootColumn, isntFootRow;

    // Right foot, left border
    isntFootColumn = true;
    for ( x = 0; x < source.cols; ++x ) {
        for ( y = 0; y < source.rows; ++y ) {
            if ( source.at<cv::Vec3b>( cv::Point(x, y) ) == cv::Vec3b( 255, 255, 255 ) ) {
                rlBorder = cv::Point(x, y);
                isntFootColumn = false;
                break;
                }
            }

        if ( !isntFootColumn ) {
            break;
            }
        }

    // Right foot, right border
    for ( x = rlBorder.x + 1; x < source.cols; ++x ) {
        isntFootColumn = true;
        for ( y = 0; y < source.rows; ++y ) {
            if ( source.at<cv::Vec3b>( cv::Point(x, y) ) == cv::Vec3b( 255, 255, 255 ) ) {
                isntFootColumn = false;
                break;
                }
            }
        // Si no encontró ningún pedazo de pie, entonces el último punto en X es el borde izquierdo
        if ( isntFootColumn ) {
            rrBorder = cv::Point( x - 1, y );
            break;
            }
        }

    // Left foot, right border
    isntFootColumn = true;
    for ( x = source.cols; x > 0; --x ) {
        for ( y = 0; y < source.rows; ++y ) {
            if ( source.at<cv::Vec3b>( cv::Point(x, y) ) == cv::Vec3b( 255, 255, 255 ) ) {
                isntFootColumn = false;
                break;
                }
            }
        // Si no encontró ningún pedazo de pie, entonces el último punto en X es el borde izquierdo
        if ( !isntFootColumn ) {
            lrBorder = cv::Point( x, y );
            break;
            }
        }

    // Left foot, left border
    for ( x = lrBorder.x - 1; x > 0; --x ) {
        isntFootColumn = true;
        for ( y = 0; y < source.rows; ++y ) {
            if ( source.at<cv::Vec3b>( cv::Point(x, y) ) == cv::Vec3b( 255, 255, 255 ) ) {
                isntFootColumn = false;
                break;
                }
            }

        if ( isntFootColumn ) {
            llBorder = cv::Point(x, y);
            break;
            }
        }

    // Right foot upper border
    isntFootRow = true;
    for ( y = 0; y < source.rows; ++y ) {
        for ( x = rlBorder.x; x < rrBorder.x; ++x ) {
            if ( source.at<cv::Vec3b>( cv::Point(x, y) ) == cv::Vec3b( 255, 255, 255 ) ) {
                ruBorder = cv::Point( x, y );
                isntFootRow = false;
                break;
                }
            }

        if ( !isntFootRow ) {
            break;
            }
        }

    // Right foot bottom border
    isntFootRow = true;
    for ( y = source.rows- 1; y > 0; --y ) {
        for ( x = rlBorder.x; x < rrBorder.x; ++x ) {
            if ( source.at<cv::Vec3b>( cv::Point(x, y) ) == cv::Vec3b( 255, 255, 255 ) ) {
                rbBorder = cv::Point(x, y);
                isntFootRow = false;
                break;
                }
            }

        if ( !isntFootRow ) {
            break;
            }
        }

    // Left foot upper border
    isntFootRow = true;
    for ( y = 0; y < source.rows; ++y ) {
        for ( x = llBorder.x; x < lrBorder.x; ++x ) {
            if ( source.at<cv::Vec3b>( cv::Point(x, y) ) == cv::Vec3b( 255, 255, 255 ) ) {
                luBorder = cv::Point( x, y );
                isntFootRow = false;
                break;
                }
            }

        if ( !isntFootRow ) {
            break;
            }
        }

    // Left foot bottom border
    isntFootRow = true;
    for ( y = source.rows- 1; y > 0; --y ) {
        for ( x = llBorder.x; x < lrBorder.x; ++x ) {
            if ( source.at<cv::Vec3b>( cv::Point(x, y) ) == cv::Vec3b( 255, 255, 255 ) ) {
                lbBorder = cv::Point(x, y);
                isntFootRow = false;
                break;
                }
            }

        if ( !isntFootRow ) {
            break;
            }
        }

    // No está bien la división
    if ( rrBorder.x > llBorder.x ) {
        return false;
        }

    rightFoot = source( cv::Rect( rlBorder.x - MARGIN, ruBorder.y - MARGIN,
                                  rrBorder.x - rlBorder.x + 2 * MARGIN,
                                  rbBorder.y - ruBorder.y + 2 * MARGIN ) ).clone();
    leftFoot = source( cv::Rect( llBorder.x - MARGIN, luBorder.y - MARGIN,
                                             lrBorder.x - llBorder.x + 2 * MARGIN,
                                             lbBorder.y - luBorder.y + 2 * MARGIN ) ).clone();

    cv::line( source, cv::Point( rlBorder.x, ruBorder.y ), cv::Point( rrBorder.x, ruBorder.y ), cv::Scalar(0, 0, 255) );
    cv::line( source, cv::Point( rlBorder.x, ruBorder.y ), cv::Point( rlBorder.x, rbBorder.y ), cv::Scalar(0, 0, 255) );
    cv::line( source, cv::Point( rlBorder.x, rbBorder.y ), cv::Point( rrBorder.x, rbBorder.y ), cv::Scalar(0, 0, 255) );
    cv::line( source, cv::Point( rrBorder.x, ruBorder.y ), cv::Point( rrBorder.x, rbBorder.y ), cv::Scalar(0, 0, 255) );

    cv::line( source, cv::Point( llBorder.x, luBorder.y ), cv::Point( lrBorder.x, luBorder.y ), cv::Scalar(0, 0, 255) );
    cv::line( source, cv::Point( llBorder.x, luBorder.y ), cv::Point( llBorder.x, lbBorder.y ), cv::Scalar(0, 0, 255) );
    cv::line( source, cv::Point( llBorder.x, lbBorder.y ), cv::Point( lrBorder.x, lbBorder.y ), cv::Scalar(0, 0, 255) );
    cv::line( source, cv::Point( lrBorder.x, luBorder.y ), cv::Point( lrBorder.x, lbBorder.y ), cv::Scalar(0, 0, 255) );

    return true;
    }
