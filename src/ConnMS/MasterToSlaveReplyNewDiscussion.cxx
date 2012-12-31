/*
 * Copyright 2012  Maciej Poleski
 */

#include "MasterToSlaveReplyNewDiscussion.hxx"

MasterToSlaveReplyNewDiscussion::MasterToSlaveReplyNewDiscussion()
{

}

MasterToSlaveReplyNewDiscussion::MasterToSlaveReplyNewDiscussion
(MasterToSlaveReplyNewDiscussion::Result result) : _result(result)
{

}

void MasterToSlaveReplyNewDiscussion::setResult(MasterToSlaveReplyNewDiscussion::Result result)
{
    auto old=_result;
    try
    {
        _result=result;
        check();
    }
    catch(...)
    {
        _result=old;
        throw;
    }
}

MasterToSlaveReplyNewDiscussion::Result MasterToSlaveReplyNewDiscussion::result()
{
    return _result;
}

void MasterToSlaveReplyNewDiscussion::check() const
{
    if(_result!=Result::ok && _result!=Result::fail)
        throw std::logic_error("Result is unknown");
}
