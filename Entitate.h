#ifndef ENTITATE_H
#define ENTITATE_H
#include <iostream>
#include <cmath>
#include "Pozitie.h"


class Entitate {
protected:
    Pozitie pozitie;
    int viata;
    Entitate(float x, float y, int viata) : pozitie(x, y), viata(viata) {}

public:
    virtual ~Entitate() = default;
    virtual void actualiz(const Pozitie& target) = 0;
    virtual void primesteDamage(int damage) = 0;
    virtual bool esteMort() const {
        return viata <= 0;
    }
    Pozitie getPozitie() const {
        return pozitie;
    }
    virtual int getViata() { return viata; }
    virtual Entitate* clone() const = 0;
    static float calculeazaDistanta(const Entitate& e1, const Entitate& e2) {
        return e1.getPozitie().distanta(e2.getPozitie());
    }
    virtual void interactCu(Entitate& other) {
        float distanta = pozitie.distanta(other.getPozitie());
        if (distanta < 10.0f) {
            // Default behavior is to damage the other entity if close enough
            other.primesteDamage(1);
        }
    }
    friend std::ostream& operator<<(std::ostream& os, const Entitate&) {
        return os;
    }
protected:
    virtual void afiseaza(std::ostream& os) const = 0;
};

#endif