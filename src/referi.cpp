#include <vector>
#include "semaphore/Semaphore.h"
#include "constantes.h"
#include "shared_stack/SharedStack.h"
#include "shmem/MemoriaCompartida.h"

int main(int argc, char** argv) {
    int cantJugadores = atoi(argv[0]);
    std::vector<SharedStack> cartasJugadores;

    for (int i = 0; i < cantJugadores; i++) {
        cartasJugadores.push_back(SharedStack(SHMEM_PATH, SHM_CARDS + i + 1, NUM_CARDS)); // Pila de cada jugador
    }

    while(true) {
        sleep(5); // cada 5 segundos me fijo la cantidad de cartas que tiene cada jugador
        for(int i = 0; i < cantJugadores; i++) {
            size_t cantCartas = cartasJugadores[i].size();
            std::cout << "[REF] " << "El jugador: " << i << " tiene " << cantCartas << " cartas en su mano." << std::endl;
        }
    }
}