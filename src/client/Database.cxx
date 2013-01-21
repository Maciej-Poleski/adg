/*
 * Copyright 2013  Maciej Poleski
 */

#include "Database.hxx"

#include <iomanip>

#include <boost/asio/ip/tcp.hpp>

#include "../ConnCM/ClientToMasterRequest.hxx"
#include "../ConnCM/ClientToMasterReply.hxx"
#include "../ConnCS/ClientToSlaveRequest.hxx"
#include "../ConnCS/ClientToSlaveReply.hxx"

extern boost::asio::io_service io_service;

Database database;

void Database::performSynchronization()
{
    if(!_masterSet)
    {
        throw std::logic_error("You must set master before synchronization");
    }
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
    _masterSet=true;
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

void Database::addNewPostToDiscussion(const Post& post, DiscussionId discussion)
{
    checkDiscussionId(discussion);
    if(_discussionNames.find(discussion)==_discussionNames.end())
        throw std::runtime_error("There is not discussion "+
                                 std::to_string(discussion));
    _newPosts[discussion].push_back(post);
}

void Database::createNewDiscussion(const std::string& name)
{
    checkDiscussionName(name);
    _newDiscussions.push_back(name);
}

void Database::listDiscussions(std::ostream& out) const
{
    out<<"Discussions in Discussion Group:\n";
    for(const auto o : _discussionNames)
    {
        out<<std::setw(4)<<o.first<<"  "<<o.second<<'\n';
    }
    if(_newDiscussions.empty()==false)
    {
        out<<"New discussions:\n";
        for(const auto o : _newDiscussions)
        {
            out<<o<<'\n';
        }
    }
}

void Database::printDiscussion(DiscussionId discussion,std::ostream &out)
{
    checkDiscussionId(discussion);
    if(_discussionNames.find(discussion)==_discussionNames.end())
        throw std::runtime_error("There is not discussion "+
                                 std::to_string(discussion));
    if(_discussions[discussion].empty()==false)
    {
        out<<"Posts in Discussion Group:\n";
        for(const auto o : _discussions[discussion])
        {
            out<<'\n'<<o.message()<<'\n';
        }
    }
    if(_newPosts.find(discussion)!=_newPosts.end() &&
            _newPosts[discussion].empty()==false)
    {
        out<<"\nLocal Posts:\n";
        for(const auto o : _newPosts[discussion])
        {
            out<<'\n'<<o.message()<<'\n';
        }
    }
}
