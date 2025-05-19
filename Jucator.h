#ifndef JUCATOR_H
#define JUCATOR_H
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

enum TipArma { NORMAL, SHOTGUN, BAZOOKA};

class Jucator{
    private:
        Pozitie pozitie;
        int viata;
        int cooldown;//timpul in care jucatorul e invulnerabil  
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
        Jucator(float x, float y, int viata = 5) : pozitie(x, y), viata(viata), cooldown(0), scor(0), nivel(1), experienta(0), reloadTime(0) {}
        void muta(float dx, float dy) {//se misca jucatorul pe harta
            pozitie.miscari(dx, dy);
        }
        void tick() {
            if (cooldown > 0) cooldown--;//scade durata de invulnerabilitate
            if (reloadTime > 0) reloadTime--;//reload
            if (rapidFire > 0) rapidFire--;
            if (armaTimer > 0) {
                armaTimer--;
                if (armaTimer == 0){
                    armaCurenta = NORMAL;
                    std::cout << "[Arma s-a terminat, inapoi la pistolul normal]\n";
                }
            }
            if (comboTimer == 0 && comboKill > 0){
                comboKill = 0;
            }
        }
        void lovit(int damage = 1) {
            if (cooldown == 0 && viata > 0) {
                viata -= damage;
                cooldown = 3;//jucatorul primeste invulnerabilitate pentru 3 secunde pentru a avea timp sa se repozitioneze
            }
        }
        void castigaExperienta(int xp){
            experienta += xp;
            while(experienta >= xpNecesarPentruNivel()){
                experienta -= xpNecesarPentruNivel();
                nivel++;
                viata++;
            }
        }
        bool poateTrage()
        {
            if(reloadTime == 0)
            {
                reloadTime = (rapidFire > 0 ? reloadMax / 2 : reloadMax);
                return true;
            }
            return false;
        }
        int xpNecesarPentruNivel() const{
            return nivel * 10;
        }
        void adaugaItem(const std::string& item) {//doua iteme de viata si invulnerabilitate
            inventar.push_back(item);
            if (item == "Viata") viata++;
            else if (item == "Scut") cooldown += 3;
            else if (item == "RapidFire") rapidFire = 10;
            else if (item == "Shotgun"){
                armaCurenta = SHOTGUN;
                armaTimer = 10;
            }
            else if (item == "Bazooka"){
                armaCurenta = BAZOOKA;
                armaTimer = 10;
            }
        }
        void dodge() {
            cooldown = 2;
            std::cout << "[Jucatorul a efectuat o eschiva!]\n";
        }
        bool eViu() const {//self explanatory
            return viata > 0;
        }
        void scorPlus(int s) { 
            scor += s; 
            castigaExperienta(s);
        }
        std::vector<Proiectil> creeazaProiectile(){
            std::vector<Proiectil> gloante;
            if (armaCurenta == NORMAL){
                gloante.emplace_back(pozitie.getX(), pozitie.getY(), 1, 0, false);
            }
            else if (armaCurenta == SHOTGUN){
                gloante.emplace_back(pozitie.getX(), pozitie.getY(), 1, 0.1f, false);
                gloante.emplace_back(pozitie.getX(), pozitie.getY(), 1, -0.1f, false);
                gloante.emplace_back(pozitie.getX(), pozitie.getY(), 1, 0.0f, false);
            }
            else if (armaCurenta == BAZOOKA){
                gloante.emplace_back(pozitie.getX(), pozitie.getY(), 0.7f, 0, true);
            }
            return gloante;
        }
        /*void actualizeazaCombo(){
            comboKill++;
            comboTimer = 5;
            if (comboKill >= 5){
                adaugaItem("RapidFire");
                comboKill = 0;
            }
        }*/
        const Pozitie& getPozitie() const {
            return pozitie;
        }
        friend std::ostream& operator<<(std::ostream& os, const Jucator& j) {
            os << "Jucator la " << j.pozitie << " | Viata: " << j.viata << " | Scor: " << j.scor << " | XP: " << j.experienta << " | Inventar: [ ";
            for (const auto& item : j.inventar) os << item << " ";
            os << "]" << (j.cooldown ? " (invincibil)" : "");
            return os;
        }
        ~Jucator() {
            std::ofstream out("jucator_final.txt");//salvam stats-urile intr-un fisier cand moare jucatorul si ii golim inventarul
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
    };
#endif