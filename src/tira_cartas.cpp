#include <vector>
#include "semaphore/Semaphore.h"
#include "constantes.h"
#include "shared_stack/SharedStack.h"
#include "shmem/MemoriaCompartida.h"

#define NEXT_PLAYER(player_num) (player_num + 1)%num_players

int main(int argc, char *argv[]) {

    if (fork() == 0) {
        execv("juega", argv);
    }

    int num_players = atoi(argv[0]); // cantidad total de jugadores

    int player_num = atoi(argv[1]); // mi número de jugador

    //std::cout << "Player PID: " << getpid() << " jugador: " << player_num << " de " << num_players << " jugadores." << std::endl;

    std::vector<Semaphore> sem_player;

    for (int i = 0; i < num_players; i++) {
        sem_player.push_back(Semaphore("/bin/bash", (char) (1 + i), 0)); // semáforos para el turno de cada jugador
    }

    Semaphore sem_jugar("/bin/bash", SEM_JUGAR, 0); // semáforo para que empiecen a hacer las acciones del juego

    Semaphore sem_turno_terminado("/bin/bash", SEM_TURNO, 0); // semáforo para esperar a que jueguen todos

    SharedStack centralCards(SHMEM_PATH, SHM_CARDS, NUM_CARDS);

    SharedStack myCards(SHMEM_PATH, SHM_CARDS + 1 + player_num, NUM_CARDS);

    MemoriaCompartida<bool> hayGanador(SHMEM_PATH, SHM_WINNER);

    // proceso que espera el turno de cada jugador

    while (true) {
        // si me quede sin cartas, gané
        // un semaforo para cada jugador, que queda libre cuando el jugador anterior termina su turno

        sem_player[player_num].p(); // espero a que sea mi turno
        std::cout << "****************************************" << std::endl;
        std::cout << "[" << player_num << "]" << " Empieza mi turno " << std::endl;
        int myTopCard = myCards.pop();
        centralCards.push(myTopCard); // pongo la carta

        std::cout << "[" << player_num << "]" << " Pongo carta " << myTopCard << std::endl;
        std::cout << "[" << player_num << "]" << " Top: " << centralCards.top() << std::endl;

        MemoriaCompartida<int> numero_jugador("/bin/bash", SHM_PLAYER_NUM);

        numero_jugador.escribir(0);

        sem_jugar.v(); // los jugadores ahora empiezan a hacer las acciones del juego

        sleep(5);

        sem_turno_terminado.p(); // espero a que cada uno haga lo suyo

        sleep(5);

        std::cout << "[" << player_num << "]" << " Termina mi turno." << std::endl;

        if (myCards.size() == 0) {
            std::cout << "Jugador " << player_num << " ganó." << std::endl;
            hayGanador.escribir(true);
            std::cout << "Se seteo con " << hayGanador.leer() << " si hay ganador." << std::endl;
            // matar todos los procesos
            break;
        }

        std::cout << "****************************************" << std::endl;

        sem_player[NEXT_PLAYER(player_num)].v();// habilito al proximo jugador para que tire su carta
    }
}