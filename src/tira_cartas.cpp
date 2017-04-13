//
// Created by chris on 11/04/17.
//

int main() {
    // proceso que espera el turno de cada jugador
    while (true) {
        // un semaforo para cada jugador, que queda libre cuando el jugador anterior termina su turno
        sem_player[i].p(); // espero a que sea mi turno
        cards.push(myTopCard); // pongo la carta
        sem_jugar.v(); // los jugadores ahora empiezan a hacer las acciones del juego
        sem_turno_terminado.p(); // espero a que cada uno haga lo suyo
        sem_player[i + 1].v();// habilito al proximo jugador para que tire su carta
    }
}