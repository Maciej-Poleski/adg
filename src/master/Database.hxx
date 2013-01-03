/*
 * Copyright 2013  Maciej Poleski
 */

#ifndef DATABASE_H
#define DATABASE_H

#include <map>
#include <mutex>

#include <boost/serialization/access.hpp>
#include <boost/serialization/map.hpp>
#include <boost/serialization/vector.hpp>

#include "../shared/Request.hxx"
#include "../shared/Address.hxx"

/**
 * Obiekt bazy danych Mastera
 */
class Database
{
    friend boost::serialization::access;

public:
    /**
     * Tworzy nowe dyskusje na serwerze i przypisuje ich obsługę do Slave
     *
     * @param newDiscussions lista nowych dyskusji do utworzenia
     * @return lista identyfikatorów nowo utworzonych dyskusji (lub 0)
     * wraz z adresami slave odpowiedzialnych za ich obsługę
     */
    std::vector<std::pair<DiscussionId,Address>>
            createNewDiscussions(std::vector< std::string > newDiscussions);

    /**
     * Tworzy listę nowych dyskusji dodanych do listy dyskusji po wersji version
     *
     * @param version Wersja listy dyskusji po stronie klienta
     * @return lista identyfikatorów dyskusji i nazw dyskusji
     */
    std::vector<std::pair<DiscussionId,std::string>> getUpdates(DiscussionListVersion version);

    /**
     * @param id ID dyskusji
     * @return adres slave odpowiedzialnego za daną dyskusje dla klienta
     */
    Address getSlave(DiscussionId id);

    /**
     * @return obecna wersja listy dyskusji
     */
    DiscussionListVersion currentDiscussionListVersion() const;

    /**
     * Rejestruje nowego Slave w tym Master'ze
     *
     * @param clientAddress adres slave dla klienta
     * @param masterAddress adres slave dla Mastera
     */
    void registerSlave(const Address &clientAddress, const Address &masterAddress);

private:
    /**
     * @return Slave który otrzyma nowe zadanie
     * (first - klient, second - master)
     */
    std::pair<Address,Address> selectSlave();

private:
    template<class Action>
    void serialize(Action &ar, const unsigned int version)
    {
        ar & _discussionNames & _discussionSlaves &
        _discussionListVersionChangeset & _nextDiscussionId &
        _nextDiscussionListVersion & _slaves & _slavesForMaster;
    }

private:
    std::map<DiscussionId,std::string> _discussionNames;
    std::map<DiscussionId,Address> _discussionSlaves;
    std::map<DiscussionListVersion,std::vector<DiscussionId>>
            _discussionListVersionChangeset;
    DiscussionId _nextDiscussionId=1;
    DiscussionListVersion _nextDiscussionListVersion=2;
    std::mutex _bigDatabaseLock;

    std::vector<Address> _slaves;
    std::vector<Address> _slavesForMaster;
    std::mutex _slavesLock;

};

extern Database database;

#endif // DATABASE_H
