#include <vector>
#include "semaphore/Semaphore.h"
#include "constantes.h"
#include "shmem/MemoriaCompartida.h"
#include "shared_stack/SharedStack.h"

int main(int argc, char *argv[]) {
    int player_num = atoi(argv[1]); // numero de jugador
    int previous_card = -1;

    Semaphore sem_jugar("/bin/bash", SEM_JUGAR + player_num, 0);
    Semaphore sem_turno_terminado("/bin/bash", SEM_TURNO, 0);

    MemoriaCompartida<int> numero_jugador("/bin/bash", SHM_PLAYER_NUM);
    MemoriaCompartida<bool> hayGanador(SHMEM_PATH, SHM_WINNER);
    MemoriaCompartida<int> logLevel(SHMEM_PATH, SHM_LOG);
    Log::instance()->loggerLevel = logLevel.leer() ? Log::ERROR : Log::DEBUG;

    SharedStack centralCards("/bin/bash", SHM_CARDS, NUM_CARDS);
    SharedStack myCards("/bin/bash", SHM_CARDS + player_num + 1, NUM_CARDS);

    while (true) {
        sem_jugar.p(1); // hago lo mio segun la carta
        if (hayGanador.leer()){
            return 0; // Si hay un ganador, termino mi ejecucion
        }
        int topCard = centralCards.top();
        if (topCard == previous_card || topCard == 7) {
            std::cout << "[" << player_num << "]" << " Pongo mano sobre la pila de cartas" << std::endl;
            numero_jugador.escribir(player_num + 1);
        } else {
            if (topCard == 10) {
                std::cout << "[" << player_num << "]" << " Buenos dias señorita" << std::endl;
            } else if (topCard == 11) {
                std::cout << "[" << player_num << "]" << " Buenas noche caballero" << std::endl;
            } else if (topCard == 12) {
                std::cout << "[" << player_num << "]" << " Saludo militar" << std::endl;
            } else {
                std::cout << "[" << player_num << "]" << " No hago nada" << std::endl;
            }
        }
        previous_card = topCard;
        sem_turno_terminado.v(1);
    }
}
