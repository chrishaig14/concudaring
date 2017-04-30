#include <iostream>
#include <sstream>
#include <unistd.h>
#include "shmem/MemoriaCompartida.h"
#include "constantes.h"
#include "Juego.h"
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
    std::vector<pid_t> pid_players(cantJugadores);
    std::vector<SharedStack> cartasJugadores;
    std::vector<Semaphore> sem_player;

    MemoriaCompartida<bool> hayGanador(SHMEM_PATH, SHM_WINNER);
    MemoriaCompartida<int> logLevel(SHMEM_PATH, SHM_LOG);

    hayGanador.escribir(false);

    for (int i = 0; i < cantJugadores; i++) {
        // semáforos para el turno de cada jugador
        // inicializar todos en 0, para que se queden esperando
        sem_player.push_back(Semaphore("/bin/bash", (char) (1 + i), 0));
        sem_player[i].inicializar();
        Semaphore sem_jugar("/bin/bash", SEM_JUGAR + i, 0);
        sem_jugar.inicializar();
    }

    char cant_str[32];
    sprintf(cant_str, "%d", cantJugadores);

    for (int i = 0; i < cantJugadores; i++) {
        pid_t pid = fork();
        pid_players[i] = pid;
        if (pid == 0) {
            char num_str[32];
            sprintf(num_str, "%d", i);
            execl("tira_cartas", cant_str, num_str, NULL);
        }
    }

    Log::instance()->append("Empiezo a repartir las cartas", Log::DEBUG);

    repartir_cartas(cartasJugadores);

    std::ostringstream s1;
    s1 << "[-]:: Se repartieron " << NUM_CARDS << " cartas, empieza el juego.";
    Log::instance()->append(s1.str(), Log::DEBUG);

    int first_player = rand()%cantJugadores;

    sem_player[first_player].v(1); // un jugador random puede jugar
    s1.clear();
    s1.str("");
    s1 << "[-]:: Comienza el jugador " << first_player;
    Log::instance()->append(s1.str(), Log::DEBUG);

    pid_t ref_pid = crearReferi(cant_str);

    for (int i = 0; i < cantJugadores; i++) {
        int id = wait(NULL);
        std::ostringstream s;
        s << "[-]:: El proceso hijo " << id << " ha terminado.";
        Log::instance()->append(s.str(), Log::DEBUG);
    }

    limpiar_semaforos(sem_player);

    waitpid(ref_pid, NULL, 0);
    Log::instance()->append("[-]:: El referi ha terminado.", Log::DEBUG);

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

void Juego::limpiar_semaforos(std::vector<Semaphore> &sem_jugadores) {
    for (int i = 0; i < cantJugadores; i++) {
        sem_jugadores[i].eliminar();
    }

    for (int i = 0; i < cantJugadores; i++) {
        Semaphore sem_jugar("/bin/bash", SEM_JUGAR + i, 1);
        sem_jugar.eliminar();
    }
}
