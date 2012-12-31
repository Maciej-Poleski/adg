/*
 * Copyright 2012  Maciej Poleski
 */

#include "Database.hxx"

#include <mutex>

#include "Discussion.hxx"

#include "../shared/Request.hxx"

void Database::createNewDiscussion(DiscussionId id, std::string name)
{
    checkDiscussionId(id);
    checkDiscussionName(name);
    std::lock_guard<std::mutex> lock(_bigDatabaseLock);
    Discussion discussion;
    discussion.setName(name);
    _discussions[_nextDiscussionId++]=std::move(discussion);
}

std::pair< DiscussionVersion, std::vector< PostId > >
Database::addPostsToDiscussion(
    const std::vector< Post >& posts,
    DiscussionId id)
{
    checkDiscussionId(id);
    std::lock_guard<std::mutex> lock(_bigDatabaseLock);
    if(_discussions.find(id)==_discussions.end())
    {
        throw std::logic_error("There is no discussion with "+
                               std::to_string(id)+" ID");
    }
    return _discussions[id].addPosts(posts);
}

std::pair< DiscussionVersion, std::vector< std::pair< PostId, Post > > >
Database::prepareUpdate(DiscussionId id, DiscussionVersion version)
{
    checkDiscussionId(id);
    checkDiscussionVersion(version);
    std::lock_guard<std::mutex> lock(_bigDatabaseLock);
    if(_discussions.find(id)==_discussions.end())
    {
        throw std::logic_error("There is no discussion with "+
        std::to_string(id)+" ID");
    }
    return _discussions[id].prepareUpdate(version);
}
