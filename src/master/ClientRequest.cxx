/*
 * Copyright 2013  Maciej Poleski
 */

#include "ClientRequest.hxx"

#include <mutex>

#include "../ConnCM/ClientToMasterRequest.hxx"
#include "../ConnCM/ClientToMasterReply.hxx"
#include "../shared/Request.hxx"
#include "Database.hxx"

ClientRequest::ClientRequest(boost::asio::ip::tcp::socket&& socket) :
    _socket(std::move(socket))
{
    ++countOfConnectedClients;
}

ClientRequest::~ClientRequest()
{
    --countOfConnectedClients;
}

void ClientRequest::dispatch() noexcept
{
    try
    {
        ClientToMasterRequest req=ClientToMasterRequest::receiveFrom(_socket);
        std::vector<std::pair<DiscussionId,Address>> newDiscussions=
        database.createNewDiscussions(req.newDiscussions());
        ClientToMasterReply rep(database.currentDiscussionListVersion());
        // impl
        // send reply
        rep.sendTo(_socket,req);
        _socket.close();
    }
    catch(const std::exception &e)
    {
        std::clog<<e.what()<<'\n';
    }
    catch(...)
    {
        // ignore
    }
}