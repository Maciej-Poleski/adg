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
class ClientToSlaveRequest
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
    static ClientToSlaveRequest receiveFrom(boost::asio::ip::tcp::socket &socket);

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
    void sendTo(boost::asio::ip::tcp::socket &socket) const;

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

#endif // CLIENTTOSLAVEREQUEST_H
