/*
 * Copyright 2012  Maciej Poleski
 */

#ifndef DATABASE_H
#define DATABASE_H

#include <map>
#include <mutex>

#include <boost/serialization/access.hpp>
#include <boost/serialization/map.hpp>

#include "../shared/Request.hxx"
#include "Discussion.hxx"

/**
 * Obiekt bazy danych Slave.
 */
class Database
{
    friend boost::serialization::access;

public:
    /**
     * Tworzy nową dyskusję w tym Slave
     *
     * @param id ID nowej dyskusji (musi być koniecznie to)
     * @param name nazwa nowej dyskusji
     */
    void createNewDiscussion(DiscussionId id, std::string name);

    /**
     * Dodaje wiadomości do dyskusji
     *
     * @param post wiadomości którę zostaną dodane
     * @param id ID dyskusji do której zostaną dodane wiadomości
     * @return wersja dyskusji po dodaniu wiadomości oraz lista ID nowych
     * wiadomości (0 jeżeli nie udało się)
     */
    std::pair<DiscussionVersion,std::vector<PostId>>
            addPostsToDiscussion(const std::vector<Post>& posts, DiscussionId id);

    /**
     * Przygotowuje aktualizacje dla klienta
     *
     * @param id ID dyskusji do aktualizacji
     * @param version wersja dyskusji do aktualizacji
     * @return przygotowana aktualizacja
     */
    std::pair<DiscussionVersion,std::vector<std::pair<PostId,Post>>>
    prepareUpdate(DiscussionId id, DiscussionVersion version);

private:
    template<class Action>
    void serialize(Action &ar, const unsigned int version)
    {
        ar & _discussions & _nextDiscussionId;
    }

private:
    std::map<DiscussionId,Discussion> _discussions;
    DiscussionId _nextDiscussionId=1;
    std::mutex _bigDatabaseLock;
};

extern Database database;

#endif // DATABASE_H
