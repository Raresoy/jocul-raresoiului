#ifndef BOSS_H
#define BOSS_H
#include <iostream>
#include <vector>
#include <cmath>
#include "Entitate.h"
#include "Proiectil.h"

class Boss : public Entitate {
private:
    int cooldown;
    int specialcooldown;

public:
    Boss(float x, float y) : Entitate(x, y, 100), cooldown(0), specialcooldown(5) {}
    
    Boss(const Boss& other) : Entitate(other.pozitie.getX(), other.pozitie.getY(), other.viata), cooldown(other.cooldown), specialcooldown(other.specialcooldown) {}
    virtual Entitate* clone() const override {
        return new Boss(*this);
    }

    virtual void actualiz(const Pozitie& jucatorPoz) override {
        float dx = jucatorPoz.getX() - pozitie.getX();
        float dy = jucatorPoz.getY() - pozitie.getY();
        float dist = std::sqrt(dx * dx + dy * dy);
        if (dist > 0) {
            dx = dx / dist * 0.4f;
            dy = dy / dist * 0.4f;
        }
        pozitie.miscari(dx, dy);
        if (cooldown > 0) cooldown--;
        if (specialcooldown > 0) specialcooldown--;
    }

    virtual void primesteDamage(int damage) override {
        viata -= damage;
        if (viata < 0) viata = 0;
    }

    bool poateTrage() {
        if (cooldown == 0) {
            cooldown = 2;
            return true;
        }
        return false;
    }

    bool poateAtacSpecial() {
        if (specialcooldown == 0) {
            specialcooldown = 8 + rand() % 5;
            return true;
        }
        return false;
    }

    std::vector<Proiectil> tragePattern() {
        std::vector<Proiectil> gloante;
        for (float unghi = 0; unghi < 360; unghi += 45) {
            float rad = unghi * 3.14159f / 180.0f;
            float dx = std::cos(rad);
            float dy = std::sin(rad);
            gloante.emplace_back(pozitie.getX(), pozitie.getY(), dx, dy, true);
        }
        return gloante;
    }

    std::vector<Proiectil> atacSpirala(int tura) {
        std::vector<Proiectil> gloante;
        float offset = (tura % 360) * 3.14159f / 180.0f;
        for (float angle = 0; angle < 360; angle += 30) {
            float rad = angle * 3.14159f / 180.0f + offset;
            float dx = std::cos(rad);
            float dy = std::sin(rad);
            gloante.emplace_back(pozitie.getX(), pozitie.getY(), dx, dy, true);
        }
        return gloante;
    }

    std::vector<Proiectil> atacExplozie() {
        std::vector<Proiectil> gloante;
        for (float angle = 0; angle < 360; angle += 20) {
            float rad = angle * 3.14159f / 180.0f;
            float dx = std::cos(rad);
            float dy = std::sin(rad);
            gloante.emplace_back(pozitie.getX(), pozitie.getY(), dx, dy, true);
        }
        return gloante;
    }

protected:
    virtual void afiseaza(std::ostream& os) const override {
        os << "👑 Boss la " << pozitie << " | Viata: " << viata;
    }
};

#endif