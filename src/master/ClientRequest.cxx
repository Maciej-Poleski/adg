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

void ClientRequest::dispatch(std::shared_ptr< ClientRequest > handle) noexcept
{
    try
    {
        ClientToMasterRequest req=ClientToMasterRequest::receiveFrom(_socket);
        std::vector<std::pair<DiscussionId,Address>> newDiscussions=
        database.createNewDiscussions(req.newDiscussions());
        ClientToMasterReply rep(database.currentDiscussionListVersion());
        for(const auto nd : newDiscussions)
        {
            rep.addNewDiscussion(nd.first,nd.second);
        }
        {
            auto update=database.getUpdates(req.discussionListVersion());
            for(const auto u : update)
            {
                rep.addNewDiscussionFromUpdate(u.first,u.second);
            }
        }
        for(const auto d : req.discussionsToSynchronization())
        {
            rep.addDiscussionToSynchronization(database.getSlave(d));
        }
        rep.sendTo(_socket,req);
        _socket.close();
    }
    catch(const std::exception &e)
    {
    }
    catch(...)
    {
        // ignore
    }
}
