//
// Created by chris on 11/04/17.
//

#include <vector>
#include "semaphore/Semaphore.h"
#include "constantes.h"
#include "SharedStack.h"
#include "shmem/MemoriaCompartida.h"

#define NEXT_PLAYER(player_num) (player_num + 1)%num_players

int main(int argc, char *argv[]) {

    int num_players = atoi(argv[0]); // cantidad total de jugadores

    int player_num = atoi(argv[1]); // mi número de jugador

//    std::cout << "tira_cartas created, player " << player_num << std::endl;

    std::vector<Semaphore> sem_player;

//    std::cout << "sem vector created, player " << player_num << std::endl;



    for (int i = 0; i < num_players; i++) {
        sem_player.push_back(Semaphore("/bin/bash", (char) (1 + i), 0)); // semáforos para el turno de cada jugador
//        std::cout << "sem for player " << i << " , player " << player_num << std::endl;
    }

    Semaphore sem_jugar("/bin/bash", SEM_JUGAR, 0); // semáforo para que empiecen a hacer las acciones del juego

//    std::cout << "sem jugar " << " , player " << player_num << std::endl;

    Semaphore sem_turno_terminado("/bin/bash", SEM_TURNO, 0); // semáforo para esperar a que jueguen todos
//    std::cout << "sem turno " << " , player " << player_num << std::endl;


    SharedStack centralCards("/bin/bash", SHM_CARDS, NUM_CARDS);

//    std::cout << "central cards " << " , player " << player_num << std::endl;

    SharedStack myCards("/bin/bash", SHM_CARDS + 1 + player_num, NUM_CARDS, NUM_CARDS / num_players);

//    std::cout << "my cards " << " , player " << player_num << std::endl;



    // proceso que espera el turno de cada jugador

    while (true) {
        // un semaforo para cada jugador, que queda libre cuando el jugador anterior termina su turno
        std::cout << "[" << player_num << "]" << " semid = " << sem_player[player_num].id() << " -> wait" << std::endl;
        sem_player[player_num].p(); // espero a que sea mi turno
        std::cout << "[" << player_num << "]" << " semid = " << sem_player[player_num].id() << " -> 0" << std::endl;
        std::cout << "[" << player_num << "]" << " Empieza mi turno " << std::endl;
        int myTopCard = myCards.pop();

        centralCards.push(myTopCard); // pongo la carta
        std::cout << "[" << player_num << "]" << " Pongo carta " << myTopCard << std::endl;
        sem_jugar.v(); // los jugadores ahora empiezan a hacer las acciones del juego
        std::cout << "[" << player_num << "]" << " Jugamos " << std::endl;

        std::cout << "[" << player_num << "]" << " Duermo por 1 segundo " << std::endl;
        sleep(1);
        std::cout << "[" << player_num << "]" << " Listo " << std::endl;
        //sem_turno_terminado.p(); // espero a que cada uno haga lo suyo

        std::cout << "[" << player_num << "]" << " Termina mi turno " << std::endl;
        // si me quede sin cartas, gané
        if (myCards.size() == 0) {
            std::cout << "Player " << player_num << " wins" << std::endl;
            // matar todos los procesos
            break;
        }
        sem_player[NEXT_PLAYER(player_num)].v();// habilito al proximo jugador para que tire su carta
        std::cout << "[" << player_num << "]" << " semid = " << sem_player[NEXT_PLAYER(player_num)].id() << " -> 1"
                  << std::endl;
    }
}