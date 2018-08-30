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
    int initialYIntercept, perpP2Intercept, perpPP2Intercept, p1Intercept, p4Intercept, p5Intercept,
        distXMFToP1, distYMFToP1;
    double initialSlope, perpSlope;
    cv::Mat marked;
    cv::Point p1( MARGIN + 1, 0 ),
              pP1( source.cols, 0 ),
              p2( 0, MARGIN ),
              pP2(0, source.rows - MARGIN),
              p4, p5,
              crossPInitialPerpP2;

    // Encontrar Punto 2 y Punto 2'
    /*
     * Lo que hace esta parte es aprovechar el margen que hay de "sobra" en la foto del pie para encontrar los puntos 2 y 2'.
     * Este margen nos permite conocer la posición en Y de antemano de ambos puntos, puesto que no hay variabilidad sobre esto, porque
     * los puntos anterior y posteriores ya se encontraron al hacer la separación.
     */
    for ( int x = 0; x < source.cols; ++x ) {
        if ( !p2.x and source.at<cv::Vec3b>( cv::Point( x, p2.y ) ) == cv::Vec3b( 255, 255, 255 ) ) {
            p2.x = x;
            }
        if ( !pP2.x and source.at<cv::Vec3b>( cv::Point( x, pP2.y ) ) == cv::Vec3b( 255, 255, 255 ) ) {
            pP2.x = x;
            }
        if ( p2.x and pP2.x ) {
            break;
            }
        }

    // Encontrar Punto 1
    /*
     * La misma lógica es utilizada para encontrar el Punto 1, excepto que la variable sería el eje Y.
     */
    for ( int y = 0; y < source.rows; ++y ) {
        if ( source.at<cv::Vec3b>( cv::Point( p1.x, y ) ) == cv::Vec3b( 255, 255, 255 ) ) {
            p1.y = y;
            break;
            }
        }

    // Encontrar Punto 1'
    /*
     *
     */
    int footLength = pP2.y - p2.y;
    for ( int y = footLength / 2; y < source.rows; ++y ) {
        for ( int x = 0; x < source.cols; ++x ) {
            if ( source.at<cv::Vec3b>( cv::Point( x, y ) ) == cv::Vec3b( 255, 255, 255 ) ) {
                if ( x < pP1.x ) {
                    pP1.x = x;
                    pP1.y = y;
                    }
                break;
                }
            }
        }
    marked = source.clone();

    // Calcular la pendiente entre Punto 1 y Punto 1'
    initialSlope = (double)((pP1.y - p1.y)/(pP1.x - p1.x));
    initialYIntercept = p1.y - initialSlope * p1.x;
    perpSlope = (double)( -1 / initialSlope );
    perpP2Intercept = p2.y - perpSlope * p2.x;
    perpPP2Intercept = pP2.y - perpSlope * pP2.x;
    crossPInitialPerpP2.x = (initialYIntercept - perpP2Intercept) /
                            (perpSlope - initialSlope);
    crossPInitialPerpP2.y = perpSlope * crossPInitialPerpP2.x + perpP2Intercept;
    /*p1Intercept = p1.y - perpSlope * p1.x;
    distXMFToP1 = p1.x - crossPInitialPerpP2.x;
    distYMFToP1 = p1.y - crossPInitialPerpP2.y;
    p4 = cv::Point( p1.x + distXMFToP1, p1.y + distYMFToP1 );
    p4Intercept = p4.y - perpSlope * p4.x;
    p5 = cv::Point( p1.x + 2 * distXMFToP1, p1.y + 2 * distYMFToP1 );
    p5Intercept = p5.y - perpSlope * p5.x;

    cv::circle( marked, p1, 2, cv::Scalar(0, 0, 255) );
    cv::circle( marked, pP1, 2, cv::Scalar(0, 0, 255) );
    cv::circle( marked, p2, 2, cv::Scalar(0, 0, 255) );
    cv::circle( marked, pP2, 2, cv::Scalar(0, 0, 255) );
    cv::circle( marked, crossPInitialPerpP2, 2, cv::Scalar(0, 0, 255) );

    cv::line( marked, cv::Point( (0 - initialYIntercept)/initialSlope, 0 ),
                      cv::Point( (source.rows - 1 - initialYIntercept)/initialSlope, source.rows - 1 ),
                      cv::Scalar( 200, 200, 100));
    cv::line( marked, cv::Point( 0, 0 * perpSlope + perpP2Intercept ),
                      cv::Point( source.cols - 1, source.cols * perpSlope + perpP2Intercept ),
                      cv::Scalar( 200, 200, 100));
    cv::line( marked, cv::Point( 0, 0 * perpSlope + perpPP2Intercept ),
                      cv::Point( source.cols - 1, source.cols * perpSlope + perpPP2Intercept ),
                      cv::Scalar( 200, 200, 100));
    cv::line( marked, cv::Point( 0, 0 * perpSlope + p1Intercept ),
                      cv::Point( source.cols - 1, source.cols * perpSlope + p1Intercept ),
                      cv::Scalar( 200, 200, 100));
    cv::line( marked, cv::Point( 0, 0 * perpSlope + p4Intercept ),
                      cv::Point( source.cols - 1, source.cols * perpSlope + p4Intercept ),
                      cv::Scalar( 200, 200, 100));
    cv::line( marked, cv::Point( 0, 0 * perpSlope + p5Intercept ),
                      cv::Point( source.cols - 1, source.cols * perpSlope + p5Intercept ),
                      cv::Scalar( 200, 200, 100));
    */
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
