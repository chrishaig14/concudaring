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

    Semaphore semAcciones(KEY_PATH, SEM_ACCIONES, 0);

    std::vector<Semaphore> semJugadorAccion;
    for (int i = 0; i < cantJugadores; i++) {
        semJugadorAccion.push_back(Semaphore(KEY_PATH, SEM_JUGADOR_ACCION + i, 0));
    }

    Semaphore semTurnoTerminado(KEY_PATH, SEM_TURNO_TERMINADO, 0);

    MemoriaCompartida<int> numJugador(KEY_PATH,
                                      SHM_PLAYER_NUM); // aca el jugador escribe su numero cuando pone su mano sobre la pila
    MemoriaCompartida<bool> hayGanador(KEY_PATH, SHM_WINNER);
    MemoriaCompartida<int> logLevel(KEY_PATH, SHM_LOG);
    Log::instance()->loggerLevel = logLevel.leer() ? Log::ERROR : Log::DEBUG;

    SharedStack centralCards(KEY_PATH, SHM_CARDS, NUM_CARDS); // pila de cartas central

    std::ostringstream player;
    player << "[" << playerNum << "]:: ";

    while (true) {
        semJugadorAccion[playerNum].p(cantJugadores);
        semAcciones.p(1); // hago lo mio segun la carta
        if (hayGanador.leer()){
            return 0; // Si hay un ganador, termino mi ejecucion
        }
        std::ostringstream s;
        s << player.str();
        int tope = centralCards.top();
        if (tope == cartaAnterior || tope == CARTA_7) {
            s << "Pongo mano sobre la pila de cartas";
            numJugador.escribir(playerNum + 1);
        } else {
            if (tope == CARTA_10) {
                s << "Buenos dias señorita";
            } else if (tope == CARTA_11) {
                s << "Buenas noche caballero";
            } else if (tope == CARTA_12) {
                s << "Saludo militar";
            } else {
                s << "No hago nada";
            }
        }
        Log::instance()->append(s.str(), Log::DEBUG);
        cartaAnterior = tope;
        for (int i = 0; i < cantJugadores; i++) {
            semJugadorAccion[i].v(1);
        }
        semTurnoTerminado.v(1);
    }
}
