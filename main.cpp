#include <iostream>
#include "logger/Log.h"
#include "constantes.h"
#include "Juego.h"
#include <vector>
#include <algorithm>

bool validarParametros(int argc, char **argv) {

    bool validos = true;

    if(argc > 3) {
        Log::instance()->Log::append(TOO_MANY_ARGS, Log::ERROR);
        validos = false;
    }

    if((char) argv[2] == 'd'){
        Log::instance()->loggerLevel = Log::INFO;
        Log::instance()->append(INFO_MODE, Log::INFO);
    }

    if(atoi(argv[1]) == 0 || atoi(argv[2])%2 != 0) {
        Log::instance()->append(INVALID_PLAYER_AMOUNT, Log::ERROR);
        validos = false;
    }
    return validos;
}

void ayuda() {
    std::cout << HELP << std::endl;
}

int main(int argc, char** argv) {

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


    Log::instance()->loggerLevel = Log::ERROR;
    if(!validarParametros(argc, argv)) {
        ayuda();
        return 0;
    }

    int numJugadores = atoi(argv[1]);
    Log::instance()->append(
            std::string(STARTING_WITH) + std::to_string(numJugadores),
            getpid(),
            Log::INFO);

    Juego* juego = new Juego(numJugadores);
    int resultado = juego->correr();

    if(resultado == 0){
        Log::instance()->append(GAME_EXIT_OK, getpid(), Log::INFO);
    } else {
        Log::instance()->append(GAME_EXIT_ERROR, getpid(), Log::ERROR);
    }
    return resultado;
}