/*
 * Copyright 2012  Maciej Poleski
 */

#include "Post.hxx"

#include "../shared/Request.hxx"

Post::Post()
{

}

void Post::setMessage(const std::string& message)
{
    std::string oldMsg=_message;
    try
    {
        _message=message;
        checkMessage();
    } catch(...)
    {
        _message=oldMsg;
        throw;
    }
}

const std::string& Post::message() const
{
    return _message;
}

void Post::check() const
{
    checkAll();
}


void Post::checkMessage() const
{
    if(_message.empty())
        throw std::logic_error("message can not be empty");
}

void Post::checkAll() const
{
    checkMessage();
}
