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
        

int main() {
    Run run;
    run.ruleaza();
    ResourceManager& res = ResourceManager::Instance();
    sf::Texture& backgroundTexture = res.getTexture("airplane.png");
    sf::Font& font = res.getFont("FiraSans-Regular.ttf");
    sf::Text text(font, "Hello world!", 24);
    sf::Sprite background(backgroundTexture); 
    return 0;
}