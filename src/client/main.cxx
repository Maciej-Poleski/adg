#include <iostream>

#include <boost/asio.hpp>

#include "../ConnCM/ClientToMasterRequest.hxx"
#include "../ConnCM/ClientToMasterReply.hxx"

int main(int argc,char**argv)
{
    using namespace boost::asio;
    io_service ios;
    ip::tcp::socket sock(ios);
    sock.connect(ip::tcp::endpoint(ip::address::from_string("127.0.0.1"),8888));
    ClientToMasterRequest req(7);
    req.addNewDiscussion("Pierwsza utworzona dyskusja");
    req.addNewDiscussion("Druga utworzona dyskusja");
    req.addNewDiscussion("Trzecia utworzona dyskusja");
    req.addDiscussionToSynchronize(12);
    req.addDiscussionToSynchronize(133);
    req.sendTo(sock);
    ClientToMasterReply rep=ClientToMasterReply::receiveFrom(sock,req);
    std::cout<<"Wersja listy po stronie servera: "<<rep.discussionListVersion()<<'\n';
    for(auto o : rep.newDiscussions())
        std::cout<<o.first<<' '<<o.second.ip<<' '<<o.second.port<<'\n';
    for(auto o : rep.newDiscussionsFromUpdate())
        std::cout<<o.first<<' '<<o.second<<'\n';
    for(auto o : rep.discussionsToSynchronization())
        std::cout<<o.ip<<' '<<o.port<<'\n';
    return 0;
}
