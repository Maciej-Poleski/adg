/*
 * Maciej Poleski (C) 2012
*/

#include "ClientToMasterRequest.hxx"

#include "../shared/Request.hxx"

#include <stdexcept>

#include <boost/asio/write.hpp>
#include <boost/asio/read.hpp>

ClientToMasterRequest::ClientToMasterRequest(DiscussionListVersion version)
    : _discussionListVersion(version)
{
    if(version==0)
        throw std::logic_error("Version of discussion list can not be 0");
}

ClientToMasterRequest::ClientToMasterRequest()
{

}

std::uint32_t ClientToMasterRequest::addDiscussionToSynchronize(DiscussionId id)
{
    if(id==0)
        throw std::logic_error("Discussion ID can not be 0");
    _discussionsToSynchronization.push_back(id);
    return _discussionsToSynchronization.size()-1;
}

uint32_t ClientToMasterRequest::addNewDiscussion(const std::string& discussion)
{
    if(discussion.empty())
        throw std::logic_error("Discussion name can not be empty");
    _newDiscussions.push_back(discussion);
    return _newDiscussions.size()-1;
}

DiscussionListVersion ClientToMasterRequest::discussionListVersion() const
{
    return _discussionListVersion;
}

const std::vector< uint32_t >& ClientToMasterRequest::discussionsToSynchronization() const
{
    return _discussionsToSynchronization;
}

const std::vector< std::string >& ClientToMasterRequest::newDiscussions() const
{
    return _newDiscussions;
}

