#ifndef CONCUDARING_GAME_H
#define CONCUDARING_GAME_H

#include <vector>
#include "shmem/MemoriaCompartidaArray.h"
#include "shared_stack/SharedStack.h"
#include "semaphore/Semaphore.h"

class Juego {
public:
    Juego(int cantJugadores);
    ~ Juego();
    int correr();

private:
    int cantJugadores;

    void repartirCartas(std::vector<SharedStack> &cartasJugadores);
    void limpiarSemaforos(std::vector<Semaphore> &sem_jugadores);
};


#endif //CONCUDARING_GAME_H
