#include <vector>
#include <iostream>
#include <sstream>
#include "semaphore/Semaphore.h"
#include "constantes.h"
#include "shmem/MemoriaCompartida.h"
#include "shared_stack/SharedStack.h"

int main(int argc, char *argv[]) {
    int player_num = atoi(argv[1]); // numero de jugador
    int previous_card = -1;

    Semaphore sem_jugar("/bin/bash", SEM_JUGAR, 0);
    Semaphore sem_turno_terminado("/bin/bash", SEM_TURNO, 0);

    MemoriaCompartida<int> numJugador("/bin/bash", SHM_PLAYER_NUM);
    MemoriaCompartida<bool> hayGanador(SHMEM_PATH, SHM_WINNER);
    MemoriaCompartida<int> logLevel(SHMEM_PATH, SHM_LOG);
    Log::instance()->loggerLevel = logLevel.leer() ? Log::ERROR : Log::DEBUG;

    SharedStack centralCards("/bin/bash", SHM_CARDS, NUM_CARDS);

    std::ostringstream player;
    player << "[" << player_num << "]:: ";

    while (true) {
        sem_jugar.p(1); // hago lo mio segun la carta
        std::ostringstream s;
        s << player.str();
        if (hayGanador.leer()){
            return 0; // Si hay un ganador, termino mi ejecucion
        }
        int topCard = centralCards.top();
        if (topCard == previous_card || topCard == 7) {
            s << "Pongo mano sobre la pila de cartas";
            numJugador.escribir(player_num + 1);
        } else {
            if (topCard == 10) {
                s << "Buenos dias señorita";
            } else if (topCard == 11) {
                s << "Buenas noche caballero";
            } else if (topCard == 12) {
                s << "Saludo militar";
            } else {
                s << "No hago nada";
            }
        }
        Log::instance()->append(s.str(), Log::DEBUG);
        previous_card = topCard;
        sem_turno_terminado.v(1);
    }
}
