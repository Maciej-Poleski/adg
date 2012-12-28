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
     * @param discussionListVersion wersja listy dyskusji po stronie Mastera
     */
    ClientToMasterReply(std::uint32_t discussionListVersion);

    /**
     * Odbiera odpowiedź na podane zapytanie od Mastera i tworzy na jej
     * podstawie obiekt tej klasy
     *
     * @param socket gniazdo z którego będzie odebrana odpowiedź
     * @param request zapytanie na które będzie odbierana odpowiedź
     * @return odpowiedź mastera w postaci obiektu
     */
    static ClientToMasterReply receiveFrom(boost::asio::ip::tcp::socket &socket,
                                           const ClientToMasterRequest &request);

    /**
     * Przydziela id oraz slave do kolejnej nowej dyskusji
     *
     * @param id ID nowo utworzonej dyskusji lub 0 w razie niepowodzenia
     * @param address w razie powodzenia adres slave odpowiedzialnego za
     *          nowo utworzoną dyskusje
     */
    void addNewDiscussion(std::uint32_t id,const Address &address=Address());

    /**
     * Dodaje informacje o nowej dyskusji pochodzącej (z punktu widzenia klienta)
     * z aktualizacji
     *
     * @param id ID nowej dyskusji
     * @param name nazwa nowej dyskusji
     */
    void addNewDiscussionFromUpdate(std::uint32_t id, const std::string &name);

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
    const std::vector<std::pair<std::uint32_t,Address>> & newDiscussions() const;

    /**
     * @return numer wersji listy dyskusji w tej odpowiedzi
     */
    const std::uint32_t discussionListVersion() const;

    /**
     * @return lista identyfikatorów wraz z nazwami dyskusji będących dla
     *          klienta aktualizacją jego listy do wersji podanej w tej
     *          odpowiedzi
     */
    const std::vector<std::pair<uint32_t,std::string>> & newDiscussionsFromUpdate() const;

    /**
     * @return lista slave odpowiedzialnych za odpowiednie dyskusje z żądania
     *          klienta (lub 0 jeżeli nie udał się zdobyć tej informacji)
     */
    const std::vector<Address>& discussionsToSynchronization() const;

    /**
     * Wysyła tą odpowiedź
     *
     * @param socket gniazdo na które zostanie wysłana ta odpowiedź
     */
    void sendTo(boost::asio::ip::tcp::socket &socket) const;

    /**
     * Wysyła informację o użyciu przez klienta nieobsługiwanej wersji
     * protokołu
     *
     * @param socket gniazdo na które zostanie wysłana informacja
     */
    static void sendUnsupportedTo(boost::asio::ip::tcp::socket &socket);

private:
    ClientToMasterReply();

private:
    std::vector<std::pair<std::uint32_t,Address>> _newDiscussions;
    std::uint32_t _discussionListVersion;
    std::vector<std::pair<uint32_t,std::string>> _newDiscussionsFromUpdate;
    std::vector<Address> _discussionsToSynchronization;

};

#endif // CLIENTTOMASTERREPLY_H
