#include <iostream>
#include <unistd.h>
#include "shmem/MemoriaCompartida.h"
#include "constantes.h"
#include "Juego.h"
#include <sys/wait.h>
#include <vector>
#include <algorithm>

Juego::Juego(int cantJugadores) {
    this->cantJugadores = cantJugadores;
}

Juego::~Juego() {

}

int Juego::correr() {

    MemoriaCompartida<int> pilaCartas; // Pila central de cartas
    pilaCartas.crear(SHMEM_PATH, 'A');

    pid_t pidReferi = getpid();
    std::vector<pid_t> pid_players(this->cantJugadores);

    std::vector<MemoriaCompartida<int>> cartasJugadores(this->cantJugadores);


    for (int i = 0; i < this->cantJugadores; i++) {
        pid_t pid = fork();
        pid_players[i] = pid;
        if (pid == 0) {

            // Código de los jugadores

            MemoriaCompartida<int> cartas(SHMEM_PATH, 'a' + i); // Pila del jugador

            // El árbitro me manda la señal para que ponga mi carta

            cartas.sacar_carta();

            pilaCartas.escribir(cartas.leer()); // Poner mi primera carta en la pila

            kill(pidReferi, SIGINT); // Aviso que y apuse la carta

            // El árbitro avisa que ya está la carta en la pila.


            if (previous == current || current == 7) {
                std::cout << "Pongo mano sobre la pila" << std::endl;
                if (soy el último) {
                    //agarrar cartas
                }
            } else if (current == 10) {
                std::cout << "Buenos días señorita" << std::endl;
            } else if (current == 11) {
                std::cout << "Buenos noches caballero" << std::endl;
            } else if (current == 12) {
                std::cout << "Saludo militar" << std::endl;
            }

            if (soy el último){
                // Enviar señal al árbitro de que ya terminó la mano.
                kill(pidReferi, SIGINT);
            }
        }
    }

    // Repartir cartas

    srand(time(NULL));
    std::vector<int> deck(48, 0);
    for (int i = 0; i < 48; ++i) {
        deck[i] = i % 12 + 1;
    }
    std::random_shuffle(deck.begin(), deck.end());

    for (int i = 0; i < this->cantJugadores; i++) {
        cartasJugadores[i] = MemoriaCompartida<int>(SHMEM_PATH, 'a' + i); // Pila de cada jugador
    }

    for (int i = 0; i < 48; ++i) {
        cartasJugadores[i % (48 / this->cantJugadores)].escribir(deck[i]);
    }


    int player_turn = 0;

    int winner() {
        for (int i = 0; i < this->cantJugadores; i++) {
            if(cartasJugadores[i].size() == 0){
                return i; // i del jugador que ganó
            }
        }
        return -1;
    }

    do {

        // Aviso al jugador que ponga su carta
        kill(pid_players[player_turn], SIGINT);

        // El jugador me avisa con una señal que ya puso la carta

        for (int i = 0; i < this->cantJugadores; i++) {
            // Aviso a los jugadores que ya está la carta en la pila central
            kill(pid_players[i], SIGINT); // Otra señal que no sa la misma que antes.
        }

        // El último jugador me avisa que ya jugaron todos.

        player_turn = (player_turn + 1) % this->cantJugadores;
    } while (winner() == -1);

    Log::instance()->append(
            std::string(PLAYER_WON) + std::to_string(winner()),
            pidReferi,
            Log::INFO);

    std::cout << PLAYER_WON << winner() << std::endl;

    return 0;
}