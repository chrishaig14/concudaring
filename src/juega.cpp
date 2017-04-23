//
// Created by chris on 12/04/17.
//

#include <vector>
#include "semaphore/Semaphore.h"
#include "constantes.h"
#include "shmem/MemoriaCompartida.h"
#include "shared_stack/SharedStack.h"

int main(int argc, char *argv[]) {
    int num_players = atoi(argv[0]); // cantidad total de jugadores
    int player_num = atoi(argv[1]); // numero de jugador
    Semaphore sem_jugar("/bin/bash", SEM_JUGAR + player_num, 0);
    Semaphore sem_turno_terminado("/bin/bash", SEM_TURNO, 0);
    MemoriaCompartida<int> numero_jugador("/bin/bash", SHM_PLAYER_NUM);
    SharedStack centralCards("/bin/bash", SHM_CARDS, NUM_CARDS);
    SharedStack myCards("/bin/bash", SHM_CARDS + player_num + 1, NUM_CARDS);
    int previous_card = -1;

    while (true) {
        sem_jugar.p(1); // hago lo mio segun la carta
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
