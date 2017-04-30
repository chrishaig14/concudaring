#include <vector>
#include <iostream>
#include <sstream>
#include "semaphore/Semaphore.h"
#include "constantes.h"
#include "shmem/MemoriaCompartida.h"
#include "shared_stack/SharedStack.h"

int main(int argc, char *argv[]) {
    int playerNum = atoi(argv[1]); // numero de jugador
    int cartaAnterior = -1;

    Semaphore semJugar("/bin/bash", SEM_JUGAR, 0);
    Semaphore semTurnoTerminado("/bin/bash", SEM_TURNO, 0);

    MemoriaCompartida<int> numJugador("/bin/bash", SHM_PLAYER_NUM);
    MemoriaCompartida<bool> hayGanador(SHMEM_PATH, SHM_WINNER);
    MemoriaCompartida<int> logLevel(SHMEM_PATH, SHM_LOG);
    Log::instance()->loggerLevel = logLevel.leer() ? Log::ERROR : Log::DEBUG;

    SharedStack centralCards("/bin/bash", SHM_CARDS, NUM_CARDS);

    std::ostringstream player;
    player << "[" << playerNum << "]:: ";

    while (true) {
        semJugar.p(1); // hago lo mio segun la carta
        if (hayGanador.leer()){
            return 0; // Si hay un ganador, termino mi ejecucion
        }
        std::ostringstream s;
        s << player.str();
        int tope = centralCards.top();
        if (tope == cartaAnterior || tope == 7) {
            s << "Pongo mano sobre la pila de cartas";
            numJugador.escribir(playerNum + 1);
        } else {
            if (tope == 10) {
                s << "Buenos dias señorita";
            } else if (tope == 11) {
                s << "Buenas noche caballero";
            } else if (tope == 12) {
                s << "Saludo militar";
            } else {
                s << "No hago nada";
            }
        }
        Log::instance()->append(s.str(), Log::DEBUG);
        cartaAnterior = tope;
        semTurnoTerminado.v(1);
    }
}
