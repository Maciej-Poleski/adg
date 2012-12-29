/*
 * Copyright 2012  Maciej Poleski
 */

#include "ClientToSlaveRequest.hxx"

#include <algorithm>

#include "../shared/Request.hxx"

ClientToSlaveRequest::ClientToSlaveRequest()
{

}

void ClientToSlaveRequest::addDiscussionToUpdate(std::uint32_t id,
        std::uint32_t version)
{
    checkDiscussionID(id);
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
    checkDiscussionID(discussionID);
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

const std::vector< std::pair< ::uint32_t, ::uint32_t > >&
ClientToSlaveRequest::discussionsToUpdate() const
{
    return _discussionsToUpdate;
}

const std::vector< std::pair< ::uint32_t, std::vector< Post > > >&
ClientToSlaveRequest::discussionsToCommit() const
{
    return _discussionsToCommit;
}

void ClientToSlaveRequest::sendTo(boost::asio::ip::tcp::socket& socket) const
{
    using namespace detail;
    writeToSocket(static_cast<std::uint32_t>(_discussionsToUpdate.size()),
                  socket);
    for(const auto o : _discussionsToUpdate)
    {
        writeToSocket(o.first,socket);
        writeToSocket(o.second,socket);
    }
    writeToSocket(static_cast<std::uint32_t>(_discussionsToCommit.size()),
                  socket);
    for(const auto o : _discussionsToCommit)
    {
        writeToSocket(o.first,socket);
        writeToSocket(static_cast<std::uint32_t>(o.second.size()),socket);
        for(const auto oo : o.second)
        {
            writeToSocket(oo,socket);
        }
    }
}
