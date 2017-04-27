#ifndef MEMORIACOMPARTIDA_H_
#define MEMORIACOMPARTIDA_H_

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <string>
#include <string.h>
#include <iostream>
#include <errno.h>
#include "../logger/Log.h"

template<class T>
class MemoriaCompartida {

private:
    int shmId;
    T *ptrDatos;

    int cantidadProcesosAdosados() const;

public:
    MemoriaCompartida();

    void crear(const std::string &archivo, const char letra);

    void liberar();

    MemoriaCompartida(const std::string &archivo, const char letra);

    MemoriaCompartida(const MemoriaCompartida &origen);

    ~MemoriaCompartida();

    MemoriaCompartida<T> &operator=(const MemoriaCompartida &origen);

    void escribir(const T &dato);

    T leer() const;
};

template<class T>
MemoriaCompartida<T>::MemoriaCompartida():shmId(0), ptrDatos(NULL) {
}

template<class T>
void MemoriaCompartida<T>::crear(const std::string &archivo, const char letra) {
    key_t clave = ftok(archivo.c_str(), letra);

    if (clave > 0) {
        this->shmId = shmget(clave, sizeof(T), 0644 | IPC_CREAT);
        if (this->shmId > 0) {
            void *tmpPtr = shmat(this->shmId, NULL, 0);
            if (tmpPtr != (void *) -1) {
                this->ptrDatos = static_cast<T *> (tmpPtr);
            } else {
                std::string mensaje = std::string("Error en shmat(): ") + std::string(strerror(errno));
                Log::instance()->append(mensaje, Log::ERROR);
                throw mensaje;
            }
        } else {
            std::string mensaje = std::string("Error en shmget(): ") + std::string(strerror(errno));
            Log::instance()->append(mensaje, Log::ERROR);
            throw mensaje;
        }
    } else {
        std::string mensaje = std::string("Error en ftok(): ") + std::string(strerror(errno));
        Log::instance()->append(mensaje, Log::ERROR);
        throw mensaje;
    }
}

template<class T>
void MemoriaCompartida<T>::liberar() {
    int errorDt = shmdt((void *) this->ptrDatos);

    if (errorDt != -1) {
        int procAdosados = this->cantidadProcesosAdosados();
        if (procAdosados == 0) {
            shmctl(this->shmId, IPC_RMID, NULL);
        }
    } else {
        std::string mensaje = std::string("Error en shmdt(): ") + std::string(strerror(errno));
        Log::instance()->append(mensaje, Log::ERROR);
        throw mensaje;
    }
}

template<class T>
MemoriaCompartida<T>::MemoriaCompartida(const std::string &archivo, const char letra):shmId(0), ptrDatos(NULL) {
    key_t clave = ftok(archivo.c_str(), letra);

    if (clave != -1) {
        this->shmId = shmget(clave, sizeof(T), 0644 | IPC_CREAT);
        if (this->shmId > 0) {
            void *tmpPtr = shmat(this->shmId, NULL, 0);
            if (tmpPtr != (void *) -1) {
                this->ptrDatos = static_cast<T *> (tmpPtr);
            } else {
                std::string mensaje = std::string("Error en shmat(): ") + std::string(strerror(errno));
                Log::instance()->append(mensaje, Log::ERROR);
                throw mensaje;
            }
        } else {
            std::string mensaje = std::string("Error en shmget(): ") + std::string(strerror(errno));
            Log::instance()->append(mensaje, Log::ERROR);
            throw mensaje;
        }
    } else {
        std::string mensaje = std::string("Error en ftok(): ") + std::string(strerror(errno));
        Log::instance()->append(mensaje, Log::ERROR);
        throw mensaje;
    }
}

template<class T>
MemoriaCompartida<T>::MemoriaCompartida(const MemoriaCompartida &origen):shmId(origen.shmId) {
    void *tmpPtr = shmat(origen.shmId, NULL, 0);

    if (tmpPtr != (void *) -1) {
        this->ptrDatos = static_cast<T *> (tmpPtr);
    } else {
        std::string mensaje = std::string("Error en shmat(): ") + std::string(strerror(errno));
        Log::instance()->append(mensaje, Log::ERROR);
        throw mensaje;
    }
}

template<class T>
MemoriaCompartida<T>::~MemoriaCompartida() {
    int errorDt = shmdt(static_cast<void *> (this->ptrDatos));

    if (errorDt != -1) {
        int procAdosados = this->cantidadProcesosAdosados();
        if (procAdosados == 0) {
            shmctl(this->shmId, IPC_RMID, NULL);
        }
    } else {
        std::string mensaje = std::string("Error en shmdt(): ") + std::string(strerror(errno));
        Log::instance()->append(mensaje, Log::ERROR);
        std::cerr << mensaje << std::endl;
    }
}

template<class T>
MemoriaCompartida<T> &MemoriaCompartida<T>::operator=(const MemoriaCompartida &origen) {
    this->shmId = origen.shmId;
    void *tmpPtr = shmat(this->shmId, NULL, 0);

    if (tmpPtr != (void *) -1) {
        this->ptrDatos = static_cast<T *> (tmpPtr);
    } else {
        std::string mensaje = std::string("Error en shmat(): ") + std::string(strerror(errno));
        throw mensaje;
    }

    return *this;
}

template<class T>
void MemoriaCompartida<T>::escribir(const T &dato) {
    *(this->ptrDatos) = dato;
}

template<class T>
T MemoriaCompartida<T>::leer() const {
    return *(this->ptrDatos);
}

template<class T>
int MemoriaCompartida<T>::cantidadProcesosAdosados() const {
    shmid_ds estado;
    shmctl(this->shmId, IPC_STAT, &estado);
    return estado.shm_nattch;
}

#endif
