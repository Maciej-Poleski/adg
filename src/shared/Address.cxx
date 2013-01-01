/*
 * Copyright 2012  Maciej Poleski
 */

#include "Address.hxx"

Address::operator boost::asio::ip::tcp::endpoint() const
{
    return boost::asio::ip::tcp::endpoint(
               boost::asio::ip::address_v4(ip),
               port
           );
}
