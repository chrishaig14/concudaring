#include <vector>
#include <iostream>
#include <sstream>
#include "semaphore/Semaphore.h"
#include "constantes.h"
#include "shared_stack/SharedStack.h"
#include "shmem/MemoriaCompartida.h"
#include <sys/wait.h>

#define NEXT_PLAYER(playerNum) (playerNum + 1)%cantJugadores

int main(int argc, char *argv[]) {
    int cantJugadores = atoi(argv[0]);
    int playerNum = atoi(argv[1]);

    if (fork() == 0) {
        execv("realiza_accion", argv);
    }

    std::vector<Semaphore> semTurnoJugador;
    for (int i = 0; i < cantJugadores; i++) {
        semTurnoJugador.push_back(Semaphore("/bin/bash", SEM_TURNO_JUGADOR + i, 0));
    }
    Semaphore semTurnoTerminado("/bin/bash", SEM_TURNO_TERMINADO, 0);
    Semaphore semJugar("/bin/bash", SEM_ACCIONES, 0);

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
        semTurnoJugador[playerNum].p(1);

        if(hayGanador.leer()){
            // Habilito al proximo jugador para que termine su ejecucion
            semTurnoJugador[NEXT_PLAYER(playerNum)].v(1);
            //std::cout << "fin tira_cartas " << playerNum << std::endl;
            wait(NULL);
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

        semJugar.inicializar();

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
                Semaphore s("/bin/bash", SEM_JUGADOR + i, 0);
                s.v(cantJugadores);
            }

            semJugar.v(cantJugadores);
            break;
        }

        Log::instance()->append(SEPARATOR, Log::DEBUG);

        semTurnoJugador[NEXT_PLAYER(playerNum)].v(1);// habilito al proximo jugador para que tire su carta
    }
    semTurnoJugador[NEXT_PLAYER(playerNum)].v(1);// habilito al proximo jugador para que termine su ejecucion
}
