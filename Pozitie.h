#ifndef POZITIE_H
#define POZITIE_H

#include <iostream>
#include <vector>
#include <SFML/Graphics.hpp>
#include "ResourceManager.hpp"
#include <cmath>
#include "GameException.h"
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
        explicit Pozitie(float x = 0, float y = 0) : x(x), y(y) {
            if (x < 0 || y < 0) {
                throw InvalidPositionException(x, y, "Pozitie Constructor");
            }
        }
        float getX() const { return x; }
        float getY() const { return y; }
        void miscari(float dx, float dy, float maxX = 100, float maxY = 100) {//misca o entitate fara ca aceasta sa iasa din bounds
            try {
                float newX = x + dx;
                float newY = y + dy;
                if (newX < 0 || newX > maxX || newY < 0 || newY > maxY) {
                    if (newX < -10 || newX > maxX + 10 || newY < -10 || newY > maxY + 10) {
                        throw InvalidPositionException(newX, newY, "Miscare out of bounds");
                    }
                }
                x = std::max(0.0f, std::min(newX, maxX));
                y = std::max(0.0f, std::min(newY, maxY));
            } 

            catch (const InvalidPositionException& e) {
                ExceptionHandler::handleException(e);
                x = std::max(0.0f, std::min(x + dx, maxX));
                y = std::max(0.0f, std::min(y + dy, maxY));
            }
        }
        float distanta(const Pozitie& other) const {//calculeaza distanta dintre doua entitati
            try {
                float dx = x - other.x;
                float dy = y - other.y;
                float dist = std::sqrt(dx * dx + dy * dy);
                if (std::isnan(dist) || std::isinf(dist)) {
                    throw EntityException("Calcularea invalida a distantei", "Distanta pozitiei");
                }
                return dist;
            } 
            catch (const EntityException& e) {
                ExceptionHandler::handleException(e);
                return 0.0f; // Safe fallback
            }
        }
        friend std::ostream& operator<<(std::ostream& os, const Pozitie& v) {
            os << "(" << v.x << ", " << v.y << ")";
            return os;
        }
    };
#endif