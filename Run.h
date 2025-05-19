#ifndef RUN_H
#define RUN_H
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
#include "Inamic.h"
#include "Boss.h"
#include "Powerup.h"

class Run{
    private:
        Jucator jucator;
        std::vector<Inamic> inamici;
        std::vector<Proiectil> proiectileJucator;
        std::vector<Proiectil> proiectileInamici;
        std::vector<Powerup> powerups;
        Boss* boss;
        int wave;
        int tura;
        bool jocActiv;
    public:
        Run() : jucator(50, 90), boss(nullptr), wave(1), tura(0), jocActiv(true) {
            srand(static_cast<unsigned>(time(0)));
            spawnWave();
        }
        void spawnWave() {
            int nrInamici = 5 + wave; // mai multi inamici pe masura ce avansezi
            for (int i = 0; i < nrInamici; ++i) {
                TipInamic tip = static_cast<TipInamic>(rand() % 3);
                float x = rand() % 100;
                float y = rand() % 100;
                inamici.emplace_back(x, y, tip);
            }
            std::cout << "[Wave " << wave << " incepe cu " << nrInamici << " inamici!]\n";
        }
        void spawnBoss() {
            boss = new Boss(50, 10);
            std::cout << "[BOSS APARUT!]\n";
        }
        void spawnPowerup(float x, float y) {
            std::vector<std::string> tipuri = { "Viata", "RapidFire", "Scut", "Shotgun", "Bazooka" };
            std::string ales = tipuri[rand() % tipuri.size()];
            powerups.emplace_back(x, y, ales);
        }
        void inputPlayer() {
            char comanda;
            std::cout << "Comanda (w/a/s/d pentru miscare, f pentru foc, e pentru dodge): ";
            std::cin >> comanda;
            if (comanda == 'w') jucator.muta(0, -3);
            else if (comanda == 's') jucator.muta(0, 3);
            else if (comanda == 'a') jucator.muta(-3, 0);
            else if (comanda == 'd') jucator.muta(3, 0);
            else if (comanda == 'e') jucator.dodge();
            else if (comanda == 'f') {
                if (jucator.poateTrage()) {
                    auto gl = jucator.creeazaProiectile();
                    proiectileJucator.insert(proiectileJucator.end(), gl.begin(), gl.end());
                }
            }
        }
        void actualizeaza() {
            jucator.tick();
            tura++;
            for (auto& p : proiectileJucator)
                p.actualizeaza();
            for (auto& p : proiectileInamici)
                p.actualizeaza();
            for (auto& i : inamici)
                i.actualizeaza(jucator.getPozitie());
            if (boss)
                boss->actualizeaza(jucator.getPozitie());
            for (auto& p : powerups)
                p.verificaColectare(jucator);
            for (const auto& p : proiectileJucator) {
                for (auto& i : inamici) {
                    if (!i.esteMort() && p.getPozitie().distanta(i.getPozitie()) < 5.0f) {
                        i.primesteDamage(p.esteExploziv() ? 3 : 1);
                        jucator.scorPlus(1);
                        if (rand() % 10 == 0)
                            spawnPowerup(i.getPozitie().getX(), i.getPozitie().getY());
                    }
                }
                if (boss && p.getPozitie().distanta(boss->getPozitie()) < 5.0f) {
                    boss->primesteDamage(p.esteExploziv() ? 5 : 2);
                }
            }
            proiectileJucator.erase(std::remove_if(proiectileJucator.begin(), proiectileJucator.end(), [](const Proiectil& p) { return p.getPozitie().getX() < 0 || p.getPozitie().getX() > 100 || p.getPozitie().getY() < 0 || p.getPozitie().getY() > 100; }), proiectileJucator.end());
            proiectileInamici.erase(std::remove_if(proiectileInamici.begin(), proiectileInamici.end(), [](const Proiectil& p) { return p.getPozitie().getX() < 0 || p.getPozitie().getX() > 100 || p.getPozitie().getY() < 0 || p.getPozitie().getY() > 100; }), proiectileInamici.end());
            for (const auto& p : proiectileInamici) {
                if (p.getPozitie().distanta(jucator.getPozitie()) < 5.0f) {
                    jucator.lovit(p.esteExploziv() ? 2 : 1);
                }
            }
            for (auto& i : inamici) {
                if (i.poateTrage()) {
                    auto p = i.trageLaJucator(jucator.getPozitie());
                    proiectileInamici.push_back(p);
                }
            }
            if (boss) {
                if (boss->poateTrage()) {
                    auto p = boss->tragePattern();
                    proiectileInamici.insert(proiectileInamici.end(), p.begin(), p.end());
                }
                if (boss->poateAtacSpecial()) {
                    int k = rand() % 2;
                    if (k == 0){
                        auto p = boss->atacSpirala(tura);
                        proiectileInamici.insert(proiectileInamici.end(), p.begin(), p.end());
                    }
                    else{
                         auto p = boss->atacExplozie();
                         proiectileInamici.insert(proiectileInamici.end(), p.begin(), p.end());
                    }
                }
            }
            inamici.erase(std::remove_if(inamici.begin(), inamici.end(), [](const Inamic& i) { return i.esteMort(); }), inamici.end());
            powerups.erase(std::remove_if(powerups.begin(), powerups.end(), [this](Powerup& p) { return p.verificaColectare(jucator); }), powerups.end());
            if (inamici.empty() && !boss) {
                wave++;
                if (wave <= 20)
                    spawnWave();
                else
                    spawnBoss();
            }
        }
        void ruleaza() {
            while (jocActiv) {
                std::cout << jucator << "\n";
                inputPlayer();
                actualizeaza();
                if (!jucator.eViu()) {
                    std::cout << "[AI PIERDUT... GAME OVER]\n";
                    jocActiv = false;
                }
                if (boss && boss->esteMort()) {
                    std::cout << "[FELICITARI! AI INVINS BOSSUL SI AI CASTIGAT JOCUL!]\n";
                    jocActiv = false;
                }
            }
        }
        ~Run() {
            if (boss) delete boss;
        }
    };
#endif