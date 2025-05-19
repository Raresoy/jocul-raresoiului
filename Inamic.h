#ifndef INAMIC_H
#define INAMIC_H
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

enum TipInamic { RANDOM, CHASER, SNIPER };


class Inamic{
    private:
        Pozitie pozitie;
        int cooldown;
        int viata;
        TipInamic tip;//vor exista mai multe tipuri de inamici ca sa nu se plictiseasca jucatorul luptandu-se cu un singur fel de inamic
    public://random se misca random, chaser fuge dupa jucator, iar sniper sta pe loc si trage lovituri puternice si precise
        Inamic(float x, float y, TipInamic tip = RANDOM, int viata = 3) : pozitie(x, y), cooldown(0), viata(viata), tip(tip) {}
        void actualizeaza(const Pozitie& jucatorPoz) {
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
                if (rand() % 5 == 0){
                    dx = ((rand() % 3) - 1) * 0.3f;
                    dy = ((rand() % 3) - 1) * 0.3f;
                }
            }
            pozitie.miscari(dx, dy);
            if (cooldown > 0) cooldown--;
        }
        /*std::vector<Proiectil> trageLaJucatorMulti(const Pozitie& tinta) {
            std::vector<Proiectil> gloante;
            float dx = tinta.getX() - pozitie.getX();
            float dy = tinta.getY() - pozitie.getY();
            float dist = std::sqrt(dx * dx + dy * dy);
            if (dist == 0) dist = 1;
            dx /= dist;
            dy /= dist;
            gloante.emplace_back(pozitie.getX(), pozitie.getY(), dx, dy, tip == SNIPER);
            gloante.emplace_back(pozitie.getX(), pozitie.getY(), dy, -dx, false);
            gloante.emplace_back(pozitie.getX(), pozitie.getY(), -dy, dx, false);
            return gloante;
        }*///deocamdata nefolosita
        Proiectil trageLaJucator(const Pozitie& tinta) {
            float dx = tinta.getX() - pozitie.getX();
            float dy = tinta.getY() - pozitie.getY();
            float dist = std::sqrt(dx * dx + dy * dy);
            if (dist == 0) dist = 1;
            return Proiectil(pozitie.getX(), pozitie.getY(), dx / dist, dy / dist, tip == SNIPER);
        }
        bool poateTrage() {
            if (cooldown == 0) {
                cooldown = (tip == SNIPER ? 2 : 4);
                return true;
            }
            return false;
        }
        Pozitie getPozitie() const {
            return pozitie;
        }
        void primesteDamage(int damage){
            viata -= damage;
            if (viata < 0) 
                viata = 0;
        }
        bool esteMort() const {
            return viata <= 0;
        }
        friend std::ostream& operator<<(std::ostream& os, const Inamic& i) {
            std::string tipStr = (i.tip == RANDOM) ? "Random" : (i.tip == CHASER ? "Chaser" : "Sniper");
            os << "Inamic [" << tipStr << "] la " << i.pozitie;
            return os;
        }
        ~Inamic(){
    
        }
    };
#endif