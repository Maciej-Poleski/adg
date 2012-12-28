#include <iostream>

#include <boost/asio.hpp>

#include "../ConnCM/ClientToMasterRequest.hxx"
#include "../ConnCM/ClientToMasterReply.hxx"

int main(int argc,char**argv)
{
    using namespace boost::asio;
    io_service ios;
    ip::tcp::acceptor acceptor(ios,ip::tcp::endpoint(ip::address::from_string("127.0.0.1"),8888));
    ip::tcp::socket sock(ios);
    acceptor.accept(sock);
    ClientToMasterRequest req=ClientToMasterRequest::receiveFrom(sock);
    std::cout<<req.discussionListVersion()<<'\n';
    for(auto a : req.newDiscussions())
        std::cout<<a<<'\n';
    for(auto a : req.discussionsToSynchronization())
        std::cout<<a<<'\n';
    ClientToMasterReply rep(9);
    rep.addNewDiscussion(1,{1,1});
    rep.addNewDiscussion(3,{3,33});
    rep.addNewDiscussion(4,{4,4444});
  //  rep.addNewDiscussionFromUpdate(2,"Dyskusja 2 z aktualizacji");
    rep.addDiscussionToSynchronization({12,1212});
    rep.addDiscussionToSynchronization({133,133});
    rep.sendTo(sock);
    sock.close();
    return 0;
}
