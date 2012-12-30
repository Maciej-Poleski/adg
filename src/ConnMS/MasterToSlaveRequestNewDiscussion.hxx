/*
 * Copyright 2012  Maciej Poleski
 */

#ifndef MASTERTOSLAVEREQUESTNEWDISCUSSION_H
#define MASTERTOSLAVEREQUESTNEWDISCUSSION_H

#include <string>

#include <boost/serialization/access.hpp>

#include "../shared/Request.hxx"

/**
 * Żądanie Mastera do Slave o uworzenie nowej dyskusji o zadanym ID
 */
class MasterToSlaveRequestNewDiscussion
{
    friend boost::serialization::access;
public:

    void setId(DiscussionId id);
    void setName(const std::string &name);
    void setName(std::string &&name);

    DiscussionId id() const;
    const std::string & name() const;

    void check() const;

private:
    template<class Action>
    void save(Action & ar, const unsigned int version) const
    {
        check();
        ar << _id << _name;
    }
    template<class Action>
    void load(Action & ar, const unsigned int version)
    {
        ar >> _id >> _name;
        try {
            check();
        }
        catch(std::logic_error e)
        {
            throw std::runtime_error(std::string("Received malformed object: ")
                                     +e.what());
        }
    }
    BOOST_SERIALIZATION_SPLIT_MEMBER()

private:
    DiscussionId _id;
    std::string _name;

};

#endif // MASTERTOSLAVEREQUESTNEWDISCUSSION_H
