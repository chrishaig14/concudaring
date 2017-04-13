//
// Created by chris on 12/04/17.
//

int main() {

    // proceso para hacer la accion que corresponda segun la ultim acarta jugada

    while (true) {
        sem_jugar.p(); // hago lo mio segun la carta
        numero_jugador.escribir(numero_jugador.leer() + 1); // para controlar quien es el ultimo jugador
        // de cada ronda
        bool ultimo = numero_jugador.leer() == total_jugadores;
        if (centralCards.top() == centralCards.secondTop() || centralCards.top() == 7) {
            // es un 7 o es igual a la carta anterior
            std::cout << "Pongo mano sobre la pila de cartas" << std::endl;
            if (ultimo) {
                // fui el ultimo en poner la mano, agarro todas las cartas
                myCards.push(centralCards);
                centralCards.clear();
            }
        } else {
            if (centralCards.top() == 10) {
                std::cout << "Buenos dias señorita" << std::endl;
            } else if (centralCards.top() == 11) {
                std::cout << "Buenas noche caballero" << std::endl;
            } else if (centralCards.top() == 12) {
                std::cout << "Saludo militar" << std::endl;
            }
        }

        if (ultimo) {
            // fui el ultimo de la ronda, empieza el proximo turno
            sem_turno_terminado.v();
        } else {
            // ya termine la accion que tocaba segun la carta, le toca al que sigue
            sem_jugar.v();
        }

    }
}