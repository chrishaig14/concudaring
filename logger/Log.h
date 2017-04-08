#ifndef LOG_H_
#define LOG_H_

#include <string>
#include <iostream>
#include <fstream>
#include <time.h>
#include <sys/types.h>
#include <unistd.h>

class Log {
public:

    typedef enum {
        INFO = 0, ERROR,
    } LOG_TYPE;

    static Log *instance();
    LOG_TYPE loggerLevel;
    void append(std::string msg, int owner, LOG_TYPE logType);
    void closeLog();


private:

    static Log *log;
    std::string logTypeToString(LOG_TYPE logType);
    void printNewLogger();
    std::string timestamp();
    std::string pid;
};

#endif /* LOG_H_ */
