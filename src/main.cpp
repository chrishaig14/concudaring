#include <iostream>
#include <stdio.h>
#include <string.h>
#include "logger/Log.h"
#include "constantes.h"
#include "Juego.h"
#include <algorithm>

bool validarParametros(int argc, char **argv) {

    bool validos = true;

    if(argc > 3) {
        Log::instance()->Log::append(TOO_MANY_ARGS, Log::ERROR);
        validos = false;
    }

    if(argc == 3 && strcmp(argv[2], "d") == 0){
        Log::instance()->loggerLevel = Log::DEBUG;
        Log::instance()->append(DEBUG_MODE, Log::DEBUG);
    }

    if(atoi(argv[1]) == 0 || atoi(argv[1])%2 != 0) {
        Log::instance()->append(INVALID_PLAYER_AMOUNT, Log::ERROR);
        validos = false;
    }
    return validos;
}

void ayuda() {
    std::cout << HELP << std::endl;
}

int main(int argc, char** argv) {

    std::cout << "MI PID: " << getpid() << std::endl;

    Log::instance()->loggerLevel = Log::ERROR;
    if(!validarParametros(argc, argv)) {
        ayuda();
        return 0;
    }
    int numJugadores = atoi(argv[1]);
    Log::instance()->append(
        std::string(STARTING_WITH) + std::to_string(numJugadores),
        Log::DEBUG);

    Juego* juego = new Juego(numJugadores);
    int resultado = juego->correr();

    if(resultado == 0){
        Log::instance()->append(GAME_EXIT_OK, Log::DEBUG);
    } else {
        Log::instance()->append(GAME_EXIT_ERROR, Log::ERROR);
    }
    return resultado;
}