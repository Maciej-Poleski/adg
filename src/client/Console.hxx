/*
 * Copyright 2013  Maciej Poleski
 */

#ifndef CONSOLE_H
#define CONSOLE_H

#include <string>
#include <unordered_map>
#include <functional>

/**
 * To nie jest część API
 */
class Console final
{

public:
    void executeFile(const std::string &param);
    void execute(const std::string &command);
    void runInteractive();

private:
    void exit(const std::string &);
    void help(const std::string &);
    void master(const std::string& param);
    void list(const std::string& param);
    void discussion(const std::string& param);
    void post(const std::string& param);
    void sync(const std::string& param);
    void print(const std::string& param);


    std::function<void(const std::string&)>
    forward(const std::function<void(Console*,const std::string&)> &function);

private:
    bool interactiveIsEnabled=false;

};

#endif // CONSOLE_H
