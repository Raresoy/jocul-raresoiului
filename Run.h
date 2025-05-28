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
#include <thread>
#include <chrono>
#include <map>
#include <iomanip>
#include <cctype>
#include <string>
#include "GameException.h"
#include <limits>
#include "Pozitie.h"
#include "Proiectil.h"
#include "Jucator.h"
#include "Inamic.h"
#include "Boss.h"
#include "Powerup.h"

class Run {
private:
    Jucator jucator;
    std::vector<std::unique_ptr<Entitate>> inamici;
    std::vector<Proiectil> proiectileJucator;
    std::vector<Proiectil> proiectileInamici;
    std::vector<Powerup> powerups;
    std::unique_ptr<Boss> boss;
    int wave;
    int tura;
    int turaCurenta;
    bool jocActiv;
    bool modPauza;
    int scor;
    std::map<std::string, int> statistici;
    std::vector<std::string> mesajeEvenimente;
    char ultimaComanda;
    int dificultate; // 1 = ușor, 2 = normal, 3 = greu
    int delayActiune; // ms între turele jocului
    void clearScreen() {
        #ifdef _WIN32
        system("cls");
        #else
        system("clear");
        #endif
    }
    void asteptare(int ms) {
        std::this_thread::sleep_for(std::chrono::milliseconds(ms));
    }
    bool criticalErrorOccurred = false;

public:
    Run() : jucator(50, 90), wave(1), tura(0), turaCurenta(0), jocActiv(true), modPauza(false), 
            scor(0), ultimaComanda(' '), dificultate(2), delayActiune(200) {
        srand(static_cast<unsigned>(time(0)));
        initJoc();
    }

    void initJoc() {
        try {
            clearScreen();
            std::cout << "=== The Adventures of Raresoi ===\n\n";
            int tentative = 0;
            while (tentative < 3) {
                try {
                    std::cout << "Alege nivelul de dificultate:\n";
                    std::cout << "1 - Usor\n2 - Normal\n3 - Greu\n";
                    std::cout << "Alegerea ta (1-3): ";
                    char optiune;
                    std::cin >> optiune;
                    if (optiune < '1' || optiune > '3') {
                        throw InvalidCommandException(optiune);
                    }
                    dificultate = optiune - '0';
                    delayActiune = (dificultate == 1) ? 250 : (dificultate == 3) ? 150 : 200;
                    break;
                } 
                catch (const InvalidCommandException& e) {
                    ExceptionHandler::handleException(e);
                    tentative++;
                    if (tentative < 3) {
                        std::cout << "Te rog introdu o optiune valida (1-3). Incercari ramase: " 
                                  << (3 - tentative) << std::endl;
                    }
                }
            }
            
            if (tentative >= 3) {
                ExceptionHandler::logRecoveryAction("S-a folosit dificultatea default (Normal)");
                dificultate = 2;
                delayActiune = 200;
            }
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            
        } 
        catch (const std::exception& e) {
            throw SystemException("Initializam jocul..", e.what());
        }
    }
    void afisareControale() {
        clearScreen();
        std::cout << "=== CONTROALE DE JOC ===\n\n";
        std::cout << "W - Miscare in sus\n";
        std::cout << "S - Miscare in jos\n";
        std::cout << "A - Miscare la stanga\n";
        std::cout << "D - Miscare la dreapta\n";
        std::cout << "F - Trage\n";
        std::cout << "E - Dodge\n";
        std::cout << "Q - Activeaza/dezactiveaza pauza\n";
        std::cout << "H - Afiseaza aceasta pagina de ajutor\n";
        std::cout << "X - Iesire din joc\n\n";
        std::cout << "Jocul are 20 de wave-uri de inamici, urmate de Azog, The Destroyer!\n";
        std::cout << "Colecteaza powerup-uri pentru a-ti imbunatati armele si abilitatile.\n\n";
        std::cout << "Apasa orice tasta pentru a incepe jocul...";
        std::cin.get();
    }

