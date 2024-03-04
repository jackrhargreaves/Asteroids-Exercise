#include "GameLoop.h"
#include <iostream>

int main() {
    try {
        GameLoop game;
        game.run();
    }
    catch (const std::exception& e) {
        std::cerr << "Exception: " << e.what() << std::endl;
        return 1;
    }
    return 0;
}
