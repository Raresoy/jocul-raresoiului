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
            
int main() {
    return 0;
}


