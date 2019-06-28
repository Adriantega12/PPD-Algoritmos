#ifndef FOOT_H
#define FOOT_H

#include <string>
#include <sstream>

class Foot {
    private:
        bool isLeft;

        // Foot data
        int x;
        int y;
        int ay;
        int ta;
        double percent;
        std::string footType;
        double zonePressure[4];

        // Image routes
        std::string parentRoute;
        std::string colorRoute;
        std::string hcRoute;

    public:
        Foot(std::string="", bool=true);

        void setX(int value);
        void setY(int value);
        void setAy(int value);
        void setTa(int value);
        void setPercent(double value);
        void setFootType(const std::string &value);
        void setZonePressure(int, double);
        void setParentRoute(const std::string &value);
        void setColorRoute(const std::string &value);
        void setHcRoute(const std::string &value);

        int getX() const;
        int getY() const;
        int getAy() const;
        int getTa() const;
        double getPercent() const;
        std::string getFootType() const;
        double getZonePressure(int);
        std::string getParentRoute() const;
        std::string getColorRoute() const;
        std::string getHcRoute() const;

        std::string toJSON();
};

#endif // FOOT_H
