#include <vector>
#include <iostream>
#include <sstream>
#include "semaphore/Semaphore.h"
#include "constantes.h"
#include "shared_stack/SharedStack.h"
#include "shmem/MemoriaCompartida.h"

#define NEXT_PLAYER(playerNum) (playerNum + 1)%cantJugadores

int main(int argc, char *argv[]) {
    int cantJugadores = atoi(argv[0]);
    int playerNum = atoi(argv[1]);

    if (fork() == 0) {
        execv("juega", argv);
    }

    std::vector<Semaphore> sem_player;
    for (int i = 0; i < cantJugadores; i++) {
        sem_player.push_back(Semaphore("/bin/bash", (char) (1 + i), 0));
    }
    Semaphore semTurnoTerminado("/bin/bash", SEM_TURNO, 0);
    Semaphore semJugar("/bin/bash", SEM_JUGAR, 1);

    SharedStack centralCards(SHMEM_PATH, SHM_CARDS, NUM_CARDS);
    SharedStack myCards(SHMEM_PATH, SHM_CARDS + 1 + playerNum, NUM_CARDS);

    MemoriaCompartida<bool> hayGanador(SHMEM_PATH, SHM_WINNER);
    MemoriaCompartida<int> logLevel(SHMEM_PATH, SHM_LOG);
    MemoriaCompartida<int> ultimoJugador("/bin/bash", SHM_PLAYER_NUM);

    Log::instance()->loggerLevel = logLevel.leer() ? Log::ERROR : Log::DEBUG;

    while (true) {
        std::ostringstream s;
        std::ostringstream player;
        player << "[" << playerNum << "]:: ";
        s << player.str() << "Esperando mi turno";
        Log::instance()->append(s.str(), Log::DEBUG);
        sem_player[playerNum].p(1);

        if(hayGanador.leer()){
            // Habilito al proximo jugador para que termine su ejecucion
            sem_player[NEXT_PLAYER(playerNum)].v(1);
            semTurnoTerminado.eliminar();
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

        ultimoJugador.escribir(0);

        semTurnoTerminado.inicializar(); //  0

        semJugar.v(cantJugadores);

        semTurnoTerminado.p(cantJugadores);

        if (ultimoJugador.leer() == 0) {
            // Ninguno se lleva las cartas
        } else {
            // Le doy las cartas al último que tocó ultimoJugador
            SharedStack cards_player("/bin/bash", SHM_CARDS + ultimoJugador.leer(), NUM_CARDS);
            s.clear();
            s.str("");
            s << "[-]:: El jugador " << ultimoJugador.leer() - 1 << " se lleva las cartas";
            Log::instance()->append(s.str(), Log::DEBUG);
            int n_cards = centralCards.size();
            for (int i = 0; i < n_cards; i++) {
                cards_player.push(centralCards.pop());
            }
            centralCards.clear();
        }

        s.clear();
        s.str("");
        s << player.str() << "Termina mi turno";
        Log::instance()->append(s.str(), Log::DEBUG);

        if (myCards.size() == 0) {
            std::ostringstream sfin;
            sfin << "[-]:: El jugador " << playerNum << " ganó";
            Log::instance()->append(sfin.str(), Log::DEBUG);
            hayGanador.escribir(true);

            // Destrabo los procesos que realizan acciones para que puedan terminar
            for (int i = 0; i < cantJugadores; i++) {
                Semaphore semJugar("/bin/bash", SEM_JUGAR + i, 1);
                semJugar.v(1);
            }
            break;
        }

        Log::instance()->append(SEPARATOR, Log::DEBUG);

        sem_player[NEXT_PLAYER(playerNum)].v(1);// habilito al proximo jugador para que tire su carta
    }
    sem_player[NEXT_PLAYER(playerNum)].v(1);// habilito al proximo jugador para que termine su ejecucion
}