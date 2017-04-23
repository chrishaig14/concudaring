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
//    std::cout << getpid() << " ... " << semid << " =" << valorInicial << std::endl;
//    std::cout << "#"<< semid << "#"<< "The semaphore has been initialized to " << this->valorInicial << "with result: " << resultado << std::endl;
    //perror("Error");
    //std::cout << "i:" << (resultado==0) << std::endl;
    return resultado;
}


int Semaphore::p(int n) const {
    struct sembuf operacion;
    operacion.sem_num = 0; // numero de Semaphore
    operacion.sem_op = -n; // restar 1 al Semaphore
    operacion.sem_flg = 0; //sem_undo
//    std::cout << "p before "<< semctl(this->semid, 0, GETVAL, NULL) << std::endl;
    int resultado = semop(this->semid, &operacion, 1);
//    std::cout << "p after "<<semctl(this->semid, 0, GETVAL, NULL) << std::endl;
//    std::cout << "#"<< semid << "#"<< "Wait operation with n=" << operacion.sem_op <<" finished with result: " << resultado << std::endl;
    //perror("Error");
//    std::cout << "p:" << (resultado==0) << std::endl;
//    std::cout << getpid() << " ... " << semid << " -" << n << std::endl;
    return resultado;
}

int Semaphore::v(int n) const {
    struct sembuf operacion;
    operacion.sem_num = 0; // numero de Semaphore
    operacion.sem_op = n; // sumar 1 al Semaphore
    operacion.sem_flg = 0; //sem_undo
//    std::cout << "v before " <<semctl(this->semid, 0, GETVAL, NULL) << std::endl;
    int resultado = semop(this->semid, &operacion, 1);
//    std::cout << "v after " << semctl(this->semid, 0, GETVAL, NULL) << std::endl;
//    std::cout << "#"<< semid << "#"<< "Signal operation with n=" << operacion.sem_op << " finished with result: " << resultado << std::endl;
    //perror("Error");
//    std::cout << "v:" << (resultado==0) << std::endl;
//    std::cout << getpid() << " ... " << semid << " +"<<n  << std::endl;
    return resultado;
}

void Semaphore::eliminar() const {
    semctl(this->semid, 0, IPC_RMID);
}

Semaphore::~ Semaphore() {

}