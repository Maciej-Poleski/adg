/*
 * Copyright 2013  Maciej Poleski <maciej.poleski@uj.edu.pl>
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
     * @param socket strumień z którego będzie odczytane żądanie i wysłana
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

#include "../ConnCM/ClientToMasterRequest.hxx"
#include "../ConnCM/ClientToMasterReply.hxx"
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
void ClientRequest<Stream>::dispatch(std::shared_ptr< ClientRequest<Stream> > handle) noexcept
{
    try
    {
        _socket->handshake(boost::asio::ssl::stream_base::server);
        ClientToMasterRequest req=ClientToMasterRequest::receiveFrom(*_socket);
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
