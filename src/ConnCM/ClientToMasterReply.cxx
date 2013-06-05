/*
 * Copyright 2012  Maciej Poleski
 */

#include "ClientToMasterReply.hxx"
#include "ClientToMasterRequest.hxx"

#include <boost/asio/write.hpp>
#include <boost/asio/read.hpp>

#include "../shared/Request.hxx"

ClientToMasterReply::ClientToMasterReply(DiscussionListVersion version)
    : _discussionListVersion(version)
{
    if(version==0)
        throw std::logic_error("Version of discussion list can not be 0");
}

void ClientToMasterReply::addNewDiscussion(DiscussionId id,
        const Address& address)
{
    _newDiscussions.push_back(std::make_pair(id,address));
}

void ClientToMasterReply::addNewDiscussionFromUpdate(DiscussionId id,
        const std::string& name)
{
    if(id==0)
        throw std::logic_error("Discussion ID can not be 0");
    if(name.empty())
        throw std::logic_error("Discussion name can not be empty");
    _newDiscussionsFromUpdate.push_back(std::make_pair(id,name));
}

void ClientToMasterReply::addDiscussionToSynchronization(const Address& address)
{
    _discussionsToSynchronization.push_back(address);
}

const std::vector< std::pair< DiscussionId, Address > >&
ClientToMasterReply::newDiscussions() const
{
    return _newDiscussions;
}

const uint32_t ClientToMasterReply::discussionListVersion() const
{
    return _discussionListVersion;
}

const std::vector< std::pair< DiscussionId, std::string > >&
ClientToMasterReply::newDiscussionsFromUpdate() const
{
    return _newDiscussionsFromUpdate;
}

const std::vector< Address >& ClientToMasterReply::discussionsToSynchronization() const
{
    return _discussionsToSynchronization;
}

ClientToMasterReply::ClientToMasterReply()
{

}
