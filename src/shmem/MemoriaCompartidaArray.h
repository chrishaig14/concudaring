#ifndef MEMORIACOMPARTIDA_ARRAY_H_
#define MEMORIACOMPARTIDA_ARRAY_H_

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <string>
#include <string.h>
#include <iostream>
#include <errno.h>

template<class T>
class MemoriaCompartidaArray {

private:
    int shmId;
    T *ptrDatos;
    size_t size;

    int cantidadProcesosAdosados() const;

public:
    MemoriaCompartidaArray();

    void crear(const std::string &archivo, const char letra, size_t size);

    void liberar();

    MemoriaCompartidaArray(const std::string &archivo, const char letra, size_t size);

    MemoriaCompartidaArray(const MemoriaCompartidaArray &origen);

    ~MemoriaCompartidaArray();

    T &operator[](size_t i);

    MemoriaCompartidaArray<T> &operator=(const MemoriaCompartidaArray &origen);

    void escribir(const T &dato);

    T leer() const;
};


template<class T>
MemoriaCompartidaArray<T>::MemoriaCompartidaArray():shmId(0), ptrDatos(NULL) {
}

template<class T>
void MemoriaCompartidaArray<T>::crear(const std::string &archivo, const char letra, size_t size) {
    this->size = size;


    key_t clave = ftok(archivo.c_str(), letra);

    if (clave > 0) {
        this->shmId = shmget(clave, sizeof(T) * size, 0644 | IPC_CREAT);

        if (this->shmId > 0) {
            void *tmpPtr = shmat(this->shmId, NULL, 0);
            if (tmpPtr != (void *) -1) {
                this->
                        ptrDatos = static_cast<T *> (tmpPtr);
            } else {
                std::string mensaje = std::string("Error en shmat(): ") + std::string(strerror(errno));

//                Log::instance()->
//
//                        append(mensaje, getpid(), Log::ERROR
//                );
                throw
                        mensaje;
            }
        } else {
            std::string mensaje = std::string("Error en shmget(): ") + std::string(strerror(errno));

//            Log::instance()->
//
//                    append(mensaje, getpid(), Log::ERROR
//            );
            throw
                    mensaje;
        }
    } else {
        std::string mensaje = std::string("Error en ftok(): ") + std::string(strerror(errno));

//        Log::instance()->
//
//                append(mensaje, getpid(), Log::ERROR
//        );
        throw
                mensaje;
    }
}

template<class T>
void MemoriaCompartidaArray<T>::liberar() {
    int errorDt = shmdt((void *) this->ptrDatos);

    if (errorDt != -1) {
        int procAdosados = this->cantidadProcesosAdosados();
        if (procAdosados == 0) {
            shmctl(this->shmId, IPC_RMID, NULL);
        }
    } else {
        std::string mensaje = std::string("Error en shmdt(): ") + std::string(strerror(errno));
//        Log::instance()->append(mensaje, getpid(), Log::ERROR);
        throw mensaje;
    }
}

template<class T>
MemoriaCompartidaArray<T>::MemoriaCompartidaArray(const std::string &archivo, const char letra, size_t size):shmId(0),
                                                                                                             ptrDatos(
                                                                                                                     NULL) {
    key_t clave = ftok(archivo.c_str(), letra);
    this->size = size;
    if (clave > 0) {
        this->shmId = shmget(clave, sizeof(T) * size, 0644 | IPC_CREAT);

        if (this->shmId > 0) {
            void *tmpPtr = shmat(this->shmId, NULL, 0);
            if (tmpPtr != (void *) -1) {
                this->ptrDatos = static_cast<T *> (tmpPtr);
            } else {
                std::string mensaje = std::string("Error en shmat(): ") + std::string(strerror(errno));
//                Log::instance()->append(mensaje, getpid(), Log::ERROR);
                throw mensaje;
            }
        } else {
            std::string mensaje = std::string("Error en shmget(): ") + std::string(strerror(errno));
//            Log::instance()->append(mensaje, getpid(), Log::ERROR);
            throw mensaje;
        }
    } else {
        std::string mensaje = std::string("Error en ftok(): ") + std::string(strerror(errno));
//        Log::instance()->append(mensaje, getpid(), Log::ERROR);
        throw mensaje;
    }
}

template<class T>
MemoriaCompartidaArray<T>::MemoriaCompartidaArray(const MemoriaCompartidaArray &origen):shmId(origen.shmId) {
    void *tmpPtr = shmat(origen.shmId, NULL, 0);

    if (tmpPtr != (void *) -1) {
        this->ptrDatos = static_cast<T *> (tmpPtr);
    } else {
        std::string mensaje = std::string("Error en shmat(): ") + std::string(strerror(errno));
//        Log::instance()->append(mensaje, getpid(), Log::ERROR);
        throw mensaje;
    }
}

template<class T>
MemoriaCompartidaArray<T>::~MemoriaCompartidaArray() {
    int errorDt = shmdt(static_cast<void *> (this->ptrDatos));

    if (errorDt != -1) {
        int procAdosados = this->cantidadProcesosAdosados();
        if (procAdosados == 0) {
            shmctl(this->shmId, IPC_RMID, NULL);
        }
    } else {
        std::string mensaje = std::string("Error en shmdt(): ") + std::string(strerror(errno));
//        Log::instance()->append(mensaje, getpid(), Log::ERROR);
        std::cerr << mensaje << std::endl;
    }
}

template<class T>
MemoriaCompartidaArray<T> &MemoriaCompartidaArray<T>::operator=(const MemoriaCompartidaArray &origen) {
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
void MemoriaCompartidaArray<T>::escribir(const T &dato) {
    *(this->ptrDatos) = dato;
}

template<class T>
T &MemoriaCompartidaArray<T>::operator[](size_t i) {
    if (i >= size) {
        // ERROR, index out of range
    }
    return this->ptrDatos[i];
}

template<class T>
T MemoriaCompartidaArray<T>::leer() const {
    return *(this->ptrDatos);
}

template<class T>
int MemoriaCompartidaArray<T>::cantidadProcesosAdosados() const {
    shmid_ds estado;
    shmctl(this->shmId, IPC_STAT, &estado);
    return estado.shm_nattch;
}

#endif
