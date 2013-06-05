/*
 * Copyright 2012  Maciej Poleski
 */

#include "ClientToSlaveRequest.hxx"

#include <algorithm>
#include <boost/asio/write.hpp>
#include <boost/asio/read.hpp>

#include "../shared/Request.hxx"

ClientToSlaveRequest::ClientToSlaveRequest()
{

}

void ClientToSlaveRequest::addDiscussionToUpdate(std::uint32_t id,
        std::uint32_t version)
{
    checkDiscussionId(id);
    checkDiscussionVersion(version);
    if(std::find_if(_discussionsToUpdate.cbegin(),_discussionsToUpdate.cend(),
                    [id](const std::pair<std::uint32_t,std::uint32_t> &o)
{
    return o.first==id;
})!=_discussionsToUpdate.cend())
    throw std::logic_error("Discussion "+std::to_string(id)+" already exist"
                           " in this request"
                          );
    _discussionsToUpdate.push_back(std::make_pair(id,version));
}

void ClientToSlaveRequest::addPostToCommit(std::uint32_t discussionID,
        const Post& post)
{
    checkDiscussionId(discussionID);
    post.check();
    auto i=std::find_if(_discussionsToCommit.begin(),_discussionsToCommit.end(),
                        [discussionID](decltype(_discussionsToCommit[0]) &o)
    {
        return o.first==discussionID;
    });
    if(i==_discussionsToCommit.end())
    {
        _discussionsToCommit.push_back(std::make_pair(
                                           discussionID,
                                           std::vector<Post> {post}));
    }
    else
    {
        i->second.push_back(post);
    }
}

const std::vector< std::pair< DiscussionId, DiscussionVersion > >&
ClientToSlaveRequest::discussionsToUpdate() const
{
    return _discussionsToUpdate;
}

const std::vector< std::pair< DiscussionId, std::vector< Post > > >&
ClientToSlaveRequest::discussionsToCommit() const
{
    return _discussionsToCommit;
}

