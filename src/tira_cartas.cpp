#include <vector>
#include "semaphore/Semaphore.h"
#include "constantes.h"
#include "shared_stack/SharedStack.h"
#include "shmem/MemoriaCompartida.h"

#define NEXT_PLAYER(player_num) (player_num + 1)%num_players

int main(int argc, char *argv[]) {
    int num_players = atoi(argv[0]);
    int player_num = atoi(argv[1]);

    if (fork() == 0) {
        execv("juega", argv);
    }

    std::vector<Semaphore> sem_player;
    for (int i = 0; i < num_players; i++) {
        sem_player.push_back(Semaphore("/bin/bash", (char) (1 + i), 0));
    }
    Semaphore sem_turno_terminado("/bin/bash", SEM_TURNO, 0);

    SharedStack centralCards(SHMEM_PATH, SHM_CARDS, NUM_CARDS);
    SharedStack myCards(SHMEM_PATH, SHM_CARDS + 1 + player_num, NUM_CARDS);

    MemoriaCompartida<bool> hayGanador(SHMEM_PATH, SHM_WINNER);

    while (true) {
        std::cout << "[" << player_num << "]" << " Esperando mi turno " << std::endl;
        sem_player[player_num].p(1);

        if(hayGanador.leer()){
            // Habilito al proximo jugador para que termine su ejecucion
            sem_player[NEXT_PLAYER(player_num)].v(1);
            sem_turno_terminado.eliminar();
            return(0);
        }

        std::cout << "****************************************" << std::endl;
        std::cout << "[" << player_num << "]" << " Empieza mi turno " << std::endl;

        int myTopCard = myCards.pop();
        centralCards.push(myTopCard); // pongo la carta

        std::cout << "[" << player_num << "]" << " Pongo carta " << myTopCard << std::endl;

        std::cout << "Cartas centrales: ";
        centralCards.show();

        MemoriaCompartida<int> numero_jugador("/bin/bash", SHM_PLAYER_NUM);

        numero_jugador.escribir(0);


        sem_turno_terminado.inicializar(); //  0

        for (int i = 0; i < num_players; i++) {
            Semaphore sem_jugar("/bin/bash", SEM_JUGAR + i, 1);
            sem_jugar.v(1);
        }

        sem_turno_terminado.p(num_players);

        if (numero_jugador.leer() == 0) {
            // Ninguno se lleva las cartas
        } else {
            // Le doy las cartas al último que tocó numero_jugador
            SharedStack cards_player("/bin/bash", SHM_CARDS + numero_jugador.leer(), NUM_CARDS);
            std::cout << "* El jugador " << numero_jugador.leer() - 1 << " se lleva las cartas *" << std::endl;
            int n_cards = centralCards.size();
            for (int i = 0; i < n_cards; i++) {
                cards_player.push(centralCards.pop());
            }
            centralCards.clear();
        }
/*
        std::cout << "* Cartas *" << std::endl;
        for (int i = 0; i < num_players; i++) {
            SharedStack cards_player("/bin/bash", SHM_CARDS + i + 1, NUM_CARDS);
            std::cout << "[" << i << "]: ";
            cards_player.show();
            std::cout << std::endl;
        }
*/
        std::cout << "[" << player_num << "]" << " Termina mi turno." << std::endl;

        if (myCards.size() == 0) {
            std::cout << "El jugador " << player_num << " ganó." << std::endl;
            hayGanador.escribir(true);

            // Destrabo los procesos que realizan acciones para que puedan terminar
            for (int i = 0; i < num_players; i++) {
                Semaphore sem_jugar("/bin/bash", SEM_JUGAR + i, 1);
                sem_jugar.v(1);
            }
            break;
        }

        std::cout << "****************************************" << std::endl;

        sem_player[NEXT_PLAYER(player_num)].v(1);// habilito al proximo jugador para que tire su carta
    }
    sem_player[NEXT_PLAYER(player_num)].v(1);// habilito al proximo jugador para que termine su ejecucion
}