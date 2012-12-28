/*
 * Copyright 2012  Maciej Poleski
 */

#include "ClientToMasterReply.hxx"
#include "ClientToMasterRequest.hxx"

#include <boost/asio/write.hpp>
#include <boost/asio/read.hpp>

#include "../shared/Request.hxx"

ClientToMasterReply::ClientToMasterReply(std::uint32_t discussionListVersion)
    : _discussionListVersion(discussionListVersion)
{
    if(discussionListVersion==0)
        throw std::logic_error("Version of discussion list can not be 0");
}

void ClientToMasterReply::addNewDiscussion(std::uint32_t id,
        const Address& address)
{
    _newDiscussions.push_back(std::make_pair(id,address));
}

void ClientToMasterReply::addNewDiscussionFromUpdate(std::uint32_t id,
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

const std::vector< std::pair< std::uint32_t, Address > >&
ClientToMasterReply::newDiscussions() const
{
    return _newDiscussions;
}

const uint32_t ClientToMasterReply::discussionListVersion() const
{
    return _discussionListVersion;
}

const std::vector< std::pair< std::uint32_t, std::string > >&
ClientToMasterReply::newDiscussionsFromUpdate() const
{
    return _newDiscussionsFromUpdate;
}

const std::vector< Address >& ClientToMasterReply::discussionsToSynchronization() const
{
    return _discussionsToSynchronization;
}

void ClientToMasterReply::sendTo(boost::asio::ip::tcp::socket& socket) const
{
    using namespace detail;
    {
        byte rep[1]= {0};
        boost::asio::write(socket,boost::asio::buffer(rep));
    }
    for(const auto o : _newDiscussions)
    {
        writeToSocket(o.first,socket);
        if(o.first!=0)
            writeToSocket(o.second,socket);
    }
    writeToSocket(_discussionListVersion,socket);
    writeToSocket(_newDiscussionsFromUpdate.size(),socket);
    for(const auto o : _newDiscussionsFromUpdate)
    {
        writeToSocket(o.first,socket);
        writeToSocket(o.second,socket);
    }
    for(const auto o : _discussionsToSynchronization)
    {
        writeToSocket(o,socket);
    }
}

void ClientToMasterReply::sendUnsupportedTo(boost::asio::ip::tcp::socket& socket)
{
    using namespace detail;
    byte rep[1]= {1};
    boost::asio::write(socket,boost::asio::buffer(rep));
}

ClientToMasterReply ClientToMasterReply::receiveFrom(
    boost::asio::ip::tcp::socket& socket,
    const ClientToMasterRequest& request)
{
    using namespace detail;
    {
        byte rep[1];
        boost::asio::read(socket,boost::asio::buffer(rep),
                          boost::asio::transfer_exactly(1));
        if(*rep!=0)
            throw std::runtime_error("Client requested unsupported protocol version");
    }
    ClientToMasterReply result;
    for(std::size_t i=0; i<request.newDiscussions().size(); ++i)
    {
        auto a=readUint32FromSocket(socket);
        auto b=Address();
        if(a!=0)
        {
            b=readFromSocket<Address>(socket);
        }
        result._newDiscussions.push_back(std::make_pair(a,b));
    }
    result._discussionListVersion=readFromSocket< std::uint32_t >(socket);
    if(result._discussionListVersion==0)
    {
        throw std::runtime_error("Received 0 as version of discussion list."
                                 "This is forbidden.");
    }
    else if(result._discussionListVersion<request.discussionListVersion())
    {
        throw std::runtime_error(
            "Received "+std::to_string(result._discussionListVersion)+
            " version, but we have "+std::to_string(request.discussionListVersion())+
            ". It is impossible to revert version of discussion"
            "list on the server side so this reply is incorrect"
        );
    }
    else if(result._discussionListVersion==request.discussionListVersion())
    {
        // We are up to date
    }
    else
    {
        auto newDiscussionsFromUpdateCount=readFromSocket< std::uint32_t >(socket);
        if(newDiscussionsFromUpdateCount==0)
            throw std::runtime_error(
                "Server has new version of discussion list but claims that this"
                " update involves 0 new discussions. This is impossible."
            );
        for(std::size_t i=0; i<newDiscussionsFromUpdateCount; ++i)
        {
            auto a=readFromSocket< std::uint32_t >(socket);
            if(a==0)
                throw std::logic_error("Discussion ID can not be 0");
            auto b=readFromSocket< std::string >(socket);
            if(b.empty())
                throw std::logic_error("Discussion name can not be empty");
            result._newDiscussionsFromUpdate.push_back(std::make_pair(a,b));
        }
    }
    for(std::size_t i=0; i<request.discussionsToSynchronization().size(); ++i)
    {
        result._discussionsToSynchronization.push_back(readFromSocket< Address >(socket));
    }
    return result;
}

ClientToMasterReply::ClientToMasterReply()
{

}
