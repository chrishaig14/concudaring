#include "Log.h"
#include "../constantes.h"
#include "../lock_file/LockFile.h"

Log *Log::log = 0;

Log* Log::instance() {
    if (!log){
        log = new Log;
        log->pid = std::to_string(getpid());
    }

    return log;
}

void Log::closeLog() {
    if(log)
        delete log;

    log = nullptr;
}

void Log::append(std::string msg, LOG_TYPE logType) {
    LockFile lock(LOG_PATH);
    std::string fileName(LOG_PATH);
    lock.tomarLock();
    std::ofstream out(fileName, std::ios_base::app);

    if(logType >= loggerLevel)
        out << timestamp() + "::[" +  std::to_string(getpid()) + logTypeToString(logType) + msg + "\n";

    out.close();
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
