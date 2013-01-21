/*
 * Copyright 2012  Maciej Poleski
 */

#include "ClientRequest.hxx"

#include <mutex>

#include "../ConnCS/ClientToSlaveRequest.hxx"
#include "../ConnCS/ClientToSlaveReply.hxx"
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

void ClientRequest::dispatch(std::shared_ptr<ClientRequest> handle) noexcept
{
    try
    {
        ClientToSlaveRequest req=ClientToSlaveRequest::receiveFrom(_socket);
        ClientToSlaveReply rep;
        // commit
        for(const auto discussion : req.discussionsToCommit())
        {
            auto discussionId=discussion.first;
            auto result=database.addPostsToDiscussion(discussion.second,discussionId);
            for(const auto postId : result.second)
            {
                rep.addCommitedPostId(postId);
            }
        }
        // update
        for(const auto up : req.discussionsToUpdate())
        {
            auto preparedUpdate=database.prepareUpdate(up.first,up.second);
            rep.addPreparedUpdate(preparedUpdate.first,
                                  std::move(preparedUpdate.second));
        }
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
