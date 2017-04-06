#include <iostream>
#include <unistd.h>
#include "shmem/MemoriaCompartida2.h"
#include <sys/wait.h>
#include <vector>
#include <algorithm>
#include <sys/sem.h>

int main() {

//      Proceso principal -> árbitro
//      Un proceso por cada jugador.
//      Los jugadores saben el pid del árbitro para mandarle señales
//      El árbitro sabe el pid de todos los jugadores

//     Pila central que ven todos.
//     Cada uno tiene su pila de cartas.

//     El árbitro le manda una señal al jugador que tiene el turno y le dice que juege ahora.
//     El jugador toma la primera carta de su pila y la pone en la pila central
//     Manda una señal al árbitro avisando que ya puso la carta.
//     El árbitro manda una señal a cada jugador avisando que ya se jugó la carta.
//     Todos los jugadores se fijan qué carta está arriba de todo.
//     Si la carta es igual a la anterior o es un 7, cada jugador pone su mano sobre la pila central.
//     El último en poner la mano agarra todas las cartas y las agrega a su pila de cartas.
//     Si era un 10, 11 o 12, cada uno hace lo que corresponda según las reglas del juego.
//     Finalmente, el último jugador (el que agarró las cartas, o el último en decir "Buenos días...")
//     Envía una señal al árbitro avisando que ya jugaron todos.

//      Al final de cada mano el árbitro se fija si hay alguno sin cartas. Si hay , ese es el ganador.

//    *** Hay que tener algo para saber quién fue el último jugador de cada mano. ***




    MemoriaCompartida2<int> cardStack; // Pila central de cartas
    cardStack.crear("/bin/bash", 'A');

    pid_t pid_referee = getpid();
    std::vector<pid_t> pid_players(numPlayers);

    std::vector<MemoriaCompartida2<int>> players_cards(numPlayers);


    for (int i = 0; i < numPlayers; i++) {
        pid_t pid = fork();
        pid_players[i] = pid;
        if (pid == 0) {

            // Código de los jugadores

            MemoriaCompartida2<int> cards("/bin/bash", 'a' + i); // Pila del jugador

            // El árbitro me manda la señal para que ponga mi carta

            cards.sacar_carta();

            cardStack.escribir(cards.leer()); // Poner mi primera carta en la pila

            kill(pid_referee, SIGINT); // Aviso que y apuse la carta

            // El árbitro avisa que ya está la carta en la pila.


            if (previous == current || current == 7) {
                std::cout << "Pongo mano sobre la pila" << std::endl;
                if (soy el último) {
                    agarrar cartas
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
                kill(pid_referee, SIGINT);
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

    for (int i = 0; i < numPlayers; i++) {
        players_cards[i] = MemoriaCompartida2<int>("/bin/bash", 'a' + i); // Pila de cada jugador
    }

    for (int i = 0; i < 48; ++i) {
        players_cards[i % (48 / numPlayers)].escribir(deck[i]);
    }


    int player_turn = 0;

    int winner() {
        for (int i = 0; i < numPlayers; i++) {
            if(players_cards[i].size() == 0){
                return i; // i del jugador que ganó
            }
        }
        return -1;
    }

    do {

        // Aviso al jugador que ponga su carta
        kill(pid_players[player_turn], SIGINT);

        // El jugador me avisa con una señal que ya puso la carta

        for (int i = 0; i < numPlayers; i++) {
            // Aviso a los jugadores que ya está la carta en la pila central
            kill(pid_players[i], SIGINT); // Otra señal que no sa la misma que antes.
        }

        // El último jugador me avisa que ya jugaron todos.

        player_turn = (player_turn + 1) % numPlayers;
    } while (winner() == -1);

    std::cout << "ganó el jugador " << winner() << std::endl;


}