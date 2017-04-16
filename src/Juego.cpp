#include <iostream>
#include <unistd.h>
#include "shmem/MemoriaCompartida.h"
#include "constantes.h"
#include "Juego.h"
#include "semaphore/Semaphore.h"
#include <algorithm>
#include <sys/wait.h>

Juego::Juego(int cantJugadores) {
    this->cantJugadores = cantJugadores;
}

Juego::~Juego() {

}

pid_t crearReferi(char* cant_str) {
    pid_t pid = fork();
    if (pid == 0) {
        execl("referi", cant_str, NULL); // creo al referi que corre en su propio proceso
    }
    return pid;
}

int Juego::correr() {
    //SharedStack centralCards("/bin/bash", SHM_CARDS, NUM_CARDS);

    std::vector<pid_t> pid_players(cantJugadores);

    std::vector<SharedStack> cartasJugadores;

    std::vector<Semaphore> sem_player;

    MemoriaCompartida<bool> hayGanador(SHMEM_PATH, SHM_WINNER);

    hayGanador.escribir(false);

    for (int i = 0; i < cantJugadores; i++) {
        // semáforos para el turno de cada jugador
        // inicializar todos en 0, para que se queden esperando
        sem_player.push_back(Semaphore("/bin/bash", (char) (1 + i), 0));
        sem_player[i].inicializar();
    }

    Semaphore sem_jugar("/bin/bash", SEM_JUGAR, 0); // semáforo para que empiecen a hacer las acciones del juego
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
            execl("tira_cartas", cant_str, num_str, NULL);
            // tira_cartas.cpp
        }
    }

    // Repartir cartas

    std::cout << "# Empiezo a repartir las cartas" << std::endl;

    repartir_cartas(cartasJugadores);

    std::cout << "# Se repartieron " << NUM_CARDS << ", empieza el juego" << std::endl;

    sem_player[0].v(); // ahora puede jugar el jugador 1

//    Log::instance()->append(
//            std::string(PLAYER_WON) + std::to_string(winner()),
//            pidReferi,
//            Log::INFO);

//    std::cout << PLAYER_WON << winner() << std::endl;

    //pid_t ref_pid = crearReferi(cant_str);

    while(!hayGanador.leer()) { // Podria usar otro semaforo para bloquear este proceso hasta que gane un jugador
        sleep(1);
    }

    for (int i = 0; i < cantJugadores; i++) {
        kill(pid_players[i], SIGQUIT);
        int child_return;
        int id = wait(&child_return);
        std::cout << "El proceso hijo " << id << " ha terminado." << std::endl;
    }

    return 0;
}

void Juego::repartir_cartas(std::vector<SharedStack> &cartasJugadores) {
    srand(time(NULL));
    std::vector<int> deck(NUM_CARDS, 0);
    for (int i = 0; i < NUM_CARDS; ++i) {
        deck[i] = i % NUM_CARDS/4 + 1;
    }
    random_shuffle(deck.begin(), deck.end());

    for (int i = 0; i < this->cantJugadores; i++) {
        cartasJugadores.push_back(SharedStack(SHMEM_PATH, SHM_CARDS + i + 1, NUM_CARDS)); // Pila de cada jugador
    }

    // reparto cartas
    for (int i = 0; i < NUM_CARDS; ++i) {
        cartasJugadores[i % this->cantJugadores].push(deck[i]);
    }
}