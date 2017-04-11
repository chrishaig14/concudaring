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

    int id;

    int valorInicial;

    int inicializar() const;

public :

    Semaphore(const std::string &nombre, const int valorInicial);

    ~ Semaphore();

    int p() const; // decrementa

    int v() const; // incrementa

    void eliminar() const;

};

#endif //CONCUDARING_SEMAPHORE_H
