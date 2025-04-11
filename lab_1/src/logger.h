#pragma once
#include <string>
#include <fstream>
#include <iostream>

// ANSI color codes for terminal output
namespace Color {
    const std::string RESET   = "\033[0m";
    const std::string RED     = "\033[31m";
    const std::string GREEN   = "\033[32m";
    const std::string BLUE    = "\033[34m";
    const std::string YELLOW  = "\033[33m";
}

class Logger {
public:
    static void logGeneration(std::ofstream& file, int requestId, const std::string& fuelType, const std::string& timestamp) {
        std::string prefix = "[GEN]";
        std::string msg = " Запрос " + std::to_string(requestId) + 
                         " сгенерирован " + fuelType + 
                         " в " + timestamp;
        
        file << prefix << msg << std::endl;
        
        #ifdef DEBUG
        std::cout << Color::GREEN << prefix << Color::RESET << msg << std::endl;
        #endif
    }
    
    static void logService(std::ofstream& file, int stationId, int requestId, 
                          const std::string& fuelType, const std::string& timestamp) {
        std::string prefix = "[SERV]";
        std::string msg = " Запрос " + std::to_string(requestId) + 
                         " обслужен станцией " + std::to_string(stationId) +
                         " (тип топлива: " + fuelType + ") в " + timestamp;
        
        file << prefix << msg << std::endl;
        
        #ifdef DEBUG
        std::cout << Color::YELLOW << prefix << Color::RESET << msg << std::endl;
        #endif
    }
    
    static void logQueueRemoval(std::ofstream& file, int stationId, int requestId, 
                               const std::string& fuelType, int queueSize, const std::string& timestamp) {
        std::string prefix = "[QUEUE]";
        std::string msg = " Запрос " + std::to_string(requestId) + 
                         " убран станцией " + std::to_string(stationId) +
                         " (тип топлива: " + fuelType + ") из очереди. " +
                         "(" + std::to_string(queueSize) + ")" +
                         " в " + timestamp;
        
        file << prefix << msg << std::endl;
        
        #ifdef DEBUG
        std::cout << Color::BLUE << prefix << Color::RESET << msg << std::endl;
        #endif
    }
    
    static void logRejected(std::ofstream& file, int requestId, 
                           const std::string& fuelType, int queueSize, 
                           const std::string& timestamp) {
        std::string prefix = "[REJECT]";
        std::string msg = " Запрос " + std::to_string(requestId) + 
                         " отклонен (тип топлива: " + fuelType + "). " +
                         "Очередь - переполнена(" + std::to_string(queueSize) + ") " +
                         "в " + timestamp;
        
        file << prefix << msg << std::endl;
        
        #ifdef DEBUG
        std::cout << Color::RED << prefix << Color::RESET << msg << std::endl;
        #endif
    }
};