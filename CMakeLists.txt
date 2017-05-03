cmake_minimum_required(VERSION 3.5)
project(concudaring)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES
        src/logger/Log.cpp
        src/logger/Log.h
        src/shmem/MemoriaCompartida.h
        src/shmem/MemoriaCompartidaArray.h
        src/signals/EventHandler.h
        src/signals/SIGINT_Handler.h
        src/signals/SignalHandler.cpp
        src/signals/SignalHandler.h
        src/constantes.h
        src/Juego.cpp
        src/Juego.h
        src/semaphore/Semaphore.cpp src/semaphore/Semaphore.h src/shared_stack/SharedStack.cpp src/shared_stack/SharedStack.h src/lock_file/LockFile.cpp src/lock_file/LockFile.h)

add_executable(realiza_accion ${SOURCE_FILES} src/realiza_accion.cpp)


add_executable(tira_cartas ${SOURCE_FILES} src/tira_cartas.cpp)

add_executable(concudaring ${SOURCE_FILES} src/main.cpp)

add_executable(referi ${SOURCE_FILES} src/referi.cpp)

