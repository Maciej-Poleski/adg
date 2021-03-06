/*
 * Copyright 2013  Maciej Poleski
 */

#include "Console.hxx"

#include <iostream>
#include <fstream>
#include <stdexcept>

#include <boost/algorithm/string/case_conv.hpp>

#include "Database.hxx"

void Console::executeFile(const std::string& param)
{
    std::ifstream in(param);
    if(!in.is_open())
        throw std::runtime_error("Can not open "+param);
    for(; !in.eof();)
    {
        std::string line;
        std::getline(in,line);
        execute(line);
    }
}

void Console::runInteractive()
{
    std::cout<<"You are in interactive mode.\n\nHint: type >>help<<\n";
    interactiveIsEnabled=true;
    for(; interactiveIsEnabled && !std::cin.eof();)
    {
        std::cout<<">>> ";
        std::string line;
        std::getline(std::cin,line);
        try
        {
            execute(line);
        }
        catch(const std::exception &e)
        {
            std::cout<<"An error occured: "<<e.what()<<'\n';
        }
    }
    interactiveIsEnabled=false;
}

std::function<void(const std::string&)>
Console::forward(
    const std::function<void(Console*,const std::string&)>& function)
{
    return std::bind(function,this,std::placeholders::_1);
}

static std::pair<std::string,std::string> splitCommand(const std::string& command)
{
    auto i=command.find(' ');
    std::string cmd=boost::algorithm::to_lower_copy(command.substr(0,i));
    std::string param;
    if(i!=std::string::npos)
        param=command.substr(i+1);
    // trim?
    return std::make_pair(cmd,param);
}

void Console::execute(const std::string& command)
{
    static const std::unordered_map<std::string,
           std::function<void(const std::string&)>>
    commandTable= {
        {"exit",forward(&Console::exit)},
        {"help",forward(&Console::help)},
        {".",forward(&Console::executeFile)},
        {"exec",forward(&Console::executeFile)},
        {"slave",forward(&Console::slave)},
    };
    if(command.empty())
        return;
    auto cmd=splitCommand(command);
    auto c=commandTable.find(cmd.first);
    if(c!=commandTable.end())
        c->second(cmd.second);
    else
    {
        std::cout<<"Unsupported command: "<<cmd.first<<'\n';
    }
}

void Console::exit(const std::string& )
{
    interactiveIsEnabled=false;
}

void Console::help(const std::string& )
{
    std::cout<<"slave add [ip] [client port] [master port]\n"
             <<"slave del [ip] [master port]\n"
             <<". [file name]\n"
             <<"exec [file name]\n"
             <<"help\n"
             <<"exit\n";
}

void Console::slave(const std::string& param)
{
    static const std::unordered_map<std::string,
           std::function<void(const std::string&)>>
    commandTable= {
        {"add",forward(&Console::slaveAdd)},
        {"del",forward(&Console::slaveDel)}
    };
    auto cmd=splitCommand(param);
    auto c=commandTable.find(cmd.first);
    if(c!=commandTable.end())
        c->second(cmd.second);
    else
    {
        std::cout<<"Unsupported command: slave "<<cmd.first<<'\n';
    }
}

void Console::slaveAdd(const std::string& param)
{
    std::string p1,p2,p3;
    {
        auto p=splitCommand(param);
        p1=p.first;
        p=splitCommand(p.second);
        p2=p.first;
        p=splitCommand(p.second);
        p3=p.first;
        if(!p.second.empty())
            throw std::runtime_error(p.second+" parameter is not expected");
    }
    std::uint32_t ip=boost::asio::ip::address::from_string(p1).to_v4().to_ulong();
    std::uint16_t clientPort=std::stoul(p2);
    std::uint16_t masterPort=std::stoul(p3);
    database.registerSlave({ip,clientPort},{ip,masterPort});
}

void Console::slaveDel(const std::string& param)
{
    std::cout<<"Sorry unimplemented (is it really what you want to do?)\n";
}
