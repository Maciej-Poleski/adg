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
