/*
 * Copyright 2012  Maciej Poleski
 */

#include "ClientToSlaveReply.hxx"

#include <unordered_map>

#include "ClientToSlaveRequest.hxx"

ClientToSlaveReply::ClientToSlaveReply()
{

}

void ClientToSlaveReply::addCommitedPostId(PostId id)
{
    _commited.push_back(id);
}

void ClientToSlaveReply::addPreparedUpdate(
    DiscussionVersion version,
    std::vector< std::pair< PostId, Post > >&& update)
{
    checkDiscussionVersion(version);
    checkUpdate(update);
    _updates.push_back(std::make_pair(version,std::move(update)));
}

void ClientToSlaveReply::addPreparedUpdate(
    DiscussionVersion version,
    const std::vector< std::pair< PostId, Post > >& update)
{
    addPreparedUpdate(version,std::vector< std::pair< PostId, Post > >(update));
}

void ClientToSlaveReply::checkUpdate(const std::vector< std::pair< PostId, Post > >& update)
{
    if(update.empty())
        throw std::logic_error("Update is empty");
    for(const auto o : update)
        checkPostId(o.first);
}

const std::vector< PostId >& ClientToSlaveReply::commited() const
{
    return _commited;
}

const std::vector< std::pair< DiscussionVersion,
      std::vector< std::pair< PostId, Post > > > >& ClientToSlaveReply::updates() const
{
    return _updates;
}

void ClientToSlaveReply::sendTo(boost::asio::ip::tcp::socket& socket,
                                const ClientToSlaveRequest &request)
{
    using namespace detail;
    std::unordered_multimap<DiscussionId,PostId> set;
    {
        std::size_t count=0;
        for(const auto o : request.discussionsToCommit())
        {
            count+=o.second.size();
        }
        if(count>_commited.size())
        {
            throw std::logic_error("This reply is not ready: not enough"
                                   " commits");
        }
        else if(count<_commited.size())
        {
            throw std::logic_error("This reply is malformed: too many"
                                   " commits");
        }
        decltype(count) i=0;
        for(const auto d : request.discussionsToCommit())
        {
            for(const auto p : d.second)
            {
                writeToSocket(_commited[i],socket);
                set.insert( {d.first,_commited[i]});
                ++i;
            }
        }
        assert(i==count);
    }
    {
        auto count=request.discussionsToUpdate().size();
        if(count>_updates.size())
        {
            throw std::logic_error("This reply is not ready: not enough"
                                   " updates"
                                  );
        }
        else if(count<_updates.size())
        {
            throw std::logic_error("This reply is malformed: too many"
                                   " updates"
                                  );
        }
        for(decltype(count) i=0; i<count; ++i)
        {
            const auto &u=_updates[i];
            const auto &ru=request.discussionsToUpdate()[i];
            writeToSocket(u.first,socket);
            checkUpdate(u.second);
            writeToSocket(static_cast<std::uint32_t>(u.second.size()),socket);
            for(const auto p : u.second)
            {
                writeToSocket(p.first,socket);
                auto discussion=ru.first;
                auto post=p.first;
                if(std::find_if(set.cbegin(set.bucket(ru.first)),
                                set.cend(set.bucket(ru.first)),
                                [discussion,post](const std::pair<DiscussionId,PostId> &o)
            {
                assert(o.first==discussion);
                    return o.second==post;
                }
                               )==set.cend(set.bucket(ru.first)))
                writeToSocket(p.second,socket);
            }
        }
    }
}

namespace std
{
template<>
struct hash<pair<uint32_t,uint32_t>>
{
    size_t operator()(const pair<uint32_t,uint32_t> &o) const
    {
        auto l=static_cast<size_t>(o.first);
        auto r=static_cast<size_t>(o.second);
        return (l<<32) | r;
    }
};
}

ClientToSlaveReply ClientToSlaveReply::receiveFrom(
    boost::asio::ip::tcp::socket& socket,
    const ClientToSlaveRequest& request)
{
    using namespace detail;
    ClientToSlaveReply result;
    std::unordered_multimap<DiscussionId,PostId> set;
    std::unordered_map<std::pair<DiscussionId,PostId>,Post> sentPosts;
    {
        std::size_t count=0;
        for(const auto o : request.discussionsToCommit())
        {
            count+=o.second.size();
        }
        decltype(count) i=0;
        for(const auto d : request.discussionsToCommit())
        {
            const auto &discussion=d.first;
            for(const auto post : d.second)
            {
                PostId id=readFromSocket<PostId>(socket);
                result.addCommitedPostId(id);
                if(id!=0)
                {
                    set.insert( {discussion,id});
                    sentPosts[std::make_pair(discussion,id)]=post;
                }
            }
        }
    }
    {
        auto count=request.discussionsToUpdate().size();
        for(std::size_t i=0; i<count; ++i)
        {
            DiscussionId discussion=request.discussionsToUpdate()[i].first;
            auto version=readFromSocket< std::uint32_t >(socket);
            std::vector<std::pair<PostId,Post>> update;
            auto c=readFromSocket< std::uint32_t >(socket);
            for(std::size_t i=0; i<c; ++i)
            {
                PostId id=readFromSocket<PostId>(socket);
                Post post=Post();
                if(std::find_if(set.cbegin(set.bucket(discussion)),
                                set.cend(set.bucket(discussion)),
                                [discussion,id](const std::pair<DiscussionId,PostId> &o)
            {
                assert(o.first==discussion);
                    return o.second==id;
                }
                               )==set.cend(set.bucket(discussion)))
                {
                    post=readFromSocket<Post>(socket);
                }
                else
                {
                    post=sentPosts[std::make_pair(discussion,id)];
                }
                update.push_back(std::make_pair(id,std::move(post)));
            }
            result.addPreparedUpdate(version,std::move(update));
        }
    }
    return result;
}
