#ifndef INAMIC_H
#define INAMIC_H
#include <iostream>
#include <cmath>
#include <vector>
#include <memory>
#include <fstream>
#include <sstream>
#include <cstdlib>
#include <ctime>
#include <algorithm>
#include "Entitate.h"
#include "Proiectil.h"
#include "Pozitie.h"
#include "Jucator.h"

enum TipInamic { RANDOM, CHASER, SNIPER };

class Inamic : public Entitate {
private:
    int cooldown;
    TipInamic tip;
public:
    Inamic(float x, float y, TipInamic tip = RANDOM, int viata = 3) : Entitate(x, y, viata), cooldown(0), tip(tip) {}
    Inamic(const Inamic& other) : Entitate(other.pozitie.getX(), other.pozitie.getY(), other.viata), cooldown(other.cooldown), tip(other.tip) {}

    virtual Entitate* clone() const override {
        return new Inamic(*this);
    }

    virtual void actualiz(const Pozitie& jucatorPoz) override {
        float dx = 0, dy = 0;
        if (tip == RANDOM) {
            dx = ((rand() % 3) - 1) * 0.5f;
            dy = ((rand() % 3) - 1) * 0.5f;
        } 
        else if (tip == CHASER) {
            dx = jucatorPoz.getX() - pozitie.getX();
            dy = jucatorPoz.getY() - pozitie.getY();
            float dist = std::sqrt(dx * dx + dy * dy);
            dx = (dist > 0) ? dx / dist * 0.7f : 0;
            dy = (dist > 0) ? dy / dist * 0.7f : 0;
        } 
        else if (tip == SNIPER) {
            if (rand() % 5 == 0) {
                dx = ((rand() % 3) - 1) * 0.3f;
                dy = ((rand() % 3) - 1) * 0.3f;
            }
        }
        pozitie.miscari(dx, dy);
        if (cooldown > 0) cooldown--;
    }

    virtual void primesteDamage(int damage) override {
        viata -= damage;
        if (viata < 0) 
            viata = 0;
    }

    bool poateTrage() {
        if (cooldown == 0) {
            cooldown = (tip == SNIPER ? 2 : 4);
            return true;
        }
        return false;
    }

    Proiectil trageLaJucator(const Pozitie& tinta) {
        float dx = tinta.getX() - pozitie.getX();
        float dy = tinta.getY() - pozitie.getY();
        float dist = std::sqrt(dx * dx + dy * dy);
        if (dist == 0) dist = 1;
        return Proiectil(pozitie.getX(), pozitie.getY(), dx / dist, dy / dist, tip == SNIPER);
    }

    TipInamic getTip() const {
        return tip;
    }

protected:
    virtual void afiseaza(std::ostream& os) const override {
        std::string tipStr = (tip == RANDOM) ? "Random" : (tip == CHASER ? "Chaser" : "Sniper");
        os << "Inamic [" << tipStr << "] la " << pozitie << " | Viata: " << viata;
    }
};

#endif