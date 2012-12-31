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

    /**
     * Dodaje nowe wiadomości do tej dyskusji
     *
     * @param posts wiadomości które zostaną dodane
     * @return wersja dyskusji po dodaniu wiadomości oraz lista ID nowych
     * wiadomości (0 jeżeli nie udało się)
     */
    std::pair< DiscussionVersion, std::vector< PostId > >
    addPosts(const std::vector< Post >& posts);

    /**
     * Przygotowuje aktualizacje dla klienta
     *
     * @param version wersja dyskusji do aktualizacji
     * @return przygotowana aktualizacja
     */
    std::pair< DiscussionVersion, std::vector< std::pair< PostId, Post > > >
    prepareUpdate(DiscussionVersion version);

    /**
     * @return następny wolny identyfikator wiadomości
     */
    PostId nextPostId();

private:
    template<class Action>
    void serialize(Action &ar, const unsigned int version)
    {
        ar & _discussion & _name;
    }

private:
    std::map<DiscussionVersion,Changeset> _discussion;
    std::string _name;
    PostId _nextPostId=1;
};

#endif // DISCUSSION_H
