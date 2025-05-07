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

enum TipInamic { RANDOM, CHASER, SNIPER };

enum TipArma { NORMAL, SHOTGUN, BAZOOKA};

class Pozitie{ //reprezinta un reper cartezian xoy pe care il vom folosi pentru a misca entitatile pe harta
private:
    float x, y;
public:
    explicit Pozitie(float x = 0, float y = 0) : x(x), y(y) {}
    float getX() const { return x; }
    float getY() const { return y; }
    void miscari(float dx, float dy, float maxX = 100, float maxY = 100) {//misca o entitate fara ca aceasta sa iasa din bounds
        x = std::max(0.0f, std::min(x + dx, maxX));
        y = std::max(0.0f, std::min(y + dy, maxY));
    }
    float distanta(const Pozitie& other) const {//calculeaza distanta dintre doua entitati
        return std::sqrt((x - other.x)*(x - other.x) + (y - other.y)*(y - other.y));
    }
    friend std::ostream& operator<<(std::ostream& os, const Pozitie& v) {
        os << "(" << v.x << ", " << v.y << ")";
        return os;
    }
};  

class Proiectil {
    private:
        Pozitie pozitie;
        float dx, dy;
        bool exploziv;
    public:
        Proiectil(float x, float y, float dx = 0, float dy = -1.0f, bool exploziv = false) : pozitie(x, y), dx(dx), dy(dy), exploziv(exploziv) {}
        Proiectil(const Proiectil& other) : pozitie(other.pozitie), dx(other.dx), dy(other.dy), exploziv(other.exploziv) {
            std::cout << "[Proiectil copiat]\n";
        }
        Proiectil& operator=(const Proiectil& other){
            if (this != &other){
                pozitie = other.pozitie;
                dx = other.dx;
                dy = other.dy;
                exploziv = other.exploziv;
            }
            std::cout << "[Proiectil asignat]\n";
            return *this;
        }
        void actualizeaza() {
            pozitie.miscari(dx * 2.5f, dy * 2.5f);
        }
        Pozitie getPozitie() const {
            return pozitie;
        }
        bool esteExploziv() const { return exploziv; }
        friend std::ostream& operator<<(std::ostream& os, const Proiectil& p) {
            os << "Proiectil la " << p.pozitie;
        return os;
    }
    ~Proiectil(){
    }
};

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

class Powerup {
private:
    Pozitie pozitie;
    std::string tip;//"Viata", "RapidFire", "Scut", "MultiShot", "Bazooka", "Shotgun"
public:
    Powerup(float x, float y, const std::string& tip) : pozitie(x, y), tip(tip) {}
    const Pozitie& getPozitie() const { return pozitie; }
    bool verificaColectare(Jucator& jucator) {
        if (pozitie.distanta(jucator.getPozitie()) < 2.0f) {
            jucator.adaugaItem(tip);
            return true;
        }
        return false;
    }
    friend std::ostream& operator<<(std::ostream& os, const Powerup& p) {
        os << "Powerup [" << p.tip << "] la " << p.pozitie;
        return os;
    }
    ~Powerup(){
    }
};
    
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
    
          

int main() {
    Run run;
    run.ruleaza();
    ResourceManager& res = ResourceManager::Instance();
    sf::Texture& backgroundTexture = res.getTexture("airplane.png");
    sf::Font font("arial.ttf");
    sf::Sprite background(backgroundTexture); 
    return 0;
}