    void spawnWave() {
        try {
            if (wave < 1 || wave > 20) {
                throw InvalidWaveException(wave);
            }
            int nrInamici = 5 + wave;
            if (dificultate == 1) {
                nrInamici = 3 + wave / 2;
            } 
            else if (dificultate == 3) {
                nrInamici = 7 + wave;
            }
            if (nrInamici > 50) {
                throw GameStateException("Prea multi inamici: " + std::to_string(nrInamici), "Wave Spawning");
            }
            
            for (int i = 0; i < nrInamici; ++i) {
                try {
                    TipInamic tip = static_cast<TipInamic>(rand() % 3);
                    float x = rand() % 100;
                    float y = 10 + rand() % 30;
                    int viataInamic = 3;
                    if (wave > 10) viataInamic += (wave - 10) / 2;
                    if (dificultate == 3) viataInamic += 1;
                    auto inamic = std::make_unique<Inamic>(x, y, tip, viataInamic);
                    if (!inamic) {
                        throw MemoryException("Crearea inamicului");
                    }
                    inamici.push_back(std::move(inamic));
                    
                } 
                catch (const MemoryException& e) {
                    ExceptionHandler::handleException(e);
                    break;
                }
            }
            
            adaugaMesaj("Wave " + std::to_string(wave) + " a inceput cu " + 
                       std::to_string(inamici.size()) + " inamici!");
            
        } 
        catch (const InvalidWaveException& e) {
            ExceptionHandler::handleException(e);
            wave = std::max(1, std::min(wave, 20)); 
            ExceptionHandler::logRecoveryAction(" " + std::to_string(wave));
        } 
        catch (const GameStateException& e) {
            ExceptionHandler::handleException(e);
            ExceptionHandler::logRecoveryAction("S-a redus spawn count-ul");
        }
    }

    void spawnBoss() {
        //boss ul are mai multa viata in functie de dificultate
        boss = std::make_unique<Boss>(50, 10);
        
        adaugaMesaj("!!! BOSS FINAL APARUT !!!");
    }

    void spawnPowerup(float x, float y) {
        std::vector<std::string> tipuri = {"Viata", "RapidFire", "Scut", "Shotgun", "Bazooka"};
        //cand dificultatea e mai mica, sansa de a primi powerup este mai mare
        if (dificultate == 1 && rand() % 3 == 0) {
            tipuri = {"Viata", "Viata", "Scut", "RapidFire", "Shotgun"};
        }
        std::string ales = tipuri[rand() % tipuri.size()];
        powerups.emplace_back(x, y, ales);
        adaugaMesaj("Un powerup " + ales + " a aparut!");
    }

    void inputPlayer() {
        try {
            if (!modPauza) {
                std::cout << "\nComanda (W/A/S/D/F/E/Q/H/X): ";
                std::cout.flush();
                char comanda;
                if (!(std::cin >> comanda)) {
                    throw InputException("Failed to read input", "Command Input");
                }
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                ultimaComanda = std::tolower(comanda);
                SAFE_EXECUTE({
                    switch (ultimaComanda) {
                        case 'w': jucator.muta(0, -10); break;
                        case 's': jucator.muta(0, 10); break;
                        case 'a': jucator.muta(-10, 0); break;
                        case 'd': jucator.muta(10, 0); break;
                        case 'e': jucator.dodge(); break;
                        case 'f':
                            if (jucator.poateTrage()) {
                                auto gl = jucator.creeazaProiectile();
                                proiectileJucator.insert(proiectileJucator.end(), gl.begin(), gl.end());
                                statistici["gloanteTrase"] += gl.size();
                            }
                            break;
                        case 'q':
                            modPauza = !modPauza;
                            adaugaMesaj(modPauza ? "Joc în pauza" : "Jocul continua!");
                            break;
                        case 'h':
                            afisareControale();
                            break;
                        case 'x':
                            jocActiv = false;
                            adaugaMesaj("Iesire din joc...");
                            break;
                        default:
                            throw InvalidCommandException(ultimaComanda);
                    }
                }, "Se proceseaza comanda jucatorului..");
                
            } 
            else {
                std::cout << "\nJoc in pauza. Apasa Q pentru a continua: ";
                char cmd;
                std::cin >> cmd;
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                if (std::tolower(cmd) == 'q') {
                    modPauza = false;
                    adaugaMesaj("Jocul continua!");
                }
            }
            
        } 
        catch (const InvalidCommandException& e) {
            ExceptionHandler::handleException(e);
            ExceptionHandler::logRecoveryAction("Se ignora comanda nevalida");
        } 
        catch (const InputException& e) {
            ExceptionHandler::handleException(e);
            ExceptionHandler::logRecoveryAction(" ");
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        }
    }

