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

void ClientToSlaveRequest::sendTo(boost::asio::ip::tcp::socket& socket) const
{
    using namespace detail;
    {
        byte v[1]= {0};
        boost::asio::write(socket,boost::asio::buffer(v),
                           boost::asio::transfer_exactly(1));
    }
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

ClientToSlaveRequest ClientToSlaveRequest::receiveFrom(
    boost::asio::ip::tcp::socket& socket)
{
    using namespace detail;
    {
        byte v[1];
        boost::asio::read(socket,boost::asio::buffer(v),
                          boost::asio::transfer_exactly(1));
        if(*v!=0)
            throw std::runtime_error("Client requested unsupported protocol");
    }
    ClientToSlaveRequest result;
    try
    {
        {
            auto count=readFromSocket< std::uint32_t >(socket);
            for(std::size_t i=0; i<count; ++i)
            {
                auto id=readFromSocket< std::uint32_t >(socket);
                auto version=readFromSocket< std::uint32_t >(socket);
                result.addDiscussionToUpdate(id,version);
            }
        }
        {
            auto count=readFromSocket< std::uint32_t >(socket);
            for(std::size_t i=0; i<count; ++i)
            {
                auto id=readFromSocket< std::uint32_t >(socket);
                checkDiscussionId(id);
                auto c=readFromSocket< std::uint32_t >(socket);
                if(c==0)
                {
                    throw std::runtime_error("Client commited 0 new posts."
                                             " This is forbidden."
                                            );
                }
                for(std::size_t i=0; i<c; ++i)
                {
                    auto post=readFromSocket<Post>(socket);
                    post.check();
                    result.addPostToCommit(id,post);
                }
            }
        }
    }
    catch(std::logic_error e)
    {
        throw std::runtime_error(std::string("Received malformed object: ")+
                                 e.what());
    }
    return result;
}
