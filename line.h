#ifndef _LINE_H
#define _LINE_H

#include "opencv2/imgcodecs.hpp"
#include <opencv2/imgproc.hpp>

#include <string>

class Line {
	private:
        double m;
        int b;
		std::string label;

	public:
		// Recta dada una pendiente y una ordenada al origen
        Line(double = 0, int = 0, std::string = "");
		// Recta con dos puntos
        Line(int, int, int, int);
		// Recta punto-pendiente
        Line(double, int, int);
		// Recta paralela con ordenada al origen relativa
        Line(Line&, int);
		// Recta perpendicular que pasa por un punto definido de otra recta
		Line(Line&, float, float);

		// Getters
		float getSlope();
		float getYCross();

		// Setters
		void setSlope(float);
		void setYCross(float);

		// Funciones útiles dada una recta definida
		float getY(float);
		float getX(float);

		// Etiqueta
		void setLabel(std::string);
		std::string getLabel();

		// Obtener punto en X de intersección con otra línea
		float getIntersectPoint(Line&);

		// Dibujar línea
		void draw( cv::Mat&, cv::Scalar );

		// Funciones útiles donde una recta no está definida
		static float calculateSlope(float, float, float, float);
		static float calculateYCross(float, float, float);
		static float calculatePerpendicularSlope(float);
	};

#endif // _LINE_H
