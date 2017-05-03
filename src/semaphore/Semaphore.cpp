//
// Created by chris on 08/04/17.
//

#include <iostream>
#include "Semaphore.h"

Semaphore::Semaphore(const std::string &nombre, char char_id, const int valorInicial) : valorInicial(

        valorInicial) {
    key_t clave = ftok(nombre.c_str(), char_id);
    this->semid = semget(clave, 1, 0666 | IPC_CREAT);
}

int Semaphore::id() {
    return this->semid;
}

int Semaphore::inicializar() const {
    union semnum {
        int val;
        struct semid_ds *buf;
        ushort *array;
    };
    semnum init;
    init.val = this->valorInicial;
    int resultado = semctl(this->semid, 0, SETVAL, init);
    return resultado;
}


int Semaphore::p(int n) const {
    struct sembuf operacion;
    operacion.sem_num = 0; // numero de Semaphore
    operacion.sem_op = -n; // restar n al Semaphore
    operacion.sem_flg = 0; //sem_undo
    int resultado = semop(this->semid, &operacion, 1);
    if (resultado == -1) {
        perror("Error:");
    }
    return resultado;
}

int Semaphore::v(int n) const {
    struct sembuf operacion;
    operacion.sem_num = 0; // numero de Semaphore
    operacion.sem_op = n; // sumar n al Semaphore
    operacion.sem_flg = 0; //sem_undo
    int resultado = semop(this->semid, &operacion, 1);
    if (resultado == -1) {
        perror("Error:");
    }
    return resultado;
}

void Semaphore::eliminar() const {
    int result = semctl(this->semid, 0, IPC_RMID);
    if (result != 0) {
        perror("Error:");
    }
}

Semaphore::~ Semaphore() {

}

