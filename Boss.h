#ifndef BOSS_H
#define BOSS_H
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
#include "Inamic.h"
#include "Jucator.h"
#include "Proiectil.h"
#include "Pozitie.h"


class Boss{//boss care apare dupa ce invingi 20 de wave-uri, mare, tanky, se misca incet si trage multe gloante
    private:
        Pozitie pozitie;
        int viata;
        int cooldown;
        int specialcooldown;
    public:
        Boss(float x, float y) : pozitie(x, y), viata(100), cooldown(0), specialcooldown(5) {}
        void actualizeaza(const Pozitie& jucatorPoz) {
            float dx = jucatorPoz.getX() - pozitie.getX();
            float dy = jucatorPoz.getY() - pozitie.getY();
            float dist = std::sqrt(dx * dx + dy * dy);
            if (dist > 0){
                dx = dx / dist * 0.4f;
                dy = dy / dist * 0.4f;
            }
            pozitie.miscari(dx, dy);
            if (cooldown > 0) cooldown--;
            if (specialcooldown > 0) specialcooldown--;
        }
        bool poateTrage() {
            if (cooldown == 0){
                cooldown = 2;
                return true;
            }
            return false;
        }
        bool poateAtacSpecial(){
            if(specialcooldown == 0){
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
        std::vector<Proiectil> atacSpirala(int tura){//atac special trage gloante in directii diferite, acestea rotindu-se
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
        std::vector<Proiectil> atacExplozie() {//un atac prin care boss-ul trage cate un glont in fiecare directie
            std::vector<Proiectil> gloante;
            for (float angle = 0; angle < 360; angle += 20) {
                float rad = angle * 3.14159f / 180.0f;
                float dx = std::cos(rad);
                float dy = std::sin(rad);
                gloante.emplace_back(pozitie.getX(), pozitie.getY(), dx, dy, true);
            }
            return gloante;
        }
        void primesteDamage(int damage) {
            viata -= damage;
            if (viata < 0) viata = 0;
        }
        bool esteMort() const {
            return viata <= 0;
        }
        Pozitie getPozitie() const {
            return pozitie;
        }
        friend std::ostream& operator<<(std::ostream& os, const Boss& b) {
            os << "👑 Boss la " << b.pozitie << " | Viata: " << b.viata;
            return os;
        }
    };
#endif