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
    virtual Entitate* clone() const = 0;
    friend std::ostream& operator<<(std::ostream& os, const Entitate&) {
        return os;
    }
protected:
    virtual void afiseaza(std::ostream& os) const = 0;
};

#endif