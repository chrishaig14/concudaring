//
// Created by chris on 12/04/17.
//

#include <vector>
#include "semaphore/Semaphore.h"
#include "constantes.h"
#include "shmem/MemoriaCompartida.h"
#include "shared_stack/SharedStack.h"

int main(int argc, char *argv[]) {

    int num_players = atoi(argv[0]); // cantidad total de jugadores

    int player_num = atoi(argv[1]); // cantidad total de jugadores

    Semaphore sem_jugar("/bin/bash", SEM_JUGAR, 0); // semáforo para que empiecen a hacer las acciones del juego
    Semaphore sem_turno_terminado("/bin/bash", SEM_TURNO, 0); // semáforo para esperar a que jueguen todos

    // proceso para hacer la accion que corresponda segun la ultim acarta jugada

    MemoriaCompartida<int> numero_jugador("/bin/bash", SHM_PLAYER_NUM);

    SharedStack centralCards("/bin/bash", SHM_CARDS, NUM_CARDS);

    SharedStack myCards("/bin/bash", SHM_CARDS + player_num + 1, NUM_CARDS);

    int previous_card = -1;

    while (true) {
        sem_jugar.p(); // hago lo mio segun la carta
        std::cout << std::endl;
        numero_jugador.escribir(numero_jugador.leer() + 1); // para controlar quien es el ultimo jugador
        // de cada ronda
        bool ultimo = numero_jugador.leer() == num_players;
        std::cout << "[" << player_num << "]" << " Numero:" << numero_jugador.leer() << std::endl;
        sleep(2);
        int topCard = centralCards.top();
        std::cout << "[" << player_num << "]" << " Top: " << topCard << std::endl;
        std::cout << "[" << player_num << "]" << " Previous: " << previous_card << std::endl;

        if (topCard == previous_card || topCard == 7) {
            // es un 7 o es igual a la carta anterior
            std::cout << "[" << player_num << "]" << "Pongo mano sobre la pila de cartas, top=" << topCard << " prev="
                      << previous_card << std::endl;
            if (ultimo) {

                std::cout << "[" << player_num << "]" << " Me llevo todas las cartas :(" << std::endl;
                sleep(40);
//                // fui el ultimo en poner la mano, agarro todas las cartas
//                for(int i = 0; i < centralCards.size(); i++) {
//                    myCards.push(centralCards.pop());
//                }
//                centralCards.clear();
            }
        } else {
            if (topCard == 10) {
                std::cout << "[" << player_num << "]" << " Buenos dias señorita" << std::endl;
            } else if (topCard == 11) {
                std::cout << "[" << player_num << "]" << " Buenas noche caballero" << std::endl;
            } else if (topCard == 12) {
                std::cout << "[" << player_num << "]" << " Saludo militar" << std::endl;
            }
        }
        if (ultimo) {
            // fui el ultimo de la ronda, empieza el proximo turno
            std::cout << "[" << player_num << "]" << " Ultimo de la ronda" << std::endl;
            numero_jugador.escribir(0);
            sem_turno_terminado.v();
        } else {
            // ya termine la accion que tocaba segun la carta, le toca al que sigue
            std::cout << "[" << player_num << "]" << " El que sigue" << std::endl;
            sem_jugar.v();
        }


        previous_card = topCard;

    }
}
