/*
 * Copyright 2013  Maciej Poleski
 */

#include "Database.hxx"

#include <boost/asio/ip/tcp.hpp>

#include "../ConnCM/ClientToMasterRequest.hxx"
#include "../ConnCM/ClientToMasterReply.hxx"
#include "../ConnCS/ClientToSlaveRequest.hxx"
#include "../ConnCS/ClientToSlaveReply.hxx"


extern boost::asio::io_service io_service;

void Database::performSynchronization()
{
    ClientToMasterRequest req(_discussionListVersion);
    for(const auto d : _newDiscussions)
        req.addNewDiscussion(d);
    for(const auto d : _discussionVersions)
        req.addDiscussionToSynchronize(d.first);
    boost::asio::ip::tcp::socket socket(io_service);
    socket.connect(_master);
    req.sendTo(socket);
    ClientToMasterReply rep=ClientToMasterReply::receiveFrom(socket,req);
    std::vector<std::string> newDiscussions;
    for(std::size_t i=0; i<_newDiscussions.size(); ++i)
        if(rep.newDiscussions()[i].first==0) // an error occurder - resync later
            newDiscussions.push_back(_newDiscussions[i]);
    // TODO: gather addresses of new discussions for immediate synchronization
    auto oldState=*this;
    try
    {
        _newDiscussions=newDiscussions;
        _discussionListVersion=rep.discussionListVersion();
        for(const auto d : rep.newDiscussionsFromUpdate())
        {
            _discussionNames[d.first]=d.second;
            _discussionVersions[d.first]=1;
            _discussions[d.first]; // yes, I want this
        }
        std::map<DiscussionId,std::vector<Post>> newPosts;
        for(std::size_t i=0; i<req.discussionsToSynchronization().size(); ++i)
        {
            try
            {
                synchronizeDiscussion(req.discussionsToSynchronization()[i],
                                      rep.discussionsToSynchronization()[i]);
            }
            catch(...)
            {
                // ignore - try again later
                newPosts[req.discussionsToSynchronization()[i]]=
                    _newPosts[req.discussionsToSynchronization()[i]];
            }
        }
        _newPosts=newPosts;
    }
    catch(...)
    {
        // rollback
        *this=std::move(oldState);
        throw;
    }
}

void Database::setMaster(const Address& address)
{
    _master=address;
}

void Database::synchronizeDiscussion(DiscussionId discussion,
                                     const Address& slave)
{
    ClientToSlaveRequest req;
    req.addDiscussionToUpdate(discussion,_discussionVersions[discussion]);
    for(const auto p : _newPosts[discussion])
    {
        req.addPostToCommit(discussion,p);
    }
    boost::asio::ip::tcp::socket socket(io_service);
    socket.connect(slave);
    req.sendTo(socket);
    ClientToSlaveReply rep=ClientToSlaveReply::receiveFrom(socket,req);
    assert(rep.updates().size()==1);
    for(const auto update : rep.updates())
    {
        _discussionVersions[discussion]=update.first;
        for(const auto p : update.second)
        {
            _discussions[discussion].push_back(p.second);
        }
    }
    for(const auto id : rep.commited())
        if(id==0)
            throw std::logic_error("Partial success on client side is not"
                " implemented yet"
            );
}
