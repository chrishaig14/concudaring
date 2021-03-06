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

    std::vector<SharedStack> cartasJugadores;
    std::vector<Semaphore> semTurnoJugador;

    MemoriaCompartida<bool> hayGanador(KEY_PATH, SHM_WINNER);
    MemoriaCompartida<int> logLevel(KEY_PATH, SHM_LOG);

    std::ostringstream s1;
    std::ostringstream smain;
    smain << "[-]:: ";

    hayGanador.escribir(false);

    Semaphore semAcciones(KEY_PATH, SEM_ACCIONES, 0);
    semAcciones.inicializar();

    for (int i = 0; i < cantJugadores; i++) {
        // semáforos para el turno de cada jugador
        // inicializar todos en 0, para que se queden esperando
        semTurnoJugador.push_back(Semaphore(KEY_PATH, SEM_TURNO_JUGADOR + i, 0));
        semTurnoJugador[i].inicializar();
        Semaphore semJugadorAccion(KEY_PATH, SEM_JUGADOR_ACCION + i, cantJugadores);
        semJugadorAccion.inicializar();
    }

    char cant_str[32];
    sprintf(cant_str, "%d", cantJugadores);

    // creo los procesos para los jugadores
    crearTiraCartas(cant_str);

    s1 << smain.str() << "Empiezo a repartir las cartas";
    Log::instance()->append(s1.str(), Log::DEBUG);

    repartirCartas(cartasJugadores);

    s1.str("");
    s1 << smain.str() << "Se repartieron " << NUM_CARDS << " cartas, empieza el juego.";
    Log::instance()->append(s1.str(), Log::DEBUG);

    int primerJugador = rand()%cantJugadores;

    semTurnoJugador[primerJugador].v(1); // un jugador random puede jugar
    s1.str("");
    s1 << smain.str() << "Comienza el jugador " << primerJugador;
    Log::instance()->append(s1.str(), Log::DEBUG);

    pid_t ref_pid = crearReferi(cant_str);

    esperarHijos();

    limpiarSemaforos();

    s1.clear();
    s1.str("");
    s1 << smain.str() << "El referi ha terminado.";
    Log::instance()->append(s1.str(), Log::DEBUG);

    return 0;
}

void Juego::crearTiraCartas(const char *cant_str) const {
    for (int i = 0; i < cantJugadores; i++) {
        pid_t pid = fork();
        if (pid == 0) {
            char num_str[32];
            sprintf(num_str, "%d", i);
            execl("tira_cartas", cant_str, num_str, NULL);
        }
    }
}

void Juego::esperarHijos() {
    std::ostringstream smain;
    smain << "[-]:: ";
    for (int i = 0; i < cantJugadores + 1; i++) {// referi + 4*tira_cartas
        int id = wait(NULL);
        if (id == -1) {
            std::ostringstream serror;
            serror << "Error esperando procesos hijos:";
            Log::instance()->append(serror.str(), Log::ERROR);
            perror(serror.str().c_str());
        }
        std::ostringstream s;
        s << smain.str() << "El proceso hijo " << id << " ha terminado.";
        Log::instance()->append(s.str(), Log::DEBUG);
    }
}

void Juego::repartirCartas(std::vector<SharedStack> &cartasJugadores) {
    srand(time(NULL));
    std::vector<int> deck(NUM_CARDS, 0);
    for (int i = 0; i < NUM_CARDS; ++i) {
        deck[i] = i % NUM_CARDS/4 + 1;
    }
    random_shuffle(deck.begin(), deck.end());

    for (int i = 0; i < this->cantJugadores; i++) {
        cartasJugadores.push_back(SharedStack(KEY_PATH, SHM_CARDS + i + 1, NUM_CARDS)); // Pila de cada jugador
    }

    // reparto cartas
    for (int i = 0; i < NUM_CARDS; ++i) {
        cartasJugadores[i % this->cantJugadores].push(deck[i]);
    }
}

void Juego::limpiarSemaforos() {
    for (int i = 0; i < cantJugadores; i++) {
        Semaphore semTurnoJugador(KEY_PATH, SEM_TURNO_JUGADOR + i, 0);
        semTurnoJugador.eliminar();
    }
    Semaphore semJugar(KEY_PATH, SEM_ACCIONES, 1);
    semJugar.eliminar();
    for (int i = 0; i < cantJugadores; i++) {
        Semaphore semJugadorAccion(KEY_PATH, SEM_JUGADOR_ACCION + i, 1);
        semJugadorAccion.eliminar();
    }
    Semaphore semTurnoTerminado(KEY_PATH, SEM_TURNO_TERMINADO, 0);
    semTurnoTerminado.eliminar();
}
