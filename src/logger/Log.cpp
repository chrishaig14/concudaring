#include <iostream>
#include <sstream>
#include "Log.h"
#include "../constantes.h"
#include "../lock_file/LockFile.h"

Log *Log::log = 0;

Log* Log::instance() {
    if (!log){
        log = new Log;
    }
    return log;
}

void Log::append(std::string msg, LOG_TYPE logType) {
    LockFile lock(LOG_PATH);
    std::string fileName(LOG_PATH);
    lock.tomarLock();
    std::ostringstream out;

    if(logType >= loggerLevel) {
        out << timestamp() + "::[" +  std::to_string(getpid()) + logTypeToString(logType) + msg + "\n";
    }
    ssize_t escrito = lock.escribir(out.str().c_str(), out.str().length());

    if (escrito != out.str().length())
        std::cerr << "Ha ocurrido un error al escribir en el archivo de log." << std::endl;
    
    lock.liberarLock();
}

std::string Log::timestamp() {
    time_t now = time(0);
    std::string dt = ctime(&now);
    return dt.substr(0, dt.size() - 1);
}

std::string Log::logTypeToString(LOG_TYPE logType){

    std::string strLogType;

    switch (logType) {
        case DEBUG: strLogType = "]::DEBUG::"; break;
        case ERROR: strLogType = "]::ERROR::"; break;
    }

    return strLogType;
}

void Log::printNewLogger(){

    std::string fileName(LOG_PATH);
    std::ofstream out(fileName, std::ios_base::app);

    out << "*&------------------------------------------------------------------------------------------------------*&\n";
    out << "*&                                       Comienzo de nuevo juego                                        *&\n";
    out << "*&------------------------------------------------------------------------------------------------------*&\n";
    out << "*&                   Corrida correspondiente a la fecha: " + timestamp() + "                       *&\n";
    out << "*&------------------------------------------------------------------------------------------------------*&\n";
    out.close();

}
