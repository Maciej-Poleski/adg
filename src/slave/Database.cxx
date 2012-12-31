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
