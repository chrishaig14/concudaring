#include <iostream>
#include <stdio.h>
#include <string.h>
#include "logger/Log.h"
#include "constantes.h"
#include "Juego.h"
#include "shmem/MemoriaCompartida.h"
#include <algorithm>

bool validarParametros(int argc, char **argv) {

    MemoriaCompartida<int> logLevel(SHMEM_PATH, SHM_LOG);
    bool validos = true;

    if(argc > 3 || argc < 2) {
        Log::instance()->Log::append("[-]:: " + std::string(TOO_MANY_ARGS), Log::ERROR);
        validos = false;
    }

    if(argc == 3 && strcmp(argv[2], "d") == 0){
        logLevel.escribir(Log::DEBUG);
        Log::instance()->loggerLevel = logLevel.leer() ? Log::ERROR : Log::DEBUG;
        Log::instance()->append("[-]:: " + std::string(DEBUG_MODE), Log::DEBUG);
    }

    if(atoi(argv[1]) == 0 || atoi(argv[1])%2 != 0) {
        Log::instance()->append("[-]:: " + std::string(INVALID_PLAYER_AMOUNT), Log::ERROR);
        validos = false;
    }
    return validos;
}

void ayuda() {
    std::cout << HELP << std::endl;
}

int main(int argc, char** argv) {
    MemoriaCompartida<int> logLevel(SHMEM_PATH, SHM_LOG);
    logLevel.escribir(Log::ERROR);
    Log::instance()->loggerLevel = Log::ERROR;
    Log::instance()->printNewLogger();
    if(!validarParametros(argc, argv)) {
        ayuda();
        return 0;
    }
    int numJugadores = atoi(argv[1]);

    std::ostringstream s;
    s << "[-]:: ";
    s << STARTING_WITH << numJugadores;
    Log::instance()->append(s.str(), Log::DEBUG);

    Juego* juego = new Juego(numJugadores);
    int resultado = juego->correr();

    if(resultado == 0){
        Log::instance()->append("[-]:: " + std::string(GAME_EXIT_OK), Log::DEBUG);
    } else {
        Log::instance()->append("[-]:: " + std::string(GAME_EXIT_ERROR), Log::ERROR);
    }
    return resultado;
}