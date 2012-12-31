/*
 * Copyright 2012  Maciej Poleski
 */

#ifndef DATABASE_H
#define DATABASE_H

#include <unordered_map>

#include <boost/serialization/access.hpp>

#include "../shared/Request.hxx"
#include "Discussion.hxx"

class Database
{
    friend boost::serialization::access;

private:
    template<class Action>
    void serialize(Action &ar, const unsigned int version)
    {
        ar & _discussions & _nextDiscussionId;
    }

private:
    std::unordered_map<DiscussionId,Discussion> _discussions;
    DiscussionId _nextDiscussionId=1;
};

#endif // DATABASE_H
