#ifndef POZITIE_H
#define POZITIE_H

#include <iostream>
#include <vector>
#include <SFML/Graphics.hpp>
#include "ResourceManager.hpp"
#include <cmath>
#include <memory>
#include <fstream>
#include <sstream>
#include <cstdlib>
#include <ctime>
#include <algorithm>

class Pozitie{ //reprezinta un reper cartezian xoy pe care il vom folosi pentru a misca entitatile pe harta
    private:
        float x, y;
    public:
        explicit Pozitie(float x = 0, float y = 0) : x(x), y(y) {}
        float getX() const { return x; }
        float getY() const { return y; }
        void miscari(float dx, float dy, float maxX = 100, float maxY = 100) {//misca o entitate fara ca aceasta sa iasa din bounds
            x = std::max(0.0f, std::min(x + dx, maxX));
            y = std::max(0.0f, std::min(y + dy, maxY));
        }
        float distanta(const Pozitie& other) const {//calculeaza distanta dintre doua entitati
            return std::sqrt((x - other.x)*(x - other.x) + (y - other.y)*(y - other.y));
        }
        friend std::ostream& operator<<(std::ostream& os, const Pozitie& v) {
            os << "(" << v.x << ", " << v.y << ")";
            return os;
        }
    };
#endif