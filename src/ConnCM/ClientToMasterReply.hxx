/*
 * Copyright 2012  Maciej Poleski
 */

#ifndef CLIENTTOMASTERREPLY_H
#define CLIENTTOMASTERREPLY_H

#include <vector>
#include <string>
#include <cstdint>
#include <utility>

#include <boost/asio/ip/tcp.hpp>

#include "../shared/Address.hxx"
#include "../shared/Request.hxx"

class ClientToMasterRequest;
/**
 * Odpowiedź na żądanie klienta do Mastera.
 * Obiekt tej klasy służy do konstrukcji odpowiedzi, przesłania jej do klienta
 * oraz jej odebrania i przeanalizowania przez klienta.
 */
class ClientToMasterReply final
{
public:
    /**
     * Tworzy odpowiedź o zadanej wersji listy dyskusji
     *
     * @param version wersja listy dyskusji po stronie Mastera
     */
    ClientToMasterReply(DiscussionListVersion version);

    /**
     * Odbiera odpowiedź na podane zapytanie od Mastera i tworzy na jej
     * podstawie obiekt tej klasy
     *
     * @param socket strumień z którego będzie odebrana odpowiedź
     * @param request zapytanie na które będzie odbierana odpowiedź
     * @return odpowiedź mastera w postaci obiektu
     */
    template<class SyncReadStream>
    static ClientToMasterReply receiveFrom(SyncReadStream &socket,
                                           const ClientToMasterRequest &request);

    /**
     * Przydziela id oraz slave do kolejnej nowej dyskusji
     *
     * @param id ID nowo utworzonej dyskusji lub 0 w razie niepowodzenia
     * @param address w razie powodzenia adres slave odpowiedzialnego za
     *          nowo utworzoną dyskusje
     */
    void addNewDiscussion(DiscussionId id,const Address &address=Address());

    /**
     * Dodaje informacje o nowej dyskusji pochodzącej (z punktu widzenia klienta)
     * z aktualizacji
     *
     * @param id ID nowej dyskusji
     * @param name nazwa nowej dyskusji
     */
    void addNewDiscussionFromUpdate(DiscussionId id, const std::string &name);

    /**
     * Dodaje informacje o slave odpowiedzialnym za odpowiednią dyskusje
     * z żądania synchronizacji klienta
     *
     * @param address adres slave odpowiedzialnego za odpowiadającą
     *          dyskusje z żądania synchronizacji lub 0 jeżeli nie udało się
     *          go uzyskać
     */
    void addDiscussionToSynchronization(const Address& address=Address());

    /**
     * @return lista identyfikatorów kolejnych nowo utworzonych dyskusji
     *          (lub 0) wraz z adresami slave odpowiedzialnymi za nie
     */
    const std::vector<std::pair<DiscussionId,Address>> & newDiscussions() const;

    /**
     * @return numer wersji listy dyskusji w tej odpowiedzi
     */
    const DiscussionListVersion discussionListVersion() const;

    /**
     * @return lista identyfikatorów wraz z nazwami dyskusji będących dla
     *          klienta aktualizacją jego listy do wersji podanej w tej
     *          odpowiedzi
     */
    const std::vector<std::pair<DiscussionId,std::string>> &
            newDiscussionsFromUpdate() const;

    /**
     * @return lista slave odpowiedzialnych za odpowiednie dyskusje z żądania
     *          klienta (lub 0 jeżeli nie udał się zdobyć tej informacji)
     */
    const std::vector<Address>& discussionsToSynchronization() const;

    /**
     * Wysyła tą odpowiedź
     *
     * @param socket strumień na który zostanie wysłana ta odpowiedź
     * @param request żądanie na które zostanie wysłana ta odpowiedź
     */
    template<class SyncWriteStream>
    void sendTo(SyncWriteStream &socket,
                const ClientToMasterRequest& request) const;

    /**
     * Wysyła informację o użyciu przez klienta nieobsługiwanej wersji
     * protokołu
     *
     * @param socket strumień na który zostanie wysłana informacja
     */
    template<class SyncWriteStream>
    static void sendUnsupportedTo(SyncWriteStream &socket);

private:
    ClientToMasterReply();

private:
    std::vector<std::pair<DiscussionId,Address>> _newDiscussions;
    DiscussionListVersion _discussionListVersion;
    std::vector<std::pair<DiscussionId,std::string>> _newDiscussionsFromUpdate;
    std::vector<Address> _discussionsToSynchronization;

};

