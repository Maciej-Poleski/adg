/*
 * Copyright 2012  Maciej Poleski
 */

#ifndef ADDRESS_H
#define ADDRESS_H

#include <cstdint>

#include <boost/asio/ip/tcp.hpp>
#include <boost/serialization/access.hpp>

/**
 * Przechowuje pełne informacje o lokalizacji pewnej usługi
 */
class Address
{
    friend boost::serialization::access;
public:

    std::uint32_t ip;
    std::uint16_t port;

    /**
     * Konwertuje obiekt na endpoint w stylu Boost
     */
    operator boost::asio::ip::tcp::endpoint() const;

private:
    template<class Action>
    void serialize(Action &ar, const unsigned int version)
    {
        ar & ip & port;
    }
};

#endif // ADDRESS_H
