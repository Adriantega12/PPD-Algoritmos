#include "line.h"

// Recta dada una pendiente y una ordenada al origen
Line::Line(double _m, int _b, std::string _l) :
			m(_m), b(_b), label(_l) { }

// Recta punto-pendiente
Line::Line(double m, int x, int y) :
			Line(m, b = Line::calculateYCross(x, y, m)) { }

// Recta con dos puntos
Line::Line(int x1, int y1, int x2, int y2) :
		   	Line( m = Line::calculateSlope(x1, y1, x2, y2), x1, y1) { }

// Recta paralela con ordenada al origen relativa
Line::Line(Line& l, int offset) :
			Line(l.m, l.b + offset) { }

// Recta perpendicular que pasa por un punto definido de otra recta
Line::Line(Line& l, int x, int y) :
			Line(Line::calculatePerpendicularSlope(l.m), x, y) { }

// Getters
double Line::getSlope() {
	return m;
	}

int Line::getYCross() {
	return b;
	}

// Setters
void Line::setSlope(double _m) {
	m = _m;
	}

void Line::setYCross(int _b) {
	b = _b;
	}

// Funciones útiles dada una recta definida
int Line::getY(int x) {
	// y = mx + b
	return m * x + b;
	}

int Line::getX(int y) {
	// x = (y - b) / m
	return (y - b) / m;
	}

void Line::setLabel(std::string str) {
	label = str;
	}

std::string Line::getLabel() {
	return label;
	}

// Obtener punto en X de intersección con otra línea
int Line::getIntersectPoint(Line& l) {
	// y1 = m1x + b1
	// y2 = m2x + b2
	// m1x + b1 = m2x + b2
	// m1x - m2x = b2 - b1
	// x(m1 - m2) = b2 - b1
	// x = (b2 - b1)/(m1 - m2)

	return (l.b - b)/(m - l.m);
	}

void Line::draw( cv::Mat& img, cv::Scalar color ) {
	cv::line( img,
		  cv::Point( getX( getYCross() ), getYCross() ),
		  cv::Point( img.cols, getY( img.cols ) ),
		  color
		);
    /*
	cv::putText( img, label, cv::Point( img.rows, getY( img.rows ) ), 
				 cv::FONT_HERSHEY_SIMPLEX, 0.8, cv::Scalar(0xFF, 0xFF, 0xFF), 1, 8, false);
	cv::putText( img, label, cv::Point( getX( 0 ), 0 ), 
				 cv::FONT_HERSHEY_SIMPLEX, 0.8, cv::Scalar(0xFF, 0xFF, 0xFF), 1, 8);
    */
	}

// Funciones útiles donde una recta no está definida
double Line::calculateSlope(int x1, int y1, int x2, int y2) {
    // m = (y2 - y1) / (x2 - x1)
    return (double)(y2 - y1) / (x2 - x1);
	}

int Line::calculateYCross(int x, int y, double m) {
	// y = mx + b
    // b = y - mx
	return y - m * x;
	}

double Line::calculatePerpendicularSlope(double m) {
	// m1 * m2 = -1
    // m1 = -1 / m2
    return -1 / m;
	}
