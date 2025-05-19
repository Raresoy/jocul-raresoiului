#ifndef Powerup_h
#define Powerup_h
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
#include "Proiectil.h"
#include "Jucator.h"

class Powerup {
private:
    Pozitie pozitie;
    std::string tip;//"Viata", "RapidFire", "Scut", "MultiShot", "Bazooka", "Shotgun"
public:
    Powerup(float x, float y, const std::string& tip) : pozitie(x, y), tip(tip) {}
    const Pozitie& getPozitie() const { return pozitie; }
    bool verificaColectare(Jucator& jucator) {
        if (pozitie.distanta(jucator.getPozitie()) < 2.0f) {
            jucator.adaugaItem(tip);
            return true;
        }
        return false;
    }
    friend std::ostream& operator<<(std::ostream& os, const Powerup& p) {
        os << "Powerup [" << p.tip << "] la " << p.pozitie;
        return os;
    }
    ~Powerup(){
    }
};
#endif