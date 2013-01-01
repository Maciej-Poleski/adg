/*
 * Copyright 2013  Maciej Poleski
 */

#include "Database.hxx"

#include <random>
#include <functional>

#include <boost/asio/ip/tcp.hpp>

Database database;

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
        DiscussionId id=_nextDiscussionId++;
        collectedIds.push_back(id);
        _discussionSlaves[id]=slave;
        _discussionNames[id]=name;
        result.push_back(std::make_pair(id,slave));
    }
    _discussionListVersionChangeset[newVersion]=std::move(collectedIds);
    return result;
}

DiscussionListVersion Database::currentDiscussionListVersion() const
{
    return _nextDiscussionListVersion-1;
}

void Database::registerSlave(const Address& address)
{
    std::lock_guard<std::mutex> lock(_slavesLock);
    _slaves.push_back(address);
}

Address Database::selectSlave()
{
    static std::mt19937 engine(404);
    std::lock_guard<std::mutex> lock(_slavesLock);
    std::uniform_int_distribution<std::size_t> dist(0,_slaves.size()-1);
    auto gen=std::bind(dist,engine);
    return _slaves[gen()];
}
