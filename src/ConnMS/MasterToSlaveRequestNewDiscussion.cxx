/*
 * Copyright 2012  Maciej Poleski
 */

#include "MasterToSlaveRequestNewDiscussion.hxx"

void MasterToSlaveRequestNewDiscussion::setId(DiscussionId id)
{
    checkDiscussionId(id);
    _id=id;
}

void MasterToSlaveRequestNewDiscussion::setName(const std::string& name)
{
    checkDiscussionName(name);
    _name=name;
}

void MasterToSlaveRequestNewDiscussion::setName(std::string&& name)
{
    checkDiscussionName(name);
    _name=std::move(name);
}

DiscussionId MasterToSlaveRequestNewDiscussion::id() const
{
    return _id;
}

const std::string& MasterToSlaveRequestNewDiscussion::name() const
{
    return _name;
}
