#include "Log.h"

Log *Log::log = 0;
const std::string fileout = "log-concudaring.txt";

Log* Log::instance() {
    if (!log){
        log = new Log;
        log->printNewLogger();
        log->pid = std::to_string(getpid());
    }

    return log;
}

void Log::closeLog() {
    if(log)
        delete log;

    log = nullptr;
}

void Log::append(std::string msg, int owner, LOG_TYPE logType) {

    std::string fileName(fileout);
    std::ofstream out(fileName, std::ios_base::app);

    if(logType >= loggerLevel)
        out << timestamp() + ":: pid: " + this->pid + " :: " + logTypeToString(logType) + msg + "\n";

    out.close();
}

std::string Log::timestamp() {
    time_t now = time(0);
    std::string dt = ctime(&now);
    return dt.substr(0, dt.size() - 1);
}

std::string Log::logTypeToString(LOG_TYPE logType){

    std::string strLogType;

    switch (logType) {
        case INFO: strLogType = ": INFO "; break;
        case ERROR: strLogType = ": ERROR "; break;
    }

    return strLogType;
}

void Log::printNewLogger(){

    std::string fileName(fileout);
    std::ofstream out(fileName, std::ios_base::app);

    out << "*&------------------------------------------------------------------------------------------------------*&\n";
    out << "*&                               Comienzo de nuevo juego                                                *&\n";
    out << "*&------------------------------------------------------------------------------------------------------*&\n";
    out << "*& Corrida correspondiente a la fecha: " + timestamp() + "                                         *&\n";
    out << "*&------------------------------------------------------------------------------------------------------*&\n";
    out.close();

}
