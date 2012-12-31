/*
 * Copyright 2012  Maciej Poleski
 */

#ifndef DISCUSSION_H
#define DISCUSSION_H

#include <map>
#include <string>

#include <boost/serialization/access.hpp>
#include <boost/serialization/map.hpp>

#include "../shared/Request.hxx"
#include "Changeset.hxx"

/**
 * Obiekt reprezentuje pojedyńczą dyskusję w ramach Slave
 */
class Discussion
{
    friend boost::serialization::access;

public:
    /**
     * Ustawia nazwę dyskusji
     *
     * @param name nowa nazwa dyskusji
     */
    void setName(const std::string &name);

private:
    template<class Action>
    void serialize(Action &ar, const unsigned int version)
    {
        ar & _discussion & _name;
    }

private:
    std::map<DiscussionVersion,Changeset> _discussion;
    std::string _name;
};

#endif // DISCUSSION_H