    void actualizeaza() {
        if (modPauza) return;
        jucator.actualiz(jucator.getPozitie());
        tura++;
        turaCurenta++;
        for (auto& p : proiectileJucator)
            p.actualizeaza();
        for (auto& p : proiectileInamici)
            p.actualizeaza();
        for (auto& i : inamici)
            i->actualiz(jucator.getPozitie());
        if (boss)
            boss->actualiz(jucator.getPozitie());
        //verifica colectarea powerupurilor
        for (auto it = powerups.begin(); it != powerups.end();) {
            if (it->verificaColectare(jucator)) {
                statistici["powerupuriColectate"]++;
                adaugaMesaj("Ai colectat un powerup!");
                it = powerups.erase(it);
            } else {
                ++it;
            }
        }
        gestioneazaInteractiuni();
        gestioneazaProiectile();
        if (inamici.empty() && !boss) {
            wave++;
            if (wave <= 20) {
                spawnWave();
            } else {
                spawnBoss();
            }
        }
        //genereaza powerup ocazional
        if (rand() % 100 < (dificultate == 1 ? 5 : (dificultate == 2 ? 3 : 2))) {
            float x = 10 + rand() % 80;
            float y = 10 + rand() % 80;
            spawnPowerup(x, y);
        }
    }
    void gestioneazaInteractiuni() {
        for (auto& i : inamici) {
            i->interactCu(jucator);
            jucator.interactCu(*i);
            for (auto& j : inamici) {
                if (i != j) {
                    i->interactCu(*j);
                }
            }
        }
        if (boss) {
            boss->interactCu(jucator);
            jucator.interactCu(*boss);
            for (auto& i : inamici) {
                boss->interactCu(*i);
                i->interactCu(*boss);
            }
        }
    }
    void gestioneazaProiectile() {
        for (const auto& p : proiectileJucator) {
            for (auto& i : inamici) {
                if (!i->esteMort() && p.getPozitie().distanta(i->getPozitie()) < 5.0f) {
                    i->primesteDamage(p.esteExploziv() ? 3 : 1);
                    statistici["damageProvocat"] += (p.esteExploziv() ? 3 : 1);
                    jucator.scorPlus(1);
                    scor++;
                    if (i->esteMort()) {
                        statistici["inamiciUcisi"]++;
                        int sansaPowerup = dificultate == 1 ? 25 : (dificultate == 2 ? 15 : 10);
                        if (rand() % 100 < sansaPowerup) {
                            spawnPowerup(i->getPozitie().getX(), i->getPozitie().getY());
                        }
                    }
                }
            }
            if (boss && p.getPozitie().distanta(boss->getPozitie()) < 5.0f) {
                int damage = p.esteExploziv() ? 5 : 2;
                boss->primesteDamage(damage);
                statistici["damageProvocat"] += damage;
                scor += 2;
                if (!boss->esteMort()) {
                    int procentViata = boss->getViata();
                    if (procentViata < 30) {
                        adaugaMesaj("Boss aproape invins! Continua!");
                    }
                }
            }
        }
        proiectileJucator.erase(std::remove_if(proiectileJucator.begin(), proiectileJucator.end(), [](const Proiectil& p) { 
                return p.getPozitie().getX() < 0 || p.getPozitie().getX() > 100 || p.getPozitie().getY() < 0 || p.getPozitie().getY() > 100; }), proiectileJucator.end());
        proiectileInamici.erase(std::remove_if(proiectileInamici.begin(), proiectileInamici.end(), [](const Proiectil& p) { 
            return p.getPozitie().getX() < 0 || p.getPozitie().getX() > 100 || p.getPozitie().getY() < 0 || p.getPozitie().getY() > 100; }), proiectileInamici.end()); 
        for (const auto& p : proiectileInamici) {
            if (p.getPozitie().distanta(jucator.getPozitie()) < 5.0f) {
                int damage = p.esteExploziv() ? 2 : 1;
                jucator.primesteDamage(damage);
                statistici["damagePrimit"] += damage;
            }
        }
        for (auto& i : inamici) {
            if (Inamic* inamic = dynamic_cast<Inamic*>(i.get())) {
                if (inamic->poateTrage()) {
                    auto p = inamic->trageLaJucator(jucator.getPozitie());
                    proiectileInamici.push_back(p);
                }
            }
        }
        if (boss) {
            if (boss->poateTrage()) {
                auto p = boss->tragePattern();
                proiectileInamici.insert(proiectileInamici.end(), p.begin(), p.end());
            }
            if (boss->poateAtacSpecial()) {
                int k = rand() % 2;
                if (k == 0) {
                    adaugaMesaj("BOSS folosește atacul spirala!");
                    auto p = boss->atacSpirala(tura);
                    proiectileInamici.insert(proiectileInamici.end(), p.begin(), p.end());
                }
                else {
                    adaugaMesaj("BOSS folosește explozia!");
                    auto p = boss->atacExplozie();
                    proiectileInamici.insert(proiectileInamici.end(), p.begin(), p.end());
                }
            }
        }
        //elimina inamicii morti
        size_t inamiciInitial = inamici.size();
        inamici.erase(std::remove_if(inamici.begin(), inamici.end(), 
            [](const std::unique_ptr<Entitate>& i) { 
                return i->esteMort(); 
            }), inamici.end());
        if (inamici.size() < inamiciInitial) {
            statistici["inamiciUcisi"] += (inamiciInitial - inamici.size());
        }
    }
    
