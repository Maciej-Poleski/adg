#include <iostream>

#include <boost/asio.hpp>

#include "../ConnCS/ClientToSlaveRequest.hxx"

int main(int argc,char**argv)
{
    using namespace boost::asio;
    io_service ios;
    ip::tcp::acceptor acceptor(ios,ip::tcp::endpoint(ip::address::from_string("127.0.0.1"),8888));
    ip::tcp::socket sock(ios);
    acceptor.accept(sock);
    ClientToSlaveRequest req=ClientToSlaveRequest::receiveFrom(sock);
    for(auto a : req.discussionsToUpdate())
        std::cout<<a.first<<' '<<a.second<<'\n';
    for(auto a : req.discussionsToCommit())
    {
        std::cout<<a.first<<'\n';
        for(auto b : a.second)
            std::cout<<' '<<b.message()<<'\n';
    }
    return 0;
}
