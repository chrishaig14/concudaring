//
// Created by chris on 08/04/17.
//

#ifndef CONCUDARING_SEMAPHORE_H
#define CONCUDARING_SEMAPHORE_H

#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/types.h>
#include <string>

class Semaphore {

private :

    int semid;

    int valorInicial;

public :

    Semaphore(const std::string &nombre, char char_id, const int valorInicial);

    ~ Semaphore();

    int p(int n) const; // decrementa

    int v(int n) const; // incrementa

    void eliminar() const;

    int inicializar() const;

    int id();
};

#endif //CONCUDARING_SEMAPHORE_H
