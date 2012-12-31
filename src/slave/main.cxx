#include <iostream>
#include <fstream>
#include <memory>
#include <thread>
#include <atomic>
#include <string>
#include <chrono>

#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>

#include <boost/asio.hpp>

#include "Database.hxx"
#include "ClientRequest.hxx"

#include "../shared/Request.hxx"
#include "../ConnMS/MasterToSlaveRequestNewDiscussion.hxx"
#include "../ConnMS/MasterToSlaveReplyNewDiscussion.hxx"
#include "../ConnCS/ClientToSlaveRequest.hxx"
#include "../ConnCS/ClientToSlaveReply.hxx"

Database database;
static std::atomic_bool stopServer(false);
std::atomic_uint_fast32_t countOfConnectedClients(0);

static std::atomic<std::uint16_t> clientServerPort;
static std::atomic<std::uint16_t> masterServerPort;

void startClientServer(boost::asio::io_service &io)
{
    boost::asio::ip::tcp::endpoint endpoint(boost::asio::ip::tcp::v4(),clientServerPort);
    boost::asio::ip::tcp::acceptor acceptor(io,endpoint);
    while(!stopServer)
    {
        try
        {
            boost::asio::ip::tcp::socket socket(io);
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

void startMasterServer(boost::asio::io_service &io)
{
    boost::asio::ip::tcp::endpoint endpoint(boost::asio::ip::tcp::v4(),masterServerPort);
    boost::asio::ip::tcp::acceptor acceptor(io,endpoint);
    while(!stopServer)
    {
        try
        {
            boost::asio::ip::tcp::socket socket(io);
            acceptor.accept(socket);
            try
            {
                MasterToSlaveRequestNewDiscussion req=
                    receiveFrom<MasterToSlaveRequestNewDiscussion>(socket);

                database.createNewDiscussion(req.id(),req.name());

                MasterToSlaveReplyNewDiscussion rep(MasterToSlaveReplyNewDiscussion::ok);
                sendTo(rep,socket);
            }
            catch(...)
            {
                MasterToSlaveReplyNewDiscussion rep(MasterToSlaveReplyNewDiscussion::fail);
                sendTo(rep,socket);
            }
            socket.close();
        }
        catch(const std::exception &e)
        {
            // W przypadku tego protokołu komunikacyjnego takie błędy nie są
            // powodem do obaw.
            std::cerr<<e.what()<<'\n';
        }
    }
}

void startStopServer(boost::asio::io_service &io)
{
    std::string ignore;
    std::getline(std::cin,ignore);
    std::clog<<"Graceful stop...\n";
    stopServer=true;
    try
    {
        boost::asio::ip::tcp::endpoint cendpoint(
            boost::asio::ip::address::from_string("127.0.0.1"),
            clientServerPort);
        boost::asio::ip::tcp::socket csocket(io);
        csocket.connect(cendpoint); // Potrzebuję obiegu pętli akceptora
        csocket.close();
    }
    catch(...)
    {}
    try
    {
        boost::asio::ip::tcp::endpoint mendpoint(
            boost::asio::ip::address::from_string("127.0.0.1"),
            masterServerPort);
        boost::asio::ip::tcp::socket msocket(io);
        msocket.connect(mendpoint); // Potrzebuję obiegu pętli akceptora
        msocket.close();
    }
    catch(...)
    {}
}

int main(int argc,char**argv)
{
    if(argc!=3)
    {
        std::cerr<<argv[0]<<" [client port] [master port]\n";
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
    masterServerPort=std::stoul(argv[2]);
    boost::asio::io_service io_service;
    std::thread clientServerThread(startClientServer,std::ref(io_service));
    std::thread masterServerThread(startMasterServer,std::ref(io_service));
    startStopServer(io_service);
    masterServerThread.join();
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
