/*
 * Copyright 2012  Maciej Poleski
 */

#ifndef CLIENTREQUEST_H
#define CLIENTREQUEST_H

#include <memory>

#include <boost/asio/ip/tcp.hpp>

extern std::atomic_uint_fast32_t countOfConnectedClients;

/**
 * Obsługuje żądanie od klienta
 */
template<class Stream>
class ClientRequest final
{
public:
    ClientRequest() = delete;

    ~ClientRequest();
    /**
     * Inicjalizuje obiekt
     *
     * @param socket gniazdo z którego będzie odczytane żądanie i wysłana
     * odpowiedź
     */
    ClientRequest(std::shared_ptr<Stream> socket);

    /**
     * Wykonuje wszystkie działania związane z obsługą żądania
     */
    void dispatch(std::shared_ptr<ClientRequest<Stream>> handle) noexcept;

private:
    std::shared_ptr<Stream> _socket;
};


#include <mutex>

#include <boost/asio/ssl.hpp>

#include "../ConnCS/ClientToSlaveRequest.hxx"
#include "../ConnCS/ClientToSlaveReply.hxx"
#include "../shared/Request.hxx"
#include "Database.hxx"

template<class Stream>
ClientRequest<Stream>::ClientRequest(std::shared_ptr<Stream> socket) :
_socket(socket)
{
    ++countOfConnectedClients;
}

template<class Stream>
ClientRequest<Stream>::~ClientRequest()
{
    --countOfConnectedClients;
}

template<class Stream>
void ClientRequest<Stream>::dispatch(std::shared_ptr<ClientRequest<Stream>> handle) noexcept
{
    try
    {
        _socket->handshake(boost::asio::ssl::stream_base::server);
        ClientToSlaveRequest req=ClientToSlaveRequest::receiveFrom(*_socket);
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
        rep.sendTo(*_socket,req);
        _socket->shutdown();
    }
    catch(const std::exception &e)
    {
    }
    catch(...)
    {
        // ignore
    }
}


#endif // CLIENTREQUEST_H
