#include <vector>
#include <iostream>
#include <sstream>
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
    MemoriaCompartida<int> logLevel(SHMEM_PATH, SHM_LOG);
    Log::instance()->loggerLevel = logLevel.leer() ? Log::ERROR : Log::DEBUG;

    while (true) {
        std::ostringstream s;
        std::ostringstream player;
        player << "[" << player_num << "]:: ";
        s << player.str() << "Esperando mi turno";
        Log::instance()->append(s.str(), Log::DEBUG);
        sem_player[player_num].p(1);

        if(hayGanador.leer()){
            // Habilito al proximo jugador para que termine su ejecucion
            sem_player[NEXT_PLAYER(player_num)].v(1);
            sem_turno_terminado.eliminar();
            return(0);
        }

        Log::instance()->append(SEPARATOR, Log::DEBUG);
        s.clear();
        s.str("");
        s << player.str() << "Empieza mi turno";
        Log::instance()->append(s.str(), Log::DEBUG);

        int myTopCard = myCards.pop();
        centralCards.push(myTopCard); // pongo la carta
        s.clear();
        s.str("");
        s << player.str() << "Pongo carta " << myTopCard;
        Log::instance()->append(s.str(), Log::DEBUG);

        s.clear();
        s.str("");
        s << player.str() << "Cartas centrales: ";
        Log::instance()->append(s.str() + centralCards.show(), Log::DEBUG);

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
            s.clear();
            s.str("");
            s << "[-]:: El jugador " << numero_jugador.leer() - 1 << " se lleva las cartas";
            Log::instance()->append(s.str(), Log::DEBUG);
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
        s.clear();
        s.str("");
        s << player.str() << "Termina mi turno";
        Log::instance()->append(s.str(), Log::DEBUG);

        if (myCards.size() == 0) {
            std::ostringstream sfin;
            sfin << "[-]:: El jugador " << player_num << " ganó";
            Log::instance()->append(sfin.str(), Log::DEBUG);
            hayGanador.escribir(true);

            // Destrabo los procesos que realizan acciones para que puedan terminar
            for (int i = 0; i < num_players; i++) {
                Semaphore sem_jugar("/bin/bash", SEM_JUGAR + i, 1);
                sem_jugar.v(1);
            }
            break;
        }

        Log::instance()->append(SEPARATOR, Log::DEBUG);

        sem_player[NEXT_PLAYER(player_num)].v(1);// habilito al proximo jugador para que tire su carta
    }
    sem_player[NEXT_PLAYER(player_num)].v(1);// habilito al proximo jugador para que termine su ejecucion
}