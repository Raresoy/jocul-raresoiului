#ifndef JUCATOR_H
#define JUCATOR_H
#include <iostream>
#include <vector>
#include <fstream>
#include <string>
#include <memory>
#include "Entitate.h"
#include "Pozitie.h"
#include "Proiectil.h"

enum TipArma { NORMAL, SHOTGUN, BAZOOKA };

class Jucator : public Entitate {
private:
    int cooldown;  // timpul in care jucatorul e invulnerabil  
    int scor;
    int nivel;
    int experienta;
    int reloadTime;
    const int reloadMax = 3;
    int rapidFire = 0;
    int comboKill = 0;
    int comboTimer = 0;
    std::vector<std::string> inventar;
    TipArma armaCurenta = NORMAL;
    int armaTimer = 0;
    
public:
    Jucator(float x, float y, int viata = 5) : Entitate(x, y, viata), cooldown(0), scor(0), nivel(1), experienta(0), reloadTime(0) {}

    Jucator(const Jucator& other) : Entitate(other.pozitie.getX(), other.pozitie.getY(), other.viata),
        cooldown(other.cooldown), scor(other.scor), nivel(other.nivel),
        experienta(other.experienta), reloadTime(other.reloadTime), 
        rapidFire(other.rapidFire), comboKill(other.comboKill),
        comboTimer(other.comboTimer), inventar(other.inventar),
        armaCurenta(other.armaCurenta), armaTimer(other.armaTimer) {}
    
    virtual Entitate* clone() const override {
        return new Jucator(*this);
    }
    
    void muta(float dx, float dy) {
        pozitie.miscari(dx, dy);
    }
  
    virtual void actualiz(const Pozitie& target) override {
        if (cooldown > 0) cooldown--;
        if (reloadTime > 0) reloadTime--;
        if (rapidFire > 0) rapidFire--;
        if (armaTimer > 0) {
            armaTimer--;
            if (armaTimer == 0) {
                armaCurenta = NORMAL;
                std::cout << "[Arma s-a terminat, inapoi la pistolul normal]\n";
            }
        }
        if (comboTimer == 0 && comboKill > 0) {
            comboKill = 0;
        }
    }
    
    virtual void primesteDamage(int damage) override {
        if (cooldown == 0 && viata > 0) {
            viata -= damage;
            cooldown = 3;
        }
    }
    
    void castigaExperienta(int xp) {
        experienta += xp;
        while (experienta >= xpNecesarPentruNivel()) {
            experienta -= xpNecesarPentruNivel();
            nivel++;
            viata++;
        }
    }
    
    bool poateTrage() {
        if (reloadTime == 0) {
            reloadTime = (rapidFire > 0 ? reloadMax / 2 : reloadMax);
            return true;
        }
        return false;
    }
    
    int xpNecesarPentruNivel() const {
        return nivel * 10;
    }
    
    void adaugaItem(const std::string& item) {
        inventar.push_back(item);
        if (item == "Viata") viata++;
        else if (item == "Scut") cooldown += 3;
        else if (item == "RapidFire") rapidFire = 10;
        else if (item == "Shotgun") {
            armaCurenta = SHOTGUN;
            armaTimer = 10;
        }
        else if (item == "Bazooka") {
            armaCurenta = BAZOOKA;
            armaTimer = 10;
        }
    }
    
    void dodge() {
        cooldown = 2;
        std::cout << "[Jucatorul a efectuat o eschiva!]\n";
    }
    
    bool eViu() const {
        return !esteMort();
    }
    
    void scorPlus(int s) { 
        scor += s; 
        castigaExperienta(s);
    }
    
    std::vector<Proiectil> creeazaProiectile() {
        std::vector<Proiectil> gloante;
        if (armaCurenta == NORMAL) {
            gloante.emplace_back(pozitie.getX(), pozitie.getY(), 1, 0, false);
        }
        else if (armaCurenta == SHOTGUN) {
            gloante.emplace_back(pozitie.getX(), pozitie.getY(), 1, 0.1f, false);
            gloante.emplace_back(pozitie.getX(), pozitie.getY(), 1, -0.1f, false);
            gloante.emplace_back(pozitie.getX(), pozitie.getY(), 1, 0.0f, false);
        }
        else if (armaCurenta == BAZOOKA) {
            gloante.emplace_back(pozitie.getX(), pozitie.getY(), 0.7f, 0, true);
        }
        return gloante;
    }
    
    ~Jucator() {
        std::ofstream out("jucator_final.txt");
        if (out.is_open()) {
            out << "Stare finala jucator:\n";
            out << "Pozitie: " << pozitie << "\n";
            out << "Viata: " << viata << "\n";
            out << "Scor: " << scor << "\n";
            out << "Inventar: ";
            for (const auto& item : inventar)
                out << item << " ";
            out << "\n";
            out.close();
        } else {
            std::cerr << "[Eroare: Nu s-a putut scrie fisierul jucator_final.txt]\n";
        }
        inventar.clear();
        std::cout << "[Jucator distrus: salvare finala completata.]\n";
    }
protected:
    virtual void afiseaza(std::ostream& os) const override {
        os << "Jucator la " << pozitie << " | Viata: " << viata << " | Scor: " << scor << " | XP: " << experienta << " | Inventar: [ ";
        for (const auto& item : inventar) os << item << " ";
        os << "]" << (cooldown ? " (invincibil)" : "");
    }
};

#endif