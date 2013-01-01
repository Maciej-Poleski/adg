/*
 * Copyright 2012  Maciej Poleski
 */

#include "Changeset.hxx"

#include "Discussion.hxx"

Changeset::Changeset()
{

}

std::vector< PostId > Changeset::addPosts(const std::vector< Post > &posts,
        const std::function<PostId()> &idGenerator)
{
    std::vector<PostId> result;
    for(const auto post : posts)
    {
        PostId id=idGenerator();
        _changes[id]=post; // na tym etapie wiadomo że się uda
        result.push_back(id);
    }
    return result;
}

std::vector< std::pair< PostId, Post > > Changeset::getEverything() const
{
    std::vector< std::pair< PostId, Post > > result;
    for(const auto p : _changes)
    {
        result.push_back(p);
    }
    return result;
}
