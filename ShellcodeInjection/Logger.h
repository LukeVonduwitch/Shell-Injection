#ifndef LOGGER_H
#define LOGGER_H

#include <stdexcept>
#include <windows.h>
#include <string>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <ctime>
#include <string>


class Logger {
public:
    enum Level {
        DEBUG,
        INFO,
        WARN,
        FAILURE,
        CRITICAL
    };

    static Logger& getInstance() {
        static Logger instance;
        return instance;
    }

    // Função para configurar o nível de log
    void setLevel(Level level) {
        logLevel = level;
    }

    // Sobrecarga do operador <<
    template <typename T>
    Logger& operator<<(const T& value) {
        buffer << value;
        return *this;
    }

    // Finalizar o log e imprimir no console
    void log(Level level) {
        if (level >= logLevel) {
            std::cout << "[" << getTimestamp() << "] "
                      << "[" << getLevelSymbol(level) << "] "
                      << buffer.str() << std::endl;
        }
        buffer.str("");
        buffer.clear();
    }

    // Métodos de log convenientes
    void logDebug()    { log(DEBUG); }
    void logInfo()     { log(INFO); }
    void logWarn()     { log(WARN); }
    void logFailure()  { log(FAILURE); }
    void logCritical() { log(CRITICAL); }

private:
    Logger() : logLevel(INFO) {}

    // Formatação de data e hora
    std::string getTimestamp() {
        std::time_t now = std::time(nullptr);
        std::tm* localtime = std::localtime(&now);

        std::ostringstream oss;
        oss << std::put_time(localtime, "%d-%m-%Y %H:%M:%S");
        return oss.str();
    }

    // Retorna o símbolo do nível de log
    char getLevelSymbol(Level level) {
        if (level == DEBUG) {
            return '*';
        } else if (level == INFO) {
            return '+';
        } else if (level == WARN) {
            return '-';
        } else if (level == FAILURE) {
            return '!';
        } else if (level == CRITICAL) {
            return '#';
        } else {
            return '?';
        }
    }

    Level logLevel;
    std::ostringstream buffer;
};

#endif // LOGGER_H
