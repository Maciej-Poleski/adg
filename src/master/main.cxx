#include <iostream>

#include <boost/asio.hpp>

#include "../ConnCM/ClientToMasterRequest.hxx"
#include "../ConnCM/ClientToMasterReply.hxx"

#include "../ConnMS/MasterToSlaveRequestNewDiscussion.hxx"

int main(int argc,char**argv)
{
    using namespace boost::asio;
    io_service ios;
    ip::tcp::socket sock(ios);
    sock.connect(ip::tcp::endpoint(ip::address::from_string("127.0.0.1"),8888));
    MasterToSlaveRequestNewDiscussion req;
    req.setId(22);
    //req.setName("");
    sendTo(req,sock);
    sock.close();
    return 0;
}
