/*
 * Copyright 2013  Maciej Poleski
 */

#include "Database.hxx"

#include <random>
#include <functional>

#include <boost/asio/ip/tcp.hpp>

#include "../ConnMS/MasterToSlaveRequestNewDiscussion.hxx"
#include "../ConnMS/MasterToSlaveReplyNewDiscussion.hxx"

Database database;

extern boost::asio::io_service io_service;

std::vector< std::pair< DiscussionId, Address > >
Database::createNewDiscussions(std::vector< std::string > newDiscussions)
{
    std::lock_guard<std::mutex> lock(_bigDatabaseLock);
    std::vector< std::pair< DiscussionId, Address > > result;
    std::vector<DiscussionId> collectedIds;
    DiscussionListVersion newVersion=_nextDiscussionListVersion++;
    for(const auto name : newDiscussions)
    {
        auto slave=selectSlave();
        // forwardować do wybranego slave....
        try
        {
            boost::asio::ip::tcp::socket socket(io_service);
            socket.connect(slave.second);
            MasterToSlaveRequestNewDiscussion req;
            DiscussionId id=_nextDiscussionId++;
            req.setId(id);
            req.setName(name);
            sendTo(req,socket); // may fail
            auto rep=receiveFrom<MasterToSlaveReplyNewDiscussion>(socket);
            rep.check(); // may fail ^
            if(rep.result()!=decltype(rep)::ok)
                throw std::logic_error("Slave notified error");
            collectedIds.push_back(id);
            _discussionSlaves[id]=slave.first;
            _discussionNames[id]=name;
            result.push_back(std::make_pair(id,slave.first));
        }
        catch(...)
        {
            result.push_back(std::make_pair(0,slave.first));
        }
    }
    if(!collectedIds.empty())
        _discussionListVersionChangeset[newVersion]=std::move(collectedIds);
    else
    {
        --_nextDiscussionListVersion; // roll back change
    }
    return result;
}

std::vector< std::pair< DiscussionId, std::string > >
Database::getUpdates(DiscussionListVersion version)
{
    checkDiscussionListVersion(version);
    std::lock_guard<std::mutex> lock(_bigDatabaseLock);
    std::vector< std::pair< DiscussionId, std::string > > result;
    if(version>=_nextDiscussionListVersion)
        throw std::logic_error("This version number is not correct");
    for(auto i=_discussionListVersionChangeset.find(version+1);
            i!=_discussionListVersionChangeset.end();
            ++i)
        for(const auto ii : i->second)
        {
            assert(_discussionNames.find(ii)!=_discussionNames.end());
            result.push_back(std::make_pair(ii,_discussionNames[ii]));
        }
    return result;
}

Address Database::getSlave(DiscussionId id)
{
    checkDiscussionId(id);
    std::lock_guard<std::mutex> lock(_bigDatabaseLock);
    if(_discussionSlaves.find(id)==_discussionSlaves.end())
        throw std::logic_error("This discussion ID is not correct");
    return _discussionSlaves[id];
}

DiscussionListVersion Database::currentDiscussionListVersion() const
{
    return _nextDiscussionListVersion-1;
}

void Database::registerSlave(const Address& clientAddress,
                             const Address &masterAddress)
{
    std::lock_guard<std::mutex> lock(_slavesLock);
    _slaves.push_back(clientAddress);
    _slavesForMaster.push_back(masterAddress);
}

std::pair<Address,Address> Database::selectSlave()
{
    static std::mt19937 engine(404);
    std::lock_guard<std::mutex> lock(_slavesLock);
    std::uniform_int_distribution<std::size_t> dist(0,_slaves.size()-1);
    auto gen=std::bind(dist,engine);
    auto r=gen();
    return std::make_pair(_slaves[r],_slavesForMaster[r]);
}
