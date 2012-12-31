#include <iostream>

#include <boost/asio.hpp>

#include "../ConnCS/ClientToSlaveRequest.hxx"
#include "../ConnCS/ClientToSlaveReply.hxx"

#include "../ConnMS/MasterToSlaveRequestNewDiscussion.hxx"
#include "../ConnMS/MasterToSlaveReplyNewDiscussion.hxx"

int main(int argc,char**argv)
{
    using namespace boost::asio;
    io_service ios;
    ip::tcp::acceptor acceptor(ios,ip::tcp::endpoint(ip::address::from_string("127.0.0.1"),8888));
    ip::tcp::socket sock(ios);
    acceptor.accept(sock);
    MasterToSlaveRequestNewDiscussion req=receiveFrom<MasterToSlaveRequestNewDiscussion>(sock);
    std::cout<<req.id()<<' '<<req.name()<<'\n';
    MasterToSlaveReplyNewDiscussion rep(MasterToSlaveReplyNewDiscussion::fail);
    sendTo(rep,sock);
    sock.close();
    return 0;
}
