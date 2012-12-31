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
