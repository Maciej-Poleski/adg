/*
 * Copyright 2013  Maciej Poleski <maciej.poleski@uj.edu.pl>
 */

#ifndef CLIENTREQUEST_H
#define CLIENTREQUEST_H

#include <memory>

#include <boost/asio/ip/tcp.hpp>

extern std::atomic_uint_fast32_t countOfConnectedClients;

/**
 * Obsługuje żądanie od klienta
 */
class ClientRequest final
{
public:
    ClientRequest() = delete;

    ~ClientRequest();
    /**
     * Inicjalizuje obiekt
     *
     * @param socket gniazdo z którego będzie odczytane żądanie i wysłana
     * odpowiedź
     */
    ClientRequest(boost::asio::ip::tcp::socket &&socket);

    /**
     * Wykonuje wszystkie działania związane z obsługą żądania
     */
    void dispatch(std::shared_ptr<ClientRequest> handle) noexcept;

private:
    boost::asio::ip::tcp::socket _socket;
};

#endif // CLIENTREQUEST_H
