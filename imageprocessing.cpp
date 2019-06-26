#include "imageprocessing.h"

cv::Vec3b ImageProcessing::SCALE[ImageProcessing::NUMBER_OF_COLORS] = { cv::Vec3b(0, 0, 0) };
std::vector<int> ImageProcessing::leftPixelCount = std::vector<int>();
std::vector<int> ImageProcessing::rightPixelCount = std::vector<int>();

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

    cv::Mat img;
    int rectHeight = ((521 - 15.0) / 13), width = 20;
    img.create(rectHeight * width, 20, CV_8UC3);

    for ( int i = 0; i < ImageProcessing::NUMBER_OF_COLORS; ++i ) {
        ImageProcessing::SCALE[i] = cv::Vec3b(scaleArray[i][0], scaleArray[i][1], scaleArray[i][2]);
        cv::rectangle( img, cv::Rect(0, i * rectHeight, width, rectHeight ),
                                cv::Scalar(scaleArray[i][0], scaleArray[i][1], scaleArray[i][2]), CV_FILLED );
        }

    cv::imwrite("scale.jpg", img);
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
    const int EROSION_SIZE = 2;
    const int EROSION_TYPE = cv::MORPH_CROSS;
    cv::Mat dest( source.size(), source.type() );
    cv::Mat element = cv::getStructuringElement( EROSION_TYPE,
                                                 cv::Size( 2 * EROSION_SIZE + 1, 2 * EROSION_SIZE + 1 ),
                                                 cv::Point( EROSION_SIZE, EROSION_SIZE ) );
    cv::erode( source, dest, element );
    return dest;
    }

cv::Mat ImageProcessing::rotate( const cv::Mat& source, int angle ) {
    cv::Point2f src_center(source.cols/2.0F, source.rows/2.0F);
    cv::Mat rot_mat = cv::getRotationMatrix2D(src_center, angle, 1.0);
    cv::Mat dst;
    cv::warpAffine(source, dst, rot_mat, source.size());
    return dst;
    }

// Colors
std::vector<cv::Mat> ImageProcessing::scaleColors( const cv::Mat& source ) {
    cv::Mat tempContainer;
    std::vector<cv::Mat> colorVectors;

    tempContainer = source.clone();
    for ( int colorIndex = 0; colorIndex < NUMBER_OF_COLORS; ++colorIndex ) {
        tempContainer = tempContainer.clone();
        for ( int y = 0; y < source.rows; ++y ) {
            for ( int x = 0; x < source.cols; ++x ) {
                if ( source.at<cv::Vec3b>( cv::Point(x, y) ) == SCALE[colorIndex] ) {
                    tempContainer.at<cv::Vec3b>( cv::Point(x, y) ) = cv::Vec3b(255, 255, 255);
                    }
                }
            }
        colorVectors.push_back( tempContainer );
        }

    return colorVectors;
}

cv::Mat ImageProcessing::polygonMask(const cv::Mat& source,
                                     const cv::Rect& rect,
                                     const int margin,
                                     std::vector<cv::Point>& points) {
    cv::Mat mask( source.size(), source.type(), cv::Scalar(0) );
    cv::Point pointsArray[1][4];
              pointsArray[0][0] = cv::Point(
                        rect.x + points[0].x - margin/2 + 1,
                        rect.y + points[0].y - margin/2 + 1
                    );
              pointsArray[0][1] = cv::Point(
                        rect.x + points[1].x - margin/2 + 1,
                        rect.y + points[1].y - margin/2 + 1
                    );
              pointsArray[0][2] = cv::Point(
                        rect.x + points[2].x - margin/2 + 1,
                        rect.y + points[2].y - margin/2 + 1
                    );
              pointsArray[0][3] = cv::Point(
                        rect.x + points[3].x - margin/2 + 1,
                        rect.y + points[3].y - margin/2 + 1
                    );
    const cv::Point* polygon[1] = { pointsArray[0] };
    const int npt[] = { 4 };
    cv::fillPoly(mask, polygon, npt, 1, cv::Scalar(255, 255, 255));

    return mask;
    }

