/*
 * Copyright 2012  Maciej Poleski
 */

#ifndef CHANGESET_H
#define CHANGESET_H

#include <map>

#include <boost/serialization/access.hpp>
#include <boost/serialization/map.hpp>

#include "../ConnCS/Post.hxx"
#include "../shared/Request.hxx"

class Discussion;

/**
 * Zestaw zmian spowodowanych pojedyńczą aktualizacją
 */
class Changeset
{
    friend boost::serialization::access;

public:
    /**
     * Konstruktor domyślny aby spełnić wymóg DefaultConstructible. Jego
     * używanie nie ma sensu.
     */
    Changeset();

    /**
     * @param parentDiscussion dyskusja której elementem jest ten zestaw
     * zmian
     */
    Changeset(Discussion *parentDiscussion);

    /**
     * Dodaje listę wiadomości do tego zestawu zmian
     *
     * @param posts wiadomości które zostaną dodane
     * @return lista identyfikatorów dodanych wiadomości (lub 0 jeżeli nie
     * udało się dodać danej wiadomości)
     */
    std::vector< PostId >  addPosts(const std::vector< Post >& posts);

    /**
     * @return wszystkie wiadomości z tego zbioru zmian w kolejności rosnących
     * identyfikatorów
     */
    std::vector<std::pair<PostId,Post>> getEverything() const;

private:
    template<class Action>
    void serialize(Action &ar,const unsigned int version)
    {
        ar & _changes & _parentDiscussion;
    }

private:
    std::map<PostId,Post> _changes;
    Discussion* _parentDiscussion;
};

#endif // CHANGESET_H
