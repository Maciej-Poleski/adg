/*
 * Copyright 2012  Maciej Poleski
 */

#ifndef POST_H
#define POST_H

#include <string>

#include <boost/asio/ip/tcp.hpp>
#include <boost/serialization/access.hpp>

#include "../shared/Request.hxx"

/**
 * Obiekty tej klasy przechowują zawartość pojedyńczej wiadomości z danej
 * dyskusji.
 */
class Post final
{
    friend boost::serialization::access;
public:
    /**
     * Tworzy pustą wiadomości.
     */
    Post();

    /**
     * Odbiera wiadomość
     *
     * @param socket gniazdo z którego zostanie odebrana wiadomość
     * @return odebrana wiadomość
     */
    static Post receiveFrom(boost::asio::ip::tcp::socket &socket);

    /**
     * Ustawia treść wiadomości
     *
     * @param message treść wiadomości
     */
    void setMessage(const std::string &message);

    /**
     * Zwraca treść wiadomości
     *
     * @return treść wiadomości
     */
    const std::string & message() const;

    /**
     * Wysyła wiadomość
     *
     * @param socket gniazdo do którego zostanie wysłana wiadomość
     */
    void sendTo(boost::asio::ip::tcp::socket &socket) const;

    /**
     * Sprawdza czy obiekt znajduje się w poprawnym stanie z punkty widzenia
     * protokołu komunikacyjnego
     */
    void check() const;

private:
    void checkMessage() const;
    void checkAll() const;

    template<class Action>
    void serialize(Action &ar, const unsigned int version)
    {
        ar & _message;
    }

private:
    std::string _message;
};

namespace detail
{
    void writeToSocket(const Post &post,boost::asio::ip::tcp::socket &socket);
    template<>
    Post readFromSocket<Post>(boost::asio::ip::tcp::socket &socket);
};

#endif // POST_H
