#include <iostream>
#include <fstream>
#include <memory>
#include <thread>
#include <atomic>
#include <string>
#include <chrono>

#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>

#include <boost/asio/ip/tcp.hpp>

#include "../shared/Request.hxx"
#include "../ConnMS/MasterToSlaveRequestNewDiscussion.hxx"
#include "../ConnMS/MasterToSlaveReplyNewDiscussion.hxx"

#include "ClientRequest.hxx"
#include "Database.hxx"
#include "Console.hxx"

boost::asio::io_service io_service;

static std::atomic_bool stopServer(false);
std::atomic_uint_fast32_t countOfConnectedClients(0);

static std::atomic<std::uint16_t> clientServerPort;
// TODO slaveServerPort

void startClientServer()
{
    boost::asio::ip::tcp::endpoint endpoint(boost::asio::ip::tcp::v4(),clientServerPort);
    boost::asio::ip::tcp::acceptor acceptor(io_service,endpoint);
    while(!stopServer)
    {
        try
        {
            boost::asio::ip::tcp::socket socket(io_service);
            acceptor.accept(socket);
            try
            {
                ClientRequest dispatcher(std::move(socket));
                std::thread t(&ClientRequest::dispatch,std::ref(dispatcher));
                t.detach();
            }
            catch(...)
            {
                // ignore
            }
        }
        catch(const std::exception &e)
        {
            // W przypadku tego protokołu komunikacyjnego takie błędy nie są
            // powodem do obaw.
            std::cerr<<e.what()<<'\n';
        }
    }
}

void startStopServer(const std::string &param)
{
    Console console;
    if(!param.empty())
        console.executeFile(param);
    console.runInteractive();
    std::clog<<"Graceful stop...\nIgnore errors below\n";
    stopServer=true;
    try
    {
        boost::asio::ip::tcp::endpoint cendpoint(
            boost::asio::ip::address::from_string("127.0.0.1"),
            clientServerPort);
        boost::asio::ip::tcp::socket csocket(io_service);
        csocket.connect(cendpoint); // Potrzebuję obiegu pętli akceptora
        csocket.close();
    }
    catch(...)
    {}
}

int main(int argc,char**argv)
{
    if(argc!=2 && argc!=3)
    {
        std::cerr<<argv[0]<<" [client port] [[config]]\n";
        return 1;
    }
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
    clientServerPort=std::stoul(argv[1]);
    std::thread clientServerThread(startClientServer);
    startStopServer(argc>=3?argv[2]:"");
    clientServerThread.join();
    while(countOfConnectedClients!=0)
    {
        // Pół minuty na zakończenie istniejących połączeń.
        // To i tak za dużo
        static std::size_t t=30;
        std::this_thread::sleep_for(std::chrono::seconds(1));
        if(t--==0)
            break;
    }
    {
        std::ofstream dbostream("database.bin");
        boost::archive::binary_oarchive dboarchive(dbostream);
        dboarchive<<database;
        dbostream.close();
    }
    return 0;
}
