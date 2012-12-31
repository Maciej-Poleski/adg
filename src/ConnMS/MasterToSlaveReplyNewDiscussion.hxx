/*
 * Copyright 2012  Maciej Poleski
 */

#ifndef MASTERTOSLAVEREPLYNEWDISCUSSION_H
#define MASTERTOSLAVEREPLYNEWDISCUSSION_H

#include "../shared/Request.hxx"

/**
 * Odpowiedź na żądanie Mastera do Slave o utworzenie nowej dyskusji.
 */
class MasterToSlaveReplyNewDiscussion final
{
    friend boost::serialization::access;
public:
    enum Result : detail::byte
    {
        ok,
        fail,
    };
    /**
     * Tworzy niezainicjalizowaną odpowiedź
     */
    MasterToSlaveReplyNewDiscussion();

    /**
     * @param result Rezultat żądania
     */
    MasterToSlaveReplyNewDiscussion(Result result);

    /**
     * Ustawia rezultat żądania
     *
     * @param result rezultat żądania
     */
    void setResult(Result result);

    /**
     * @return Rezultat żądania
     */
    Result result();

    /**
     * Sprawdza czy stan obiektu jest poprawny
     */
    void check() const;

private:
    template<class Action>
    void save(Action & ar, const unsigned int version) const
    {
        check();
        ar << _result;
    }
    template<class Action>
    void load(Action & ar, const unsigned int version)
    {
        ar >> _result;
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
    Result _result;

};

#endif // MASTERTOSLAVEREPLYNEWDISCUSSION_H
