/*
 * Copyright 2013  Maciej Poleski
 */

#ifndef DISCUSSIONLIST_H
#define DISCUSSIONLIST_H

#include <vector>
#include <map>

#include <boost/serialization/access.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/serialization/map.hpp>

#include "../shared/Request.hxx"

/**
 * FIXME: DEAD
 */
class DiscussionList final
{
    friend boost::serialization::access;

private:
    DiscussionListVersion _version;
    std::map<DiscussionId,std::string> _discussions;
};

#endif // DISCUSSIONLIST_H
