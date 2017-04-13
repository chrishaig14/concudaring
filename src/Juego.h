#ifndef CONCUDARING_GAME_H
#define CONCUDARING_GAME_H

#include <vector>
#include "shmem/MemoriaCompartidaArray.h"
#include "SharedStack.h"

class Juego {
public:
    Juego(int cantJugadores);
    ~ Juego();
    int correr();

private:
    int cantJugadores;

    void repartir_cartas(std::vector<SharedStack> &cartasJugadores);
};


#endif //CONCUDARING_GAME_H
