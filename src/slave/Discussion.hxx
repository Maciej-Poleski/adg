/*
 * Copyright 2012  Maciej Poleski
 */

#ifndef DISCUSSION_H
#define DISCUSSION_H

#include <map>

#include <boost/serialization/access.hpp>

#include "../shared/Request.hxx"
#include "Changeset.hxx"

class Discussion
{
    friend boost::serialization::access;

private:
    template<class Action>
    void serialize(Action &ar, const unsigned int version)
    {
        ar & _discussion;
    }

private:
    std::map<DiscussionVersion,Changeset> _discussion;
};

#endif // DISCUSSION_H
