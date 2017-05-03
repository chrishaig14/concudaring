#include <vector>
#include <iostream>
#include <sstream>
#include "semaphore/Semaphore.h"
#include "constantes.h"
#include "shmem/MemoriaCompartida.h"
#include "shared_stack/SharedStack.h"

int main(int argc, char *argv[]) {
    int cantJugadores = atoi(argv[0]); // numero de jugadores
    int playerNum = atoi(argv[1]); // numero de jugador

    int cartaAnterior = -1; // numero de la carta de la ronda anterior

    Semaphore semAcciones("/bin/bash", SEM_ACCIONES, 0);

    std::vector<Semaphore> semJugador;
    for (int i = 0; i < cantJugadores; i++) {
        semJugador.push_back(Semaphore("/bin/bash", SEM_JUGADOR + i, 0));
    }

    Semaphore semTurnoTerminado("/bin/bash", SEM_TURNO_TERMINADO, 0);

    MemoriaCompartida<int> numJugador("/bin/bash",
                                      SHM_PLAYER_NUM); // aca el jugador escribe su numero cuando pone su mano sobre la pila
    MemoriaCompartida<bool> hayGanador(SHMEM_PATH, SHM_WINNER);
    MemoriaCompartida<int> logLevel(SHMEM_PATH, SHM_LOG);
    Log::instance()->loggerLevel = logLevel.leer() ? Log::ERROR : Log::DEBUG;

    SharedStack centralCards("/bin/bash", SHM_CARDS, NUM_CARDS); // pila de cartas central

    std::ostringstream player;
    player << "[" << playerNum << "]:: ";

    while (true) {
        semJugador[playerNum].p(cantJugadores);
        semAcciones.p(1); // hago lo mio segun la carta
        if (hayGanador.leer()){
            //std::cout << "fin realiza_accion " << playerNum << std::endl;
            return 0; // Si hay un ganador, termino mi ejecucion
        }
        std::ostringstream s;
        s << player.str();
        int tope = centralCards.top();
        if (tope == cartaAnterior || tope == 7) {
            s << "Pongo mano sobre la pila de cartas";
            numJugador.escribir(playerNum + 1);
        } else {
            if (tope == 10) {
                s << "Buenos dias señorita";
            } else if (tope == 11) {
                s << "Buenas noche caballero";
            } else if (tope == 12) {
                s << "Saludo militar";
            } else {
                s << "No hago nada";
            }
        }
        Log::instance()->append(s.str(), Log::DEBUG);
        cartaAnterior = tope;
        for (int i = 0; i < cantJugadores; i++) {
            semJugador[i].v(1);
        }
        semTurnoTerminado.v(1);
    }
}