void ImageProcessing::countPixels(const cv::Mat& source, std::vector<int>& counter) {
    int count;


    for ( int colorIndex = 0; colorIndex < NUMBER_OF_COLORS; ++colorIndex ) {
        count = 0;
        for ( int y = 0; y < source.rows; ++y ) {
            for ( int x = 0; x < source.cols; ++x ) {
                if ( source.at<cv::Vec3b>( cv::Point(x, y) ) == SCALE[colorIndex] ) {
                    count++;
                    }
                }
            }
        counter.push_back(count);
    }
}

int ImageProcessing::countReds(const cv::Mat& source, const cv::Mat& mask) {
    int count = 0;

    for (int colorIndex = NUMBER_OF_COLORS - 5; colorIndex < NUMBER_OF_COLORS; ++colorIndex) {
        // qDebug() << SCALE[colorIndex][0] << SCALE[colorIndex][1] << SCALE[colorIndex][2];
        for ( int y = 0; y < source.rows; ++y ) {
            for ( int x = 0; x < source.cols; ++x ) {
                // Si es parte de la máscara y está en la escala
                if ( mask.at<cv::Vec3b>( cv::Point(x, y) ) == cv::Vec3b(255, 255, 255) and
                     source.at<cv::Vec3b>( cv::Point(x, y) ) == SCALE[colorIndex] ) {
                    count++;
                    }
                }
            }
        }

    return count;
    }

