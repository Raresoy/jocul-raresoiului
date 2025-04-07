#include <iostream>
#include <vector>
#include <cmath>
#include <memory>
#include <fstream>
#include <sstream>

class Harta{ //reprezinta un reper cartezian xoy pe care il vom folosi pentru a misca entitatile pe harta
private:
    float x, y;
public:
    Harta(float x = 0, float y = 0) : x(x), y(y) {}
    float getX() const { return x; }
    float getY() const { return y; }
    void miscari(float dx, float dy, float maxX = 100, float maxY = 100) {//misca o entitate fara ca aceasta sa iasa din bounds
        x = std::max(0.0f, std::min(x + dx, maxX));
        y = std::max(0.0f, std::min(y + dy, maxY));
    }
    float distanta(const Harta& other) const {//calculeaza distanta dintre doua entitati
        return std::sqrt((x - other.x)*(x - other.x) + (y - other.y)*(y - other.y));
    }
    friend std::ostream& operator<<(std::ostream& os, const Harta& v) {
        os << "(" << v.x << ", " << v.y << ")";
        return os;
    }
};

class Jucator{
private:
    Harta pozitie;
    int viata;
    int cooldown;//timpul in care jucatorul e invulnerabil  
    int scor;
    std::vector<std::string> inventar;
public:
    Jucator(float x, float y, int viata = 5) : pozitie(x, y), viata(viata), cooldown(0), scor(0) {}
    void muta(float dx, float dy) {//se misca jucatorul pe harta
        pozitie.miscari(dx, dy);
    }
    void tick() {
        if (cooldown > 0) cooldown--;//scade durata de invulnerabilitate
    }
    void lovit(int damage = 1) {
        if (cooldown == 0 && viata > 0) {
            viata -= damage;
            cooldown = 3;
        }
    }
    void adaugaItem(const std::string& item) {//doua iteme de viata si invulnerabilitate
        inventar.push_back(item);
        if (item == "Viata") viata++;
        else if (item == "Scut") cooldown += 2;
    }
    void dodge() {
        cooldown = 2;
        std::cout << "[Jucatorul a efectuat o eschiva!]\n";
    }
    bool eViu() const {//self explanatory
        return viata > 0;
    }
    void scorPlus(int s) { scor += s; }
    const Harta& getPozitie() const {
        return pozitie;
    }
    friend std::ostream& operator<<(std::ostream& os, const Jucator& j) {
        os << "Jucator la " << j.pozitie << " | Viata: " << j.viata << " | Scor: " << j.scor << " | Inventar: [ ";
        for (auto& item : j.inventar) os << item << " ";
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
        } 
        else {
            std::cerr << "[Eroare: Nu s-a putut scrie fisierul jucator_final.txt]\n";
        }
        inventar.clear();
        std::cout << "[Jucator distrus: salvare finala completata.]\n";
    }
};
            
int main() {
    return 0;
}


