/*
 * Copyright 2012  Maciej Poleski
 */

#ifndef CLIENTTOSLAVEREQUEST_H
#define CLIENTTOSLAVEREQUEST_H

#include <vector>
#include <utility>
#include <cstdint>

#include <boost/asio/ip/tcp.hpp>

#include "Post.hxx"

#include "../shared/Request.hxx"

/**
 * Przechowuje żądanie klienta da Slave. Pozwala na jego wysłanie oraz
 * odebranie.
 */
class ClientToSlaveRequest final
{
public:
    /**
     * Tworzy pusty obiekt żądania
     */
    ClientToSlaveRequest();

    /**
     * Odbiera żądanie
     *
     * @param socket gniazdo z którego zostanie odebrane żądanie
     * @return odebrane żądanie
     */
    template<class SyncReadStream>
    static ClientToSlaveRequest receiveFrom(SyncReadStream &socket);

    /**
     * Dodaje dyskusje do aktualizacji
     *
     * @param id ID dyskusji do aktualizacji
     * @param version lokalna wersja dyskusji o podanym ID
     */
    void addDiscussionToUpdate(std::uint32_t id,std::uint32_t version);

    /**
     * Dodaje wiadomość do wysłania na server
     *
     * @param discussionID ID dyskusji w której została utworzona wiadomość
     * @param post wiadomość która zostanie wysłana na serwer
     */
    void addPostToCommit(std::uint32_t discussionID,const Post &post);

    /**
     * Wysyła żądanie
     *
     * @param socket gniazdo na które zostanie wysłane żądanie
     */
    template<class SyncWriteStream>
    void sendTo(SyncWriteStream &socket) const;

    /**
     * @return lista dyskusji wraz z numerem wersji do aktualizacji
     */
    const std::vector<std::pair<DiscussionId,DiscussionVersion>>&
            discussionsToUpdate() const;

    /**
     * @return lista dyskusji wraz z wiadomościami do przesłania na serwer
     */
    const std::vector<std::pair<DiscussionId,std::vector<Post>>> &
    discussionsToCommit() const;

private:
    std::vector<std::pair<std::uint32_t,std::uint32_t>> _discussionsToUpdate;
    std::vector<std::pair<std::uint32_t,std::vector<Post>>> _discussionsToCommit;

};

template<class SyncWriteStream>
void ClientToSlaveRequest::sendTo(SyncWriteStream& socket) const
{
    using namespace detail;
    {
        byte v[1]= {0};
        boost::asio::write(socket,boost::asio::buffer(v),
                           boost::asio::transfer_exactly(1));
    }
    writeToSocket(static_cast<std::uint32_t>(_discussionsToUpdate.size()),
                  socket);
    for(const auto o : _discussionsToUpdate)
    {
        writeToSocket(o.first,socket);
        writeToSocket(o.second,socket);
    }
    writeToSocket(static_cast<std::uint32_t>(_discussionsToCommit.size()),
                  socket);
    for(const auto o : _discussionsToCommit)
    {
        writeToSocket(o.first,socket);
        writeToSocket(static_cast<std::uint32_t>(o.second.size()),socket);
        for(const auto oo : o.second)
        {
            writeToSocket(oo,socket);
        }
    }
}

template<class SyncReadStream>
ClientToSlaveRequest ClientToSlaveRequest::receiveFrom(
    SyncReadStream& socket)
{
    using namespace detail;
    {
        byte v[1];
        boost::asio::read(socket,boost::asio::buffer(v),
                          boost::asio::transfer_exactly(1));
        if(*v!=0)
            throw std::runtime_error("Client requested unsupported protocol");
    }
    ClientToSlaveRequest result;
    try
    {
        {
            auto count=readFromSocket< std::uint32_t >(socket);
            for(std::size_t i=0; i<count; ++i)
            {
                auto id=readFromSocket< std::uint32_t >(socket);
                auto version=readFromSocket< std::uint32_t >(socket);
                result.addDiscussionToUpdate(id,version);
            }
        }
        {
            auto count=readFromSocket< std::uint32_t >(socket);
            for(std::size_t i=0; i<count; ++i)
            {
                auto id=readFromSocket< std::uint32_t >(socket);
                checkDiscussionId(id);
                auto c=readFromSocket< std::uint32_t >(socket);
                if(c==0)
                {
                    throw std::runtime_error("Client commited 0 new posts."
                                             " This is forbidden."
                                            );
                }
                for(std::size_t i=0; i<c; ++i)
                {
                    auto post=readFromSocket<Post>(socket);
                    post.check();
                    result.addPostToCommit(id,post);
                }
            }
        }
    }
    catch(std::logic_error e)
    {
        throw std::runtime_error(std::string("Received malformed object: ")+
                                 e.what());
    }
    return result;
}


#endif // CLIENTTOSLAVEREQUEST_H