    void adaugaMesaj(const std::string& mesaj) {
        mesajeEvenimente.push_back(mesaj);
        if (mesajeEvenimente.size() > 5) {
            mesajeEvenimente.erase(mesajeEvenimente.begin());
        }
    }
    
    void desenareHUD() {
        std::string barViata = "[";
        for (int i = 0; i < 10; i++) {
            if (i < jucator.getViata()) {
                barViata += "♥";
            } else {
                barViata += " ";
            }
        }
        barViata += "]";
        
        std::cout << "Valul: " << wave << "/20";
        if (boss) std::cout << " [BOSS FIGHT]";
        std::cout << "Pozitie: " << jucator.getPozitie();
        std::cout << "\n=== EVENIMENTE RECENTE ===\n";
        for (const auto& mesaj : mesajeEvenimente) {
            std::cout << "- " << mesaj << std::endl;
        }
        
        std::cout << "\n";
    }
    
    void deseneazaHarta() {
        char harta[10][20] = {' '};
        for (int y = 0; y < 10; y++) {
            for (int x = 0; x < 20; x++) {
                harta[y][x] = ' ';
            }
        }
        
        int px = static_cast<int>(jucator.getPozitie().getX() / 5);
        int py = static_cast<int>(jucator.getPozitie().getY() / 10);
        px = std::min(19, std::max(0, px));
        py = std::min(9, std::max(0, py));
        harta[py][px] = 'P';
        for (const auto& i : inamici) {
            int ix = static_cast<int>(i->getPozitie().getX() / 5);
            int iy = static_cast<int>(i->getPozitie().getY() / 10);
            ix = std::min(19, std::max(0, ix));
            iy = std::min(9, std::max(0, iy));
            
            Inamic* inamic = dynamic_cast<Inamic*>(i.get());
            if (inamic) {
                if (harta[iy][ix] != 'P') {
                    harta[iy][ix] = 'E';
                }
            }
        }
        if (boss) {
            int bx = static_cast<int>(boss->getPozitie().getX() / 5);
            int by = static_cast<int>(boss->getPozitie().getY() / 10);
            bx = std::min(19, std::max(0, bx));
            by = std::min(9, std::max(0, by));
            for (int y = by - 1; y <= by + 1; y++) {
                for (int x = bx - 1; x <= bx + 1; x++) {
                    if (x >= 0 && x < 20 && y >= 0 && y < 10 && harta[y][x] != 'P') {
                        harta[y][x] = 'B';
                    }
                }
            }
        }
        for (const auto& p : proiectileJucator) {
            int px = static_cast<int>(p.getPozitie().getX() / 5);
            int py = static_cast<int>(p.getPozitie().getY() / 10);
            if (px >= 0 && px < 20 && py >= 0 && py < 10) {
                if (harta[py][px] == ' ') {
                    harta[py][px] = '^';
                }
            }
        }
        for (const auto& p : proiectileInamici) {
            int px = static_cast<int>(p.getPozitie().getX() / 5);
            int py = static_cast<int>(p.getPozitie().getY() / 10);
            if (px >= 0 && px < 20 && py >= 0 && py < 10) {
                if (harta[py][px] == ' ') {
                    harta[py][px] = '*';
                }
            }
        }
        for (const auto& p : powerups) {
            int px = static_cast<int>(p.getPozitie().getX() / 5);
            int py = static_cast<int>(p.getPozitie().getY() / 10);
            if (px >= 0 && px < 20 && py >= 0 && py < 10) {
                if (harta[py][px] == ' ') {
                    harta[py][px] = '?';
                }
            }
        }
        std::cout << "+--------------------+\n";
        for (int y = 0; y < 10; y++) {
            std::cout << "|";
            for (int x = 0; x < 20; x++) {
                char c = harta[y][x];
                if (c == 'P') std::cout << "P";
                else if (c == 'E') std::cout << "E";
                else if (c == 'B') std::cout << "B";
                else if (c == '^') std::cout << "^";
                else if (c == '*') std::cout << "*";
                else if (c == '?') std::cout << "?";
                else std::cout << " ";
            }
            std::cout << "|\n";
        }
        std::cout << "+--------------------+\n";
        std::cout << "Legenda: P = Jucator, E = Inamic, B = Boss, ^ = Gloante jucator, * = Gloante inamici, ? = Powerup\n";
    }
    
