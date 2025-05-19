#ifndef PROIECTIL_H
#define PROIECTIL_H
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
#include "Pozitie.h"

class Proiectil {
    private:
        Pozitie pozitie;
        float dx, dy;
        bool exploziv;
    public:
        Proiectil(float x, float y, float dx = 0, float dy = -1.0f, bool exploziv = false) : pozitie(x, y), dx(dx), dy(dy), exploziv(exploziv) {}
        Proiectil(const Proiectil& other) : pozitie(other.pozitie), dx(other.dx), dy(other.dy), exploziv(other.exploziv) {
            std::cout << "[Proiectil copiat]\n";
        }
        Proiectil& operator=(const Proiectil& other){
            if (this != &other){
                pozitie = other.pozitie;
                dx = other.dx;
                dy = other.dy;
                exploziv = other.exploziv;
            }
            std::cout << "[Proiectil asignat]\n";
            return *this;
        }
        void actualizeaza() {
            pozitie.miscari(dx * 2.5f, dy * 2.5f);
        }
        Pozitie getPozitie() const {
            return pozitie;
        }
        bool esteExploziv() const { return exploziv; }
        friend std::ostream& operator<<(std::ostream& os, const Proiectil& p) {
            os << "Proiectil la " << p.pozitie;
        return os;
    }
    ~Proiectil(){
    }
};
#endif