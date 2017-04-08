#ifndef CONCUDARING_GAME_H
#define CONCUDARING_GAME_H

class Juego {
public:
    Juego(int cantJugadores);
    ~ Juego();
    int correr();

private:
    int cantJugadores;
};


#endif //CONCUDARING_GAME_H
