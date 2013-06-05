/*
 * Copyright 2012  Maciej Poleski
 */

#include "ClientToSlaveReply.hxx"

#include <unordered_map>

#include "ClientToSlaveRequest.hxx"

ClientToSlaveReply::ClientToSlaveReply()
{

}

void ClientToSlaveReply::addCommitedPostId(PostId id)
{
    _commited.push_back(id);
}

void ClientToSlaveReply::addPreparedUpdate(
    DiscussionVersion version,
    std::vector< std::pair< PostId, Post > >&& update)
{
    checkDiscussionVersion(version);
    checkUpdate(update);
    _updates.push_back(std::make_pair(version,std::move(update)));
}

void ClientToSlaveReply::addPreparedUpdate(
    DiscussionVersion version,
    const std::vector< std::pair< PostId, Post > >& update)
{
    addPreparedUpdate(version,std::vector< std::pair< PostId, Post > >(update));
}

void ClientToSlaveReply::checkUpdate(const std::vector< std::pair< PostId, Post > >& update)
{
    if(update.empty())
        throw std::logic_error("Update is empty");
    for(const auto o : update)
        checkPostId(o.first);
}

const std::vector< PostId >& ClientToSlaveReply::commited() const
{
    return _commited;
}

const std::vector< std::pair< DiscussionVersion,
      std::vector< std::pair< PostId, Post > > > >& ClientToSlaveReply::updates() const
{
    return _updates;
}
