/*
 * Maciej Poleski (C) 2012
*/

#include "ClientToMasterRequest.hxx"

#include "../shared/Request.hxx"

#include <stdexcept>

#include <boost/asio/write.hpp>
#include <boost/asio/read.hpp>

ClientToMasterRequest::ClientToMasterRequest(uint32_t discussionListVersion)
    : _discussionListVersion(discussionListVersion)
{
    if(discussionListVersion==0)
        throw std::logic_error("Version of discussion list can not be 0");
}

ClientToMasterRequest::ClientToMasterRequest()
{

}

std::uint32_t ClientToMasterRequest::addDiscussionToSynchronize(std::uint32_t discussionId)
{
    if(discussionId==0)
        throw std::logic_error("Discussion ID can not be 0");
    _discussionsToSynchronization.push_back(discussionId);
    return _discussionsToSynchronization.size()-1;
}

uint32_t ClientToMasterRequest::addNewDiscussion(const std::string& discussion)
{
    if(discussion.empty())
        throw std::logic_error("Discussion name can not be empty");
    _newDiscussions.push_back(discussion);
    return _newDiscussions.size()-1;
}

uint32_t ClientToMasterRequest::discussionListVersion() const
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

void ClientToMasterRequest::sendTo(boost::asio::ip::tcp::socket& socket) const
{
    using namespace detail;
    byte version[]= {1};
    boost::asio::write(socket,boost::asio::buffer(version));
    writeToSocket(static_cast<std::uint32_t>(_newDiscussions.size()),socket);
    for(const auto s : _newDiscussions)
    {
        writeToSocket(s,socket);
    }
    writeToSocket(_discussionListVersion,socket);
    writeToSocket(static_cast<std::uint32_t>
                  (_discussionsToSynchronization.size()),
                  socket);
    for(const auto d : _discussionsToSynchronization)
    {
        writeToSocket(d,socket);
    }
}

ClientToMasterRequest ClientToMasterRequest::receiveFrom(
    boost::asio::ip::tcp::socket& socket)
{
    using namespace detail;
    ClientToMasterRequest result;
    byte version[1];
    boost::asio::read(socket,boost::asio::buffer(version));
    if(*version!=1)
        throw std::runtime_error("Client try to use unsupported protocol"
                                 "version "+std::to_string(*version));
    auto newDiscussionsSize=readUint32FromSocket(socket);
    for(decltype(newDiscussionsSize) i=0; i<newDiscussionsSize; ++i)
    {
        auto && o=readStringFromSocket(socket);
        if(o.empty())
            throw std::runtime_error("Received empty discussion name. This"
                                     "is forbidden."
                                    );
        result._newDiscussions.push_back(std::move(o));
    }
    result._discussionListVersion=readUint32FromSocket(socket);
    if(result._discussionListVersion==0)
        throw std::runtime_error("Received 0 as version of discussion list."
                                 "This is forbidden.");
    auto discussionsToSync=readUint32FromSocket(socket);
    for(decltype(discussionsToSync) i=0; i<discussionsToSync; ++i)
    {
        auto o=readUint32FromSocket(socket);
        if(o==0)
            throw std::runtime_error("Received 0 as discussion ID. This is"
                                     "forbidden");
        result._discussionsToSynchronization.push_back(o);
    }
    return result;
}


