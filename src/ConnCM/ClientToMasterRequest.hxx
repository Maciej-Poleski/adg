/*
 * Maciej Poleski (C) 2012
 */

#ifndef CLIENTTOMASTERREQUEST_H
#define CLIENTTOMASTERREQUEST_H

#include <cstdint>
#include <vector>
#include <string>

#include <boost/asio/ip/tcp.hpp>

#include "../shared/Request.hxx"

/**
 * Żądanie klienta do Mastera.
 * Obiekt tej klasy służy do konstrukcji żądania, przesłania go do Mastera
 * oraz jego odebrania i przeanalizowania przez Mastera.
 */
class ClientToMasterRequest final
{
public:
    /**
     * @param version numer wersji listy dyskusyjnej posiadanej
     *          przez klienta
     */
    ClientToMasterRequest(DiscussionListVersion version);

    /**
     * Odbiera żądanie od klienta i tworzy na jego podstawie obiekt
     *
     * @param socket gniazdo z którego obiekt będzie odbierany
     * @return Odebrane żądanie
     */
    static ClientToMasterRequest receiveFrom(boost::asio::ip::tcp::socket &socket);

    /**
     * Dodaje żądanie utworzenia nowej dyskusji o podanej nazwie
     *
     * @param discussion nazwa nowej dyskusji do utworzenia
     * @return ID podżądania
     */
    std::uint32_t addNewDiscussion(const std::string &discussion);

    /**
     * Dodaje żądanie synchronizacji dyskusji o podanym ID
     *
     * @param discussionId ID dyskusji do synchronizacji
     * @return ID podżądania
     */
    std::uint32_t addDiscussionToSynchronize(std::uint32_t discussionId);

    /**
     * Wysyła żądanie do Mastera
     *
     * @param socket gniazdo na które żądanie zostanie wysłane
     */
    void sendTo(boost::asio::ip::tcp::socket &socket) const;

    /**
     * @return numer wersji listy dyskusji po stronie klienta
     */
    DiscussionListVersion discussionListVersion() const;

    /**
     * @return Lista nowych dyskusji do utworzenia
     */
    const std::vector< std::string >& newDiscussions() const;

    /**
     * @return Lista dyskusji do synchronizacji
     */
    const std::vector< DiscussionId >& discussionsToSynchronization() const;

private:
    ClientToMasterRequest();

private:
    /// Lista nowych dyskusji które klient chce utworzyć
    std::vector<std::string> _newDiscussions;
    /// Wersja listy dyskusji po stronie klienta
    DiscussionListVersion _discussionListVersion;
    /// Lista identyfikatorów dyskusji które klient chce zsynchronizować
    std::vector<DiscussionId> _discussionsToSynchronization;

};

#endif // CLIENTTOMASTERREQUEST_H