cv::Mat ImageProcessing::hernandezCorvo(const cv::Mat& source, std::vector<std::vector<cv::Point>>& polyList, bool isLeft ) {
    cv::Mat marked;
    marked = source.clone();

    cv::Point pt1( (isLeft ? MARGIN + 1 : source.cols - MARGIN - 1), 0 ),
              pt1P( (isLeft ? source.cols : 0), 0 ),
              pt2( 0, MARGIN ),
              pt2P(0, source.rows - MARGIN),
              ptExt, interMF, ptExt4, ptExt5, ptInt4, ptInt5, ptInt9,
              interX1, interX2, interY1, interY2, interAY, interTA;
    Line *primeLine, *perPrimePt2, *perPrimePt2P, *l3, *l4, *l5, *l6, *l7, *l8, *l9;
    bool found;
    int distance;

    // Encontrar Punto 2 y Punto 2'
    // Punto 2
    /*
     * Búsqueda de arriba hacia abajo
     */
    found = false;
    for (int y = 0; y < source.rows; ++y) {
        for (int x = 0; x < source.cols; ++x) {
            if ( source.at<cv::Vec3b>( cv::Point(x, y) ) == cv::Vec3b(255, 255, 255) ) {
                pt2 = cv::Point(x, y);
                found = true;
                break;
                }
            }
        if (found) {
            break;
            }
        }

    // Punto 2'
    /*
     * Búsqueda de abajo hacia arriba
     */
    found = false;
    for (int y = source.rows - 1; y >= 0; --y) {
        for (int x = 0; x < source.cols; ++x) {
            if ( source.at<cv::Vec3b>( cv::Point(x, y) ) == cv::Vec3b(255, 255, 255) ) {
                pt2P = cv::Point(x, y);
                found = true;
                break;
                }
            }
        if (found) {
            break;
            }
        }

    // Encontrar Punto 1
    /*
     * La misma lógica es utilizada para encontrar el Punto 1, excepto que la variable sería el eje Y.
     */
    // p2.getY() + ( distY / 5 )
    int footLength = pt2P.y - pt2.y;
    distance = isLeft * source.cols;
    found = false;
    for (int y = pt2.y + ( footLength / 5 ); y < pt2.y + (footLength / 2); ++y) {
        for (int x = !isLeft * (source.cols - 1);
                 isLeft * (x < source.cols) + !isLeft * (x >= 0);
                 x += isLeft - !isLeft ) {
            if (source.at<cv::Vec3b>(cv::Point(x, y)) == cv::Vec3b(255, 255, 255)
                and ( isLeft * (x < distance) + !isLeft * (x > distance) ) ) {
                distance = x;
                pt1 = cv::Point(x, y);
                }
            }
        }

    // Encontrar Punto 1'
    /*
     *
     */
    for ( int y = pt2P.y; y >= pt2.y + 3 * footLength / 5; --y ) {
        for ( int x = !isLeft * (source.cols - 1);
              isLeft * (x < source.cols) + !isLeft * (x >= 0);
              x += isLeft - !isLeft ) {
            if ( source.at<cv::Vec3b>( cv::Point( x, y ) ) == cv::Vec3b( 255, 255, 255 ) ) {
                if ( isLeft * (x < pt1P.x) + !isLeft * ( x > pt1P.x ) ) {
                    pt1P = cv::Point(x, y);
                    }
                break;
                }
            }
        }

    // Línea principal
    primeLine = new Line(pt1.x, pt1.y, pt1P.x, pt1P.y);
    // Segunda línea que pasa por punto 2, perpendicular a la primer línea
    perPrimePt2 = new Line((*primeLine), pt2.x, pt2.y);
    // Tercer línea que pasa por punto 2', perpendicular a la primer línea
    perPrimePt2P = new Line((*primeLine), pt2P.x, pt2P.y);

    // Punto de intersección de la línea principal y pt2
    interMF.x = perPrimePt2->getIntersectPoint( (*primeLine) );
    interMF.y = perPrimePt2->getY( interMF.x );

    // Creación de divisiones de la medida fundamental, línea 3, 4 y 5
    cv::Point distPt1InterMF = cv::Point(pt1.x - interMF.x, pt1.y - interMF.y),
              // ---- Necesarios para el cálculo de colores -----
              pt3Int = cv::Point(interMF.x + distPt1InterMF.x, interMF.y + distPt1InterMF.y),
              pt4Int = cv::Point(interMF.x + 2 * distPt1InterMF.x, interMF.y + 2 * distPt1InterMF.y),
              pt5Int = cv::Point(interMF.x + 3 * distPt1InterMF.x, interMF.y + 3 * distPt1InterMF.y);
              // ------------------------------------------------

    l3 = new Line(*primeLine,
                    pt3Int.x,
                    pt3Int.y
                    );
    l4 = new Line(*primeLine,
                    pt4Int.x,
                    pt4Int.y
                    );
    l5 = new Line(*primeLine,
                    pt5Int.x,
                    pt5Int.y
                    );

    // Encontrar punto más externo entre línea 3 y línea 4.
    distance = source.cols - 1;
    for (int x = isLeft * (source.cols - 1);
         isLeft * (x >= 0) + !isLeft * (x < source.cols);
         x += !isLeft - isLeft) {
        for (int y = l3->getY(x); y < l4->getY(x); ++y) {
            if (source.at<cv::Vec3b>(cv::Point(x, y)) != cv::Vec3b(0, 0, 0) and
                !isLeft * (x < distance) + isLeft * ((source.cols - 1 - x) < distance)) {
                distance = isLeft * (source.cols - x) + !isLeft * x;
                ptExt = cv::Point(x, y);
                }
            }
        }


    // Creación de línea 6, perpendicular a la línea 3, paralela a la principal. Toca punto externo
    l6 = new Line(*l3, ptExt.x, ptExt.y);

    // Encontrar Punto Interno 4
    for (int i = !isLeft * (source.cols - 1);
        isLeft * (i < source.cols) + !isLeft * (i >= 0) ;
        i += isLeft - !isLeft) {
        int lineY = l4->getY(i);
        if ( source.at<cv::Vec3b>(lineY, i) != cv::Vec3b(0, 0, 0) ) {
            ptInt4.x = i;
            ptInt4.y = l4->getY(i);
            break;
            }
        }

    // Encontrar Punto Externo 4
    for (int i = isLeft * (source.cols - 1);
        isLeft * (i >= 0) + !isLeft * (i < source.cols);
        i += - isLeft + !isLeft) {
        if ( source.at<cv::Vec3b>(cv::Point(i, l4->getY(i))) != cv::Vec3b(0, 0, 0) ) {
            ptExt4.x = i;
            ptExt4.y = l4->getY(i);
            break;
            }
        }

    // Encontrar Punto Interno 5
    for (int i = !isLeft * (source.cols - 1);
        isLeft * (i < source.cols) + !isLeft * (i >= 0) ;
        i += isLeft - !isLeft) {
        if ( source.at<cv::Vec3b>(l5->getY(i), i) != cv::Vec3b(0, 0, 0) ) {
            ptInt5.x = i;
            ptInt5.y = l5->getY(i);
            break;
            }
        }

    // Encontrar Punto Externo 5
    for (int i = isLeft * (source.cols - 1);
        isLeft * (i >= 0) + !isLeft * (i < source.cols);
        i += - isLeft + !isLeft) {
        if ( source.at<cv::Vec3b>(cv::Point(i, l5->getY(i))) != cv::Vec3b(0, 0, 0) ) {
            ptExt5.x = i;
            ptExt5.y = l5->getY(i);
            break;
            }
        }


    // Obtener línea 7
    l7 = new Line(primeLine->getSlope(), ptExt4.x, ptExt4.y);
    // Obtener línea 8
    l8 = new Line(primeLine->getSlope(), ptExt5.x, ptExt5.y);

    // Encontrar punto más externo de la zona interna entre la línea 4 y 5.
    // Se llamará ptInt9. Dicho punto ayudará a trazar la línea 9.
    distance = 0;
    for (int y = ptInt4.y; y < ptInt5.y; ++y) {
        for (int x = !isLeft * (source.cols - 1);
             isLeft * (x < source.cols) +
             !isLeft * (x >= 0);
             x += isLeft - !isLeft) {
            if (source.at<cv::Vec3b>(cv::Point(x, y)) == cv::Vec3b(255, 255, 255) ) {
                if ( isLeft * (x > distance) + !isLeft * ((source.cols - x) > distance) ) {
                    distance = isLeft * x + !isLeft * (source.cols - x);
                    ptInt9 = cv::Point(x, y);
                    }
                break;
                }
            }
        }


    l9 = new Line(primeLine->getSlope(), ptInt9.x, ptInt9.y);

    // Obtener X
    // Intersección de una línea perpendicular a las dos líneas que se quiere medir la distancia entre ellas.
    // Se eligió la línea 4 como la que intersecta a la principal y  a la línea 6.
    interX1.x = l4->getIntersectPoint(*primeLine );
    interX1.y = l4->getY( interX1.x );
    interX2.x = l4->getIntersectPoint(*l6 );
    interX2.y = l4->getY( interX2.x );

    // Obtener Y
    interY1.x = l4->getIntersectPoint(*l7 );
    interY1.y = l4->getY( interY1.x );
    interY2.x = l4->getIntersectPoint(*l9 );
    interY2.y = l4->getY( interY2.x );
    interAY.x = l4->getIntersectPoint(*primeLine );
    interAY.y = l4->getY( interAY.x );

    // Obtener TA
    interTA.x = l5->getIntersectPoint( *primeLine );
    interTA.y = l5->getY( interTA.x );

    int medidaX = sqrt(pow(interX2.x - interX1.x, 2) + pow(interX2.y - interX1.y, 2));
    int medidaY = sqrt(pow(interY2.x - interY1.x, 2) + pow(interY2.y - interY1.y, 2));
    int medidaAY = sqrt(pow(interAY.x - interY1.x, 2) + pow(interAY.y - interY1.y, 2));
    int medidaTA = sqrt(pow(interTA.x - ptExt5.x, 2) + pow(interTA.x - ptExt5.y, 2));
    double percent = ((medidaX - medidaY) / (double) medidaX) * 100.0;

    // Necesario para la escala de colores
    cv::Point extMF;
    extMF.x = perPrimePt2->getIntersectPoint(*l6);
    extMF.y = perPrimePt2->getY(extMF.x);
    cv::Point pt3Ext = cv::Point(extMF.x + distPt1InterMF.x, extMF.y + distPt1InterMF.y),
              pt4Ext = cv::Point(extMF.x + 2 * distPt1InterMF.x, extMF.y + 2 * distPt1InterMF.y),
              pt5Ext = cv::Point(extMF.x + 3 * distPt1InterMF.x, extMF.y + 3 * distPt1InterMF.y);

    cv::Point intLow, extLow;
    intLow.x = perPrimePt2P->getIntersectPoint(*primeLine);
    intLow.y = perPrimePt2P->getY(intLow.x);
    extLow.x = perPrimePt2P->getIntersectPoint(*l6);
    extLow.y = perPrimePt2P->getY(extLow.x);


    // Impresión
    qDebug() << (isLeft ? "Pie izquierdo" : "Pie derecho") << ":";
    qDebug() << "X, anchura del metatarso: " << medidaX;
    qDebug() << "Y: " << medidaY;
    qDebug() << "AY, complemento de Y: " << medidaAY;
    qDebug() << "TA, anchura del talón: " << medidaTA;
    qDebug() << "Porcentaje: " << percent << "\%\n" ;

    std::string fileName = (isLeft ? "lData.txt" : "rData.txt");
    std::ofstream file(fileName, std::ios::out | std::ios::trunc );
    file << percent << std::endl;
    file.close();

    // Algoritmo usual
    cv::circle( marked, pt1, 1, cv::Scalar(0, 0, 255), 2);
    cv::circle( marked, pt1P, 1, cv::Scalar(0, 0, 255), 2);
    cv::circle( marked, pt2, 1, cv::Scalar(0, 0, 255), 2);
    cv::circle( marked, pt2P, 1, cv::Scalar(0, 0, 255), 2);
    cv::circle( marked, ptExt, 1, cv::Scalar(0, 0, 255), 2);
    cv::circle( marked, interMF, 1, cv::Scalar(0, 0, 255), 2);
    cv::circle( marked, ptExt4, 1, cv::Scalar(0, 0, 255), 2);
    cv::circle( marked, ptExt5, 1, cv::Scalar(0, 0, 255), 2);
    cv::circle( marked, ptInt4, 1, cv::Scalar(0, 0, 255), 2);
    cv::circle( marked, ptInt5, 1, cv::Scalar(0, 0, 255), 2);
    cv::circle( marked, ptInt9, 1, cv::Scalar(0, 255, 0), 2);

    // Escala de colores
    cv::circle( marked, pt3Int, 1, cv::Scalar(0, 255, 0), 2);
    cv::circle( marked, pt4Int, 1, cv::Scalar(0, 255, 0), 2);
    cv::circle( marked, pt5Int, 1, cv::Scalar(0, 255, 0), 2);
    cv::circle( marked, intLow, 1, cv::Scalar(0, 255, 0), 2);
    cv::circle( marked, extMF, 1, cv::Scalar(0, 255, 0), 2);
    cv::circle( marked, pt3Ext, 1, cv::Scalar(0, 255, 0), 2);
    cv::circle( marked, pt4Ext, 1, cv::Scalar(0, 255, 0), 2);
    cv::circle( marked, pt5Ext, 1, cv::Scalar(0, 255, 0), 2);
    cv::circle( marked, extLow, 1, cv::Scalar(0, 255, 0), 2);

    // Primer sección
    polyList.push_back(std::vector<cv::Point>());
    polyList[0].push_back(interMF);
    polyList[0].push_back(extMF);
    polyList[0].push_back(pt3Ext);
    polyList[0].push_back(pt3Int);

    // Segunda sección
    polyList.push_back(std::vector<cv::Point>());
    polyList[1].push_back(pt3Int);
    polyList[1].push_back(pt3Ext);
    polyList[1].push_back(pt4Ext);
    polyList[1].push_back(pt4Int);

    // Tercer sección
    polyList.push_back(std::vector<cv::Point>());
    polyList[2].push_back(pt4Int);
    polyList[2].push_back(pt4Ext);
    polyList[2].push_back(pt5Ext);
    polyList[2].push_back(pt5Int);

    // Cuarta sección
    polyList.push_back(std::vector<cv::Point>());
    polyList[3].push_back(pt5Int);
    polyList[3].push_back(pt5Ext);
    polyList[3].push_back(extLow);
    polyList[3].push_back(intLow);

    // Puntos medios
    /*cv::circle(marked, cv::Point((pt1.x + ptExt.x)/2, (pt1.y + ptExt.y)/2), 3, cv::Scalar(255, 0, 255));
    cv::circle(marked, cv::Point((ptExt4.x + ptInt4.x)/2, (ptExt4.y + ptInt4.y)/2), 3, cv::Scalar(255, 0, 255));
    cv::circle(marked, cv::Point((ptExt5.x + ptInt5.x)/2, (ptExt5.y + ptInt5.y)/2), 3, cv::Scalar(255, 0, 255));*/

    /*cv::line( marked,
          cv::Point(cv::Point((pt1.x + ptExt.x)/2, (pt1.y + ptExt.y)/2)),
          cv::Point((ptExt5.x + ptInt5.x)/2, (ptExt5.y + ptInt5.y)/2),
          cv::Scalar(0,0,255)
        );*/

    primeLine->draw( marked, cv::Scalar( 200, 200, 100 ) );
    primeLine->draw( marked, cv::Scalar( 0, 0, 200 ) );
    perPrimePt2->draw( marked, cv::Scalar( 200, 200, 100 ) );
    perPrimePt2P->draw( marked, cv::Scalar( 200, 200, 100 ) );
    l3->draw( marked, cv::Scalar( 200, 200, 100 ) );
    l4->draw( marked, cv::Scalar( 200, 200, 100 ) );
    l5->draw( marked, cv::Scalar( 200, 200, 100 ) );
    l6->draw( marked, cv::Scalar( 200, 200, 100 ) );
//    l7->draw( marked, cv::Scalar( 200, 200, 100 ) );
//    l8->draw( marked, cv::Scalar( 200, 200, 100 ) );
//    l9->draw( marked, cv::Scalar( 200, 200, 100 ) );

//    cv::line( marked, interX2, interX1, cv::Scalar(0, 255, 0));
//    cv::circle( marked, interY2, 3, cv::Scalar(0, 255, 255), 2);
//    cv::circle( marked, interY1, 3, cv::Scalar(0, 255, 255), 2);
//    cv::line( marked, interY2, interY1, cv::Scalar(0, 255, 0));

    return marked;
}

