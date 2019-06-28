#include "foot.h"

Foot::Foot(std::string route, bool iL) : parentRoute(route), isLeft(iL) { }

void Foot::setX(int value) {
    x = value;
    }

void Foot::setY(int value) {
    y = value;
    }

void Foot::setAy(int value) {
    ay = value;
    }

void Foot::setTa(int value) {
    ta = value;
    }

void Foot::setPercent(double value) {
    percent = value;
    }

void Foot::setFootType(const std::string& value) {
    footType = value;
}

void Foot::setZonePressure(int index, double percent) {
    zonePressure[index] = percent;
    }

int Foot::getX() const {
    return x;
    }

int Foot::getY() const {
    return y;
    }

int Foot::getAy() const {
    return ay;
    }

int Foot::getTa() const {
    return ta;
    }

double Foot::getPercent() const {
    return percent;
    }

std::string Foot::getFootType() const {
    return footType;
    }

double Foot::getZonePressure(int index) {
    return zonePressure[index];
    }

std::string Foot::toJSON() {
    std::ostringstream json;

    json << "\""
         << (isLeft ? "left" : "right")
         << "\": {"
         << "\"original\": \"" << parentRoute
         << "\", " "\"color\": \"" << colorRoute << "\", "
         << "\"hernandez-corvo\": \"" << hcRoute << "\", "
         << "\"x\": " << std::to_string(x) << ", "
         << "\"y\": " << std::to_string(y) << ", "
         << "\"ay\": " << std::to_string(ay) << ", "
         << "\"ta\": " << std::to_string(ta) << ", "
         << "\"percent\": " << std::to_string(percent) << "}";

    return json.str();
    }


