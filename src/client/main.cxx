#include <iostream>

#include <boost/asio.hpp>

#include "../ConnCM/ClientToMasterRequest.hxx"
#include "../ConnCM/ClientToMasterReply.hxx"
#include "../ConnCS/ClientToSlaveRequest.hxx"
#include "../ConnCS/ClientToSlaveReply.hxx"

int main(int argc,char**argv)
{
    using namespace boost::asio;
    io_service ios;
    ip::tcp::socket sock(ios);
    sock.connect(ip::tcp::endpoint(ip::address::from_string("127.0.0.1"),8888));
    /* ClientToMasterRequest req(7);
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
         std::cout<<o.ip<<' '<<o.port<<'\n'; */
    ClientToSlaveRequest req;
    req.addDiscussionToUpdate(1,1);
    req.addDiscussionToUpdate(2,2);
    req.addDiscussionToUpdate(3,33);
    Post p1;
    p1.setMessage("post 1");
    req.addPostToCommit(1,p1);
    Post p2,p3,p4;
    p2.setMessage("post 2");
    p3.setMessage("post 3");
    p4.setMessage("post 4");
    req.addPostToCommit(1,p2);
    req.addPostToCommit(2,p3);
    req.addPostToCommit(3,p4);
    req.sendTo(sock);
    ClientToSlaveReply rep=ClientToSlaveReply::receiveFrom(sock,req);
    for(auto o : rep.commited())
        std::cout<<o<<'\n';
    for(auto o : rep.updates())
    {
        std::cout<<' '<<o.first<<'\n';
        for(auto oo : o.second)
        {
            std::cout<<"   "<<oo.first<<' '<<oo.second.message()<<'\n';
        }
    }
    return 0;
}
