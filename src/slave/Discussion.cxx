/*
 * Copyright 2012  Maciej Poleski
 */

#include "Discussion.hxx"

#include "../shared/Request.hxx"

void Discussion::setName(const std::string& name)
{
    checkDiscussionName(name);
    _name=name;
}

std::pair< DiscussionVersion, std::vector< PostId > >
Discussion::addPosts(const std::vector< Post >& posts)
{
    DiscussionVersion newVersion=2;
    if(!_discussion.empty())
        newVersion=_discussion.rbegin()->first+1;
    Changeset changeset(this);
    auto ids=changeset.addPosts(posts);
    _discussion[newVersion]=std::move(changeset);
    return std::make_pair(newVersion,ids);
}

std::pair< DiscussionVersion, std::vector< std::pair< PostId, Post > > >
Discussion::prepareUpdate(DiscussionVersion version)
{
    DiscussionVersion newVersion=_discussion.rbegin()->first;
    std::vector<std::pair<PostId,Post>> posts;
    if(version!=newVersion)
    {
        decltype(_discussion.begin()) i;
        if(version==1)
            i=_discussion.begin();
        else
        {
            i=_discussion.find(version);
            if(i==_discussion.end())
            {
                throw std::logic_error("This version is unknown");
            }
            ++i;
        }
        for(; i!=_discussion.end(); ++i)
        {
            auto dump=i->second.getEverything();
            posts.insert(posts.end(),dump.begin(),dump.end());
        }
    }
    return std::make_pair(newVersion,std::move(posts));
}

PostId Discussion::nextPostId()
{
    return _nextPostId++;
}
