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

void Foot::setParentRoute(const std::string& value) {
    parentRoute = value;
    }

void Foot::setColorRoute(const std::string& value) {
    colorRoute = value;
    }

void Foot::setHcRoute(const std::string& value) {
    hcRoute = value;
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

std::string Foot::getParentRoute() const {
    return parentRoute;
    }

std::string Foot::getColorRoute() const {
    return colorRoute;
    }

std::string Foot::getHcRoute() const {
    return hcRoute;
    }

std::string Foot::toJSON() {
    std::ostringstream json;
    std::string type;

    if (percent <= 34.0) {
        type = "Plano";
    } else if (percent <= 39.0) {
        type = "Plano - Normal";
    } else if (percent <= 54.0) {
        type = "Normal";
    } else if (percent <= 59.0) {
        type = "Normal - Cavo";
    } else if (percent <= 74) {
        type = "Cavo";
    } else if (percent <= 84.0) {
        type = "Cavo - Fuerte";
    } else if (percent <= 100.0) {
        type = "Cavo - Extremo";
    }

    json << "\""
         << (isLeft ? "left" : "right")
         << "\":{"
         << "\"original\":\"" << parentRoute
         << "\"," "\"color\":\"" << colorRoute << "\","
         << "\"hernandez-corvo\":\"" << hcRoute << "\","
         << "\"x\":" << std::to_string(Foot::pixelsToCm(x)) << ","
         << "\"y\":" << std::to_string(Foot::pixelsToCm(y)) << ","
         << "\"ay\":" << std::to_string(Foot::pixelsToCm(ay)) << ","
         << "\"ta\":" << std::to_string(Foot::pixelsToCm(ta)) << ","
         << "\"percent\":" << std::to_string(percent) << ","
         << "\"type\":\"" << type << "\","
         << "\"z1\":" << std::to_string(zonePressure[0]) << ","
         << "\"z2\":" << std::to_string(zonePressure[1]) << ","
         << "\"z3\":" << std::to_string(zonePressure[2]) << ","
         << "\"z4\":" << std::to_string(zonePressure[3])
         << "}";

    return json.str();
    }

double Foot::pixelsToCm(int pixels) {
    return pixels * 24.0 / 285.0;
    }
