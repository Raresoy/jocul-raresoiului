#ifndef JUCATOR_H
#define JUCATOR_H
#include <iostream>
#include <vector>
#include <fstream>
#include "GameException.h"
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
    Jucator(float x, float y, int viata = 5) : Entitate(x, y, viata), cooldown(0), scor(0), nivel(1), experienta(0), reloadTime(0) {
        if (viata <= 0) {
            throw EntityException("Player cannot be created with non-positive health", "Jucator");
        }
    }

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
        try {
            if (esteMort()) {
                throw DeadEntityException("movement", "Jucator");
            }
            pozitie.miscari(dx, dy);
        } 
        catch (const DeadEntityException& e) {
            ExceptionHandler::handleException(e);
            ExceptionHandler::logRecoveryAction("Ignora comanda unui jucator mort");
        }
    }
  
    virtual void actualiz(const Pozitie&) override {
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
        try {
            if (damage < 0) {
                throw CombatException("Damage negativ", "Player Damage");
            }
            
            if (esteMort()) {
                throw DeadEntityException("damage application", "Jucator");
            }
            
            if (cooldown == 0 && viata > 0) {
                viata -= damage;
                cooldown = 3;
                
                if (viata < 0) viata = 0;
            }
        } 
        catch (const CombatException& e) {
            ExceptionHandler::handleException(e);
        } 
        catch (const DeadEntityException& e) {
            ExceptionHandler::handleException(e);
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

    virtual void interactCu(Entitate& other) override {
        float distanta = pozitie.distanta(other.getPozitie());
        if (distanta < 5.0f) {
            primesteDamage(1);
            other.primesteDamage(1);
            experienta += 1;
        }
    }

    virtual int getViata() override { return viata; }
    
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
        try {
            if (item.empty()) {
                throw PowerupException("", "Empty powerup");
            }
            std::vector<std::string> validTypes = {"Viata", "Scut", "RapidFire", "Shotgun", "Bazooka"};
            bool valid = false;
            for (const auto& type : validTypes) {
                if (item == type) {
                    valid = true;
                    break;
                }
            }
            
            if (!valid) {
                throw PowerupException(item, "Powerup necunoscut");
            }
            inventar.push_back(item);
            SAFE_EXECUTE({
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
            }, "Se aplica powerup-ul..");
            
        } 
        catch (const PowerupException& e) {
            ExceptionHandler::handleException(e);
            ExceptionHandler::logRecoveryAction("Ignora powerup nevalid: " + item);
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
        try {
            if (esteMort()) {
                throw DeadEntityException("projectile creation", "Jucator");
            }
            switch (armaCurenta) {
                case NORMAL:
                    gloante.emplace_back(pozitie.getX(), pozitie.getY(), 1, 0, false);
                    break;
                case SHOTGUN:
                    gloante.emplace_back(pozitie.getX(), pozitie.getY(), 1, 0.1f, false);
                    gloante.emplace_back(pozitie.getX(), pozitie.getY(), 1, -0.1f, false);
                    gloante.emplace_back(pozitie.getX(), pozitie.getY(), 1, 0.0f, false);
                    break;
                case BAZOOKA:
                    gloante.emplace_back(pozitie.getX(), pozitie.getY(), 0.7f, 0, true);
                    break;
                default:
                    throw WeaponException("Unknown", "Arma invalida");
            }
        } 
        catch (const DeadEntityException& e) {
            ExceptionHandler::handleException(e);
            return {}; 
        } 
        catch (const WeaponException& e) {
            ExceptionHandler::handleException(e);
            gloante.emplace_back(pozitie.getX(), pozitie.getY(), 1, 0, false);
            ExceptionHandler::logRecoveryAction("Arma normala");
        }
        return gloante;
    }
    
protected:
    virtual void afiseaza(std::ostream& os) const override {
        os << "Jucator la " << pozitie << " | Viata: " << viata << " | Scor: " << scor << " | XP: " << experienta << " | Inventar: [ ";
        for (const auto& item : inventar) os << item << " ";
        os << "]" << (cooldown ? " (invincibil)" : "");
    }
};

#endif