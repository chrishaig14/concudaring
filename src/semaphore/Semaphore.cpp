#include <iostream>
#include <sstream>
#include "Semaphore.h"
#include "../logger/Log.h"

Semaphore::Semaphore(const std::string &nombre, char char_id, const int valorInicial) : valorInicial(

        valorInicial) {
    clave = ftok(nombre.c_str(), char_id);
    this->semid = semget(clave, 1, 0666 | IPC_CREAT);
    if (this->semid == -1) {
        std::ostringstream serror;
        serror << "Error al obtener semáforo:";
        Log::instance()->append(serror.str(), Log::ERROR);
        perror(serror.str().c_str());
    }
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
    if (resultado == -1) {
        std::ostringstream serror;
        serror << "Error al inicializar semáforo:";
        Log::instance()->append(serror.str(), Log::ERROR);
        perror(serror.str().c_str());
    }
    return resultado;
}


int Semaphore::p(int n) const {
    struct sembuf operacion;
    operacion.sem_num = 0; // numero de Semaphore
    operacion.sem_op = -n; // restar n al Semaphore
    operacion.sem_flg = 0; //sem_undo
    int resultado = semop(this->semid, &operacion, 1);
    if (resultado == -1) {
        std::ostringstream serror;
        serror << "Error en operación p de semáforo:";
        Log::instance()->append(serror.str(), Log::ERROR);
        perror(serror.str().c_str());
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
        std::ostringstream serror;
        serror << "Error en operación v de semáforo:";
        Log::instance()->append(serror.str(), Log::ERROR);
        perror(serror.str().c_str());
    }
    return resultado;
}

void Semaphore::eliminar() const {
    int result = semctl(this->semid, 0, IPC_RMID);
    if (result != 0) {
        std::ostringstream serror;
        serror << "Error al eliminar semáforo:";
        Log::instance()->append(serror.str(), Log::ERROR);
        perror(serror.str().c_str());
    }
}

Semaphore::~ Semaphore() {

}

