/*
 * Copyright 2012  Maciej Poleski
 */

#ifndef CHANGESET_H
#define CHANGESET_H

#include <map>

#include <boost/serialization/access.hpp>

#include "../ConnCS/Post.hxx"
#include "../shared/Request.hxx"

class Changeset
{
    friend boost::serialization::access;
private:
    template<class Action>
    void serialize(Action &ar,const unsigned int version)
    {
        ar & _changes;
    }

private:
    std::map<PostId,Post> _changes;
};

#endif // CHANGESET_H
