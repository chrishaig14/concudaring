#include <iostream>
#include <unistd.h>
#include "shmem/MemoriaCompartida.h"
#include "constantes.h"
#include "Juego.h"
#include "semaphore/Semaphore.h"
#include <algorithm>

Juego::Juego(int cantJugadores) {
    this->cantJugadores = cantJugadores;
}

Juego::~Juego() {

}

int Juego::correr() {
    int num_players = cantJugadores;

    SharedStack centralCards("/bin/bash", SHM_CARDS, NUM_CARDS);

    pid_t pidReferi = getpid();

    std::vector<pid_t> pid_players(cantJugadores);

    std::vector<SharedStack> cartasJugadores;

    std::vector<Semaphore> sem_player;

    for (int i = 0; i < num_players; i++) {
        // semáforos para el turno de cada jugador
        // inicializar tdos en 0, para que se queden esperando
        sem_player.push_back(Semaphore("/bin/bash", (char) (1 + i), 0));
        sem_player[i].inicializar();
    }

    //sleep(60);

    Semaphore sem_jugar("/bin/bash", SEM_JUGAR, 1); // semáforo para que empiecen a hacer las acciones del juego
    sem_jugar.inicializar();
    Semaphore sem_turno_terminado("/bin/bash", SEM_TURNO, 0); // semáforo para esperar a que jueguen todos
    sem_turno_terminado.inicializar();

    char cant_str[32];
    sprintf(cant_str, "%d", cantJugadores);

    for (int i = 0; i < cantJugadores; i++) {
        pid_t pid = fork();
        pid_players[i] = pid;
        if (pid == 0) {
            char num_str[32];
            sprintf(num_str, "%d", i);
            //std::cout << i << " - " << getpid() << std::endl;
            execl("tira_cartas", cant_str, num_str, "\0");
            // tira_cartas.cpp
        }
    }

    // Repartir cartas

    std::cout << "# Empiezo a repartir las cartas" << std::endl;

    repartir_cartas(cartasJugadores);

    std::cout << "# Cartas repartidas, empieza el juego" << std::endl;

    sem_player[0].v(); // ahora puede jugar el jugador 1

    std::cout << "[*] " << sem_player[0].id() << " -> 1" << std::endl;

//    Log::instance()->append(
//            std::string(PLAYER_WON) + std::to_string(winner()),
//            pidReferi,
//            Log::INFO);

//    std::cout << PLAYER_WON << winner() << std::endl;

    return 0;
}

void Juego::repartir_cartas(std::vector<SharedStack> &cartasJugadores) {
    srand(time(NULL));
    std::vector<int> deck(48, 0);
    for (int i = 0; i < 48; ++i) {
        deck[i] = i % 12 + 1;
    }
    random_shuffle(deck.begin(), deck.end());

    for (int i = 0; i < this->cantJugadores; i++) {
        cartasJugadores.push_back(SharedStack(SHMEM_PATH, SHM_CARDS + i + 1, NUM_CARDS)); // Pila de cada jugador
    }

    // reparto cartas
    for (int i = 0; i < 48; ++i) {
        cartasJugadores[i % this->cantJugadores].push(deck[i]);
    }
}