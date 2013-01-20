#include <iostream>
#include <fstream>

#include <boost/asio/ip/tcp.hpp>

#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>

#include "Database.hxx"
#include "Console.hxx"

boost::asio::io_service io_service;

int main(int argc,char**argv)
{
    {
        try
        {
            std::ifstream dbistream("database.bin");
            boost::archive::binary_iarchive dbiarchive(dbistream);
            dbiarchive>>database;
            dbistream.close();
        }
        catch(...)
        {
            std::clog<<"Assuming database is empty\n";
        }
    }
    std::cout<<"Going interactive mode...\n";
    Console console;
    console.runInteractive();
    {
        std::cout<<"Saving database...\n";
        std::ofstream dbostream("database.bin");
        boost::archive::binary_oarchive dboarchive(dbostream);
        dboarchive<<database;
        dbostream.close();
        std::cout<<"Done\n";
    }
    return 0;
}
