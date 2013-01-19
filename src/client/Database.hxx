/*
 * Copyright 2013  Maciej Poleski
 */

#ifndef DATABASE_H
#define DATABASE_H

#include <vector>
#include <map>

#include <boost/serialization/access.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/serialization/map.hpp>

#include "../ConnCS/Post.hxx"

#include "../shared/Request.hxx"
#include "../shared/Address.hxx"

/**
 * DefaultConstructible
 */
class Database final
{
    friend boost::serialization::access;

    /**
     * Synchronizuje klienta zgodnie z założeniami projektowymi
     */
    void performSynchronization();

    /**
     * Wprowadza informacje o adresie Master'a
     */
    void setMaster(const Address &address);

private:
    void synchronizeDiscussion(DiscussionId discussion, const Address& slave);

    template<class Action>
    void serialize(Action & ar,const unsigned int version)
    {
        ar & _discussionListVersion & _discussionNames & _discussionVersions &
        _discussions & _newDiscussions & _newPosts;

        // ar & _discussionsToSynchronization;
    }

private:
    // STATE
    DiscussionListVersion _discussionListVersion;
    std::map<DiscussionId,std::string> _discussionNames;
    std::map<DiscussionId,DiscussionVersion> _discussionVersions;
    std::map<DiscussionId,std::vector<Post>> _discussions;

    Address _master;

    // TASKS
    std::vector<std::string> _newDiscussions;
    std::map<DiscussionId,std::vector<Post>> _newPosts;
    // TODO: post for discussions without assigned ID's (not yet sent to Master)
    // std::vector<DiscussionId> _discussionsToSynchronization; // just sync everything
};

#endif // DATABASE_H