// Feet detection
bool ImageProcessing::separateFeet( const cv::Mat& source, cv::Mat& rightFoot, cv::Mat& leftFoot, cv::Rect& right, cv::Rect& left ) {
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
    for ( y = MARGIN; y < source.rows; ++y ) {
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
    for ( y = MARGIN; y < source.rows; ++y ) {
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

    cv::Point rightRectPoint = cv::Point( rlBorder.x - MARGIN, ruBorder.y - MARGIN );
    int rightRectWidth = rrBorder.x - rlBorder.x + 2 * MARGIN;
    int rightRectHeight = rbBorder.y - ruBorder.y + 2 * MARGIN;
    cv::Point leftRectPoint = cv::Point( llBorder.x - MARGIN, luBorder.y - MARGIN );
    int leftRectWidth = lrBorder.x - llBorder.x + 2 * MARGIN;
    int leftRectHeight = lbBorder.y - luBorder.y + 2 * MARGIN;

    if ( rightRectPoint.x + rightRectWidth >= source.cols or
         rightRectPoint.y + rightRectHeight >= source.rows or
         leftRectPoint.x + leftRectWidth >= source.cols or
         leftRectPoint.y + leftRectHeight >= source.rows) {
        return false;
        }

    rightFoot = source( right = cv::Rect( rlBorder.x - MARGIN, ruBorder.y - MARGIN,
                                  rrBorder.x - rlBorder.x + 2 * MARGIN,
                                  rbBorder.y - ruBorder.y + 2 * MARGIN ) ).clone();
    leftFoot = source( left = cv::Rect( llBorder.x - MARGIN, luBorder.y - MARGIN,
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
