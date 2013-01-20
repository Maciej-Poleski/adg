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
        {"list",forward(&Console::list)},
        {"print",forward(&Console::print)},
        {"discussion",forward(&Console::discussion)},
        {"post",forward(&Console::post)},
        {".",forward(&Console::executeFile)},
        {"exec",forward(&Console::executeFile)},
        {"master",forward(&Console::master)},
        {"sync",forward(&Console::sync)},
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
    std::cout<<"master [ip] [port]\n"
             <<"sync\n"
             <<"list\n"
             <<"print [id]\n"
             <<"discussion [name]\n"
             <<"post [id] [message]\n"
             <<".\n"
             <<"exec\n"
             <<"help\n"
             <<"exit\n";
}

void Console::master(const std::string& param)
{
    std::string p1,p2;
    {
        auto p=splitCommand(param);
        p1=p.first;
        p=splitCommand(p.second);
        p2=p.first;
        if(!p.second.empty())
            throw std::runtime_error(p.second+" parameter is not expected");
    }
    std::uint32_t ip=boost::asio::ip::address::from_string(p1).to_v4().to_ulong();
    std::uint16_t port=std::stoul(p2);
    database.setMaster( {ip,port});
}

void Console::sync(const std::string& param)
{
    database.performSynchronization();
}

void Console::list(const std::string& param)
{
    database.listDiscussions(std::cout);
}

void Console::discussion(const std::string& param)
{
    std::string p1;
    {
        auto p=splitCommand(param);
        p1=p.first;
        if(!p.second.empty())
            throw std::runtime_error(p.second+" parameter is not expected");
    }
    database.createNewDiscussion(param);
}

void Console::post(const std::string& param)
{
    auto p=splitCommand(param);
    DiscussionId discussion=std::stoul(p.first);
    Post post;
    post.setMessage(p.second);
    database.addNewPostToDiscussion(post,discussion);
}

void Console::print(const std::string& param)
{
    std::string p1;
    {
        auto p=splitCommand(param);
        p1=p.first;
        if(!p.second.empty())
            throw std::runtime_error(p.second+" parameter is not expected");
    }
    database.printDiscussion(std::stoul(p1),std::cout);
}