#include <vector>
#include <iostream>
#include <sstream>
#include "constantes.h"
#include "shared_stack/SharedStack.h"
#include "shmem/MemoriaCompartida.h"

int main(int argc, char** argv) {
    int cantJugadores = atoi(argv[0]);
    std::vector<SharedStack> cartasJugadores;
    MemoriaCompartida<bool> hayGanador(SHMEM_PATH, SHM_WINNER);
    MemoriaCompartida<int> logLevel(SHMEM_PATH, SHM_LOG);
    Log::instance()->loggerLevel = logLevel.leer() ? Log::ERROR : Log::DEBUG;

    for (int i = 0; i < cantJugadores; i++) {
        cartasJugadores.push_back(SharedStack(SHMEM_PATH, SHM_CARDS + i + 1, NUM_CARDS)); // Pila de cada jugador
    }

    while(true) {
        if (hayGanador.leer()){
            return 0;
        }
        for(int i = 0; i < cantJugadores; i++) {
            std::ostringstream s;
            size_t cantCartas = cartasJugadores[i].size();

            s << "[R]:: " << "El jugador " << i << " tiene en su mano ";
            Log::instance()->append(s.str() + cartasJugadores[i].show(), Log::DEBUG);

        }
        usleep(1000); // cada 1000 usecs me fijo las cartas que tiene cada jugador
    }
}