#include "ClientToMasterRequest.hxx"

template<class SyncWriteStream>
void ClientToMasterReply::sendTo(SyncWriteStream& socket,
                                 const ClientToMasterRequest& request) const
{
    using namespace detail;
    {
        byte rep[1]= {0};
        boost::asio::write(socket,boost::asio::buffer(rep));
    }
    for(const auto o : _newDiscussions)
    {
        writeToSocket(o.first,socket);
        if(o.first!=0)
            writeToSocket(o.second,socket);
    }
    writeToSocket(_discussionListVersion,socket);
    if(_discussionListVersion>request.discussionListVersion())
    {
        writeToSocket(static_cast<std::uint32_t>(_newDiscussionsFromUpdate.size()),
                      socket);
        for(const auto o : _newDiscussionsFromUpdate)
        {
            writeToSocket(o.first,socket);
            writeToSocket(o.second,socket);
        }
    }
    else if(_discussionListVersion==request.discussionListVersion())
    {
        // nothing to do
    }
    else
    {
        throw std::logic_error("Client has incorrect version of discussions"
                               "list");
    }
    for(const auto o : _discussionsToSynchronization)
    {
        writeToSocket(o,socket);
    }
}

template<class SyncWriteStream>
void ClientToMasterReply::sendUnsupportedTo(SyncWriteStream& socket)
{
    using namespace detail;
    byte rep[1]= {1};
    boost::asio::write(socket,boost::asio::buffer(rep));
}

template<class SyncReadStream>
ClientToMasterReply ClientToMasterReply::receiveFrom(
    SyncReadStream& socket,
    const ClientToMasterRequest& request)
{
    using namespace detail;
    {
        byte rep[1];
        boost::asio::read(socket,boost::asio::buffer(rep),
                          boost::asio::transfer_exactly(1));
        if(*rep!=0)
            throw std::runtime_error("Client requested unsupported protocol version");
    }
    ClientToMasterReply result;
    for(std::size_t i=0; i<request.newDiscussions().size(); ++i)
    {
        auto a=readFromSocket<std::uint32_t>(socket);
        auto b=Address();
        if(a!=0)
        {
            b=readFromSocket<Address>(socket);
        }
        result._newDiscussions.push_back(std::make_pair(a,b));
    }
    result._discussionListVersion=readFromSocket< std::uint32_t >(socket);
    if(result._discussionListVersion==0)
    {
        throw std::runtime_error("Received 0 as version of discussion list."
                                 "This is forbidden.");
    }
    else if(result._discussionListVersion<request.discussionListVersion())
    {
        throw std::runtime_error(
            "Received "+std::to_string(result._discussionListVersion)+
            " version, but we have "+std::to_string(request.discussionListVersion())+
            ". It is impossible to revert version of discussion"
            "list on the server side so this reply is incorrect"
        );
    }
    else if(result._discussionListVersion==request.discussionListVersion())
    {
        // We are up to date
    }
    else
    {
        auto newDiscussionsFromUpdateCount=readFromSocket< std::uint32_t >(socket);
        if(newDiscussionsFromUpdateCount==0)
            throw std::runtime_error(
                "Server has new version of discussion list but claims that this"
                " update involves 0 new discussions. This is impossible."
            );
        for(std::size_t i=0; i<newDiscussionsFromUpdateCount; ++i)
        {
            auto a=readFromSocket< std::uint32_t >(socket);
            if(a==0)
                throw std::logic_error("Discussion ID can not be 0");
            auto b=readFromSocket< std::string >(socket);
            if(b.empty())
                throw std::logic_error("Discussion name can not be empty");
            result._newDiscussionsFromUpdate.push_back(std::make_pair(a,b));
        }
    }
    for(std::size_t i=0; i<request.discussionsToSynchronization().size(); ++i)
    {
        result._discussionsToSynchronization.push_back(readFromSocket< Address >(socket));
    }
    return result;
}


#endif // CLIENTTOMASTERREPLY_H
