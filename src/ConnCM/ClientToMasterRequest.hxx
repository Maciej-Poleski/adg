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
    template<class SyncReadStream>
    static ClientToMasterRequest receiveFrom(SyncReadStream &socket);

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
    template<class SyncWriteStream>
    void sendTo(SyncWriteStream &socket) const;

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

template<class SyncWriteStream>
void ClientToMasterRequest::sendTo(SyncWriteStream& socket) const
{
    using namespace detail;
    byte version[]= {1};
    boost::asio::write(socket,boost::asio::buffer(version));
    writeToSocket(static_cast<std::uint32_t>(_newDiscussions.size()),socket);
    for(const auto s : _newDiscussions)
    {
        writeToSocket(s,socket);
    }
    writeToSocket(_discussionListVersion,socket);
    writeToSocket(static_cast<std::uint32_t>
                  (_discussionsToSynchronization.size()),
                  socket);
    for(const auto d : _discussionsToSynchronization)
    {
        writeToSocket(d,socket);
    }
}

template<class SyncReadStream>
ClientToMasterRequest ClientToMasterRequest::receiveFrom(
    SyncReadStream& socket)
{
    using namespace detail;
    ClientToMasterRequest result;
    byte version[1];
    boost::asio::read(socket,boost::asio::buffer(version));
    if(*version!=1)
        throw std::runtime_error("Client try to use unsupported protocol"
                                 "version "+std::to_string(*version));
    auto newDiscussionsSize=readFromSocket<std::uint32_t>(socket);
    for(decltype(newDiscussionsSize) i=0; i<newDiscussionsSize; ++i)
    {
        auto && o=readFromSocket<std::string>(socket);
        if(o.empty())
            throw std::runtime_error("Received empty discussion name. This"
                                     "is forbidden."
                                    );
        result._newDiscussions.push_back(std::move(o));
    }
    result._discussionListVersion=readFromSocket<std::uint32_t>(socket);
    if(result._discussionListVersion==0)
        throw std::runtime_error("Received 0 as version of discussion list."
                                 "This is forbidden.");
    auto discussionsToSync=readFromSocket<std::uint32_t>(socket);
    for(decltype(discussionsToSync) i=0; i<discussionsToSync; ++i)
    {
        auto o=readFromSocket<std::uint32_t>(socket);
        if(o==0)
            throw std::runtime_error("Received 0 as discussion ID. This is"
                                     "forbidden");
        result._discussionsToSynchronization.push_back(o);
    }
    return result;
}



#endif // CLIENTTOMASTERREQUEST_H