    void afisareStatistici() {
        std::cout << "\n=== STATISTICI DE JOC ===\n";
        std::cout << "Inamici eliminati: " << statistici["inamiciUcisi"] << std::endl;
        std::cout << "Gloante trase: " << statistici["gloanteTrase"] << std::endl;
        std::cout << "Powerup-uri colectate: " << statistici["powerupuriColectate"] << std::endl;
        std::cout << "Damage primit: " << statistici["damagePrimit"] << std::endl;
        std::cout << "Damage provocat: " << statistici["damageProvocat"] << std::endl;
        std::cout << "Scor total: " << scor << std::endl;
    }
    
    void afisareControaleRapide() {
        std::cout << "Controale: [W,A,S,D] = miscare, [F] = tragere, [E] = dodge, [Q] = pauza, [H] = ajutor, [X] = iesire\n";
    }
    
    void afisareEcranJoc() {
        clearScreen();
        desenareHUD();
        std::cout << "\n";
        deseneazaHarta();
        std::cout << "\n";
        afisareControaleRapide();
    }
    
    void afisareFinal() {
        clearScreen();
        std::cout << "==================================\n";
        std::cout << "          JOCUL S-A INCHEIAT      \n";
        std::cout << "==================================\n\n";
        if (jucator.eViu() && boss && boss->esteMort()) {
            std::cout << "FELICITARI! L-ai invins pe Azog, Lord of Darknes și ai salvat lumea!\n\n";
        } else {
            std::cout << "YOU DIED!\n\n";
        }
        std::cout << "Ai ajuns pana la valul: " << wave << "/20\n";
        std::cout << "Scor final: " << scor << "\n\n";
        afisareStatistici();
        std::cout << "Apasa orice tasta pentru a iesi...";
        std::cin.get();
    }
    
    void ruleaza() {
        try {
            while (jocActiv && !criticalErrorOccurred) {
                SAFE_EXECUTE({
                    inputPlayer();
                    if (!modPauza) 
                        actualizeaza();
                    afisareEcranJoc();
                    if (!jucator.eViu()) {
                        adaugaMesaj("AI PIERDUT... GAME OVER");
                        jocActiv = false;
                    }
                    if (boss && boss->esteMort()) {
                        adaugaMesaj("FELICITARI! L-AI INVINS PE AZOG!");
                        jocActiv = false;
                    }
                    asteptare(delayActiune);
                }, "Main game loop");
            }
            
        } 
        catch (const CriticalGameException& e) {
            criticalErrorOccurred = true;
            ExceptionHandler::handleException(e);
            std::cout << "\nA survenit o eroare critica. Jocul se va inchide..." << std::endl;
        } 
        catch (...) {
            criticalErrorOccurred = true;
            ExceptionHandler::handleUnknownException();
            std::cout << "\nEroare necunoscuta. Jocul se va inchide..." << std::endl;
        }
        afisareFinal();
    }
    
    ~Run() {}
};
#endif