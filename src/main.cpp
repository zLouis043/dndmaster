#include "core/engine.h"
#include <iostream>

int main(int argc, char* argv[]) {
    Engine engine;

    if (!engine.init()) {
        std::cerr << "Impossibile avviare il motore grafico.\n";
        return -1;
    }

    engine.run();

    return 0;
}