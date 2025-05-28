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
#include "Run.h"
#include "GameException.h"
        

int main() {
    try {
        Run run;
        run.ruleaza();
    } 
    catch (const CriticalGameException& e) {
        e.logError();
        std::cout << "Eroare critica." << std::endl;
        return 1;
    } 
    catch (const GameException& e) {
        e.logError();
        std::cout << "Eroare." << std::endl;
        return 1;
    } 
    catch (...) {
        std::cout << "Eroare necunoscuta" << std::endl;
        return 1;
    }
    ResourceManager& res = ResourceManager::Instance();
    sf::Texture& backgroundTexture = res.getTexture("airplane.png");
    sf::Font& font = res.getFont("FiraSans-Regular.ttf");
    sf::Text text(font, "Hello world!", 24);
    sf::Sprite background(backgroundTexture); 
    return 0;
}