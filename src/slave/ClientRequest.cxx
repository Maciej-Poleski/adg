/*
 * Copyright 2012  Maciej Poleski <maciej.poleski@uj.edu.pl>
 */

#include "ClientRequest.hxx"

ClientRequest::ClientRequest(boost::asio::ip::tcp::socket&& socket) :
_socket(std::move(socket))
{

}

void ClientRequest::dispatch() noexcept
{

}
