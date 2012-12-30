#include <iostream>

#include <boost/asio.hpp>

#include "../ConnCS/ClientToSlaveRequest.hxx"
#include "../ConnCS/ClientToSlaveReply.hxx"

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
    ClientToSlaveReply rep;
    rep.addCommitedPostId(1);
    rep.addCommitedPostId(2);
    rep.addCommitedPostId(1);
    rep.addCommitedPostId(1);
    rep.addPreparedUpdate(2,{{1,Post()}});
    rep.addPreparedUpdate(4,{{1,Post()}});
    rep.addPreparedUpdate(4,{{1,Post()}});

    rep.sendTo(sock,req);
    sock.close();
    return 0;
}
