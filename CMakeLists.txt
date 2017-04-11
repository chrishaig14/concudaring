cmake_minimum_required(VERSION 3.7)
project(concudaring)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES
        src/logger/Log.cpp
        src/logger/Log.h
        src/shmem/MemoriaCompartida.h
        src/signals/EventHandler.h
        src/signals/SIGINT_Handler.h
        src/signals/SignalHandler.cpp
        src/signals/SignalHandler.h
        src/constantes.h
        src/Juego.cpp
        src/Juego.h
        src/main.cpp src/semaphore/Semaphore.cpp src/semaphore/Semaphore.h)

add_executable(concudaring ${SOURCE_FILES})