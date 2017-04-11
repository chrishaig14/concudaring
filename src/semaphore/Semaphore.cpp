//
// Created by chris on 08/04/17.
//

#include "Semaphore.h"

Semaphore::Semaphore(const std::string &nombre, const int valorInicial) : valorInicial(

        valorInicial) {
    key_t clave = ftok(nombre.c_str(), 'a');
    this->id = semget(clave, 1, 0666 | IPC_CREAT);
    this->inicializar();
}

int Semaphore::inicializar() const {
    union semnum {
        int val;
        struct semid_ds *buf;
        ushort *array;
    };
    semnum init;
    init.val = this->valorInicial;
    int resultado = semctl(this->id, 0, SETVAL, init);
    return resultado;
}


int Semaphore::p() const {
    struct sembuf operacion;
    operacion.sem_num = 0; // numero de Semaphore
    operacion.sem_op = -1; // restar 1 al Semaphore
    operacion.sem_flg = SEM_UNDO;
    int resultado = semop(this->id, &operacion, 1);
    return resultado;
}

int Semaphore::v() const {
    struct sembuf operacion;
    operacion.sem_num = 0; // numero de Semaphore
    operacion.sem_op = 1; // sumar 1 al Semaphore
    operacion.sem_flg = SEM_UNDO;
    int resultado = semop(this->id, &operacion, 1);
    return resultado;
}

void Semaphore::eliminar() const {
    semctl(this->id, 0, IPC_RMID);
}

Semaphore::~ Semaphore() {

}