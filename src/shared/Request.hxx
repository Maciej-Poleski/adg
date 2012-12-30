
#ifndef REQUEST_HXX
#define REQUEST_HXX

#include <vector>
#include <string>
#include <cstdint>

#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/streambuf.hpp>
#include <boost/asio/write.hpp>
#include <boost/asio/read.hpp>

class Address;

typedef std::uint32_t DiscussionId;
typedef std::uint32_t DiscussionVersion;
typedef std::uint32_t DiscussionListVersion;
typedef std::uint32_t PostId;

/**
 * Sprawdza czy dane ID może być poprawnym ID dyskusji
 *
 * @param id ID które będzie sprawdzone
 */
void checkDiscussionId(DiscussionId id);

/**
 * Sprawdza czy dany numer wersji może być poprawnym numerem wersji dyskusji
 *
 * @param version numer wersji który będzie sprawdzony
 */
void checkDiscussionVersion(DiscussionVersion version);

/**
 * Sprawdza czy dany numer wersji może być poprawnym numerem wersji listy
 * dyskusji
 *
 * @param version numer wersji który będzie sprawdzony
 */
void checkDiscussionListVersion(DiscussionListVersion version);

/**
 * Sprawdza czy dany napis może być poprawną nazwą dyskusji
 *
 * @param name napis który będzie sprawdzony
 */
void checkDiscussionName(const std::string &name);

/**
 * Sprawdza czy dane ID może być poprawnym ID wiadomości
 *
 * @param id ID które będzie sprawdzone
 */
void checkPostId(PostId id);

/*
 * Tutaj zaczyna się magia
 *
 * Dostarczam metody umożliwiające przesyłanie dowolnych serializowalnych
 * obiektów i tworzę cztery specjalizacja umożliwiające przesyłanie prymitywnych
 * nie serializowalnych obiektów zgodnie z wymogiem projektu.
 */

/**
 * Globalna funkcja realizująca żądanie transmisji wychodzącej na dowolnym
 * obiekcie dowolnego typu
 *
 * @param o obiekt który zostanie wysłany
 * @param socket gniazdo na które zostanie wysłany obiekt
 */
template<class T>
void sendTo(const T &o,boost::asio::ip::tcp::socket &socket);

/**
 * Globalna funkcja realizująca żądanie transmisji przychodzącej dowolnego
 * obiektu dowolnego typu
 *
 * @param socket gniazdo z którego zostanie odebrany obiekt
 * @return odebrany obiekt
 */
template<class T>
T receiveFrom(boost::asio::ip::tcp::socket &socket);

class Post;

namespace detail
{
typedef unsigned char byte;
void writeToSocket(std::uint32_t count,boost::asio::ip::tcp::socket &socket);
void writeToSocket(const std::string &string,boost::asio::ip::tcp::socket &socket);
void writeToSocket(const Address &address,boost::asio::ip::tcp::socket &socket);
void writeToSocket(byte byte,boost::asio::ip::tcp::socket &socket);
template<typename T>
void writeToSocket(const T &o,boost::asio::ip::tcp::socket &socket);

std::uint32_t readUint32FromSocket(boost::asio::ip::tcp::socket &socket);
std::string readStringFromSocket(boost::asio::ip::tcp::socket &socket);
template<typename T>
T readFromSocket(boost::asio::ip::tcp::socket &socket);
template<>
Address readFromSocket<Address>(boost::asio::ip::tcp::socket &socket);
template<>
std::uint32_t readFromSocket<std::uint32_t>(boost::asio::ip::tcp::socket &socket);
template<>
std::string readFromSocket<std::string>(boost::asio::ip::tcp::socket &socket);
template<>
byte readFromSocket<byte>(boost::asio::ip::tcp::socket &socket);
};


// IMPLEMENTATION

namespace detail
{
template<typename T>
void writeToSocket(const T& o, boost::asio::ip::tcp::socket& socket)
{
    sendTo(o,socket);
}

template<typename T>
T readFromSocket(boost::asio::ip::tcp::socket& socket)
{
    return receiveFrom<T>(socket);
}
}


///////////////////////////////////  MAGIC   ///////////////////////////////////

#include <boost/archive/binary_oarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>

template<class T>
void sendTo(const T& o, boost::asio::ip::tcp::socket& socket)
{
    using namespace detail;
    boost::asio::streambuf buf;
    boost::archive::binary_oarchive ar(buf);
    ar<<o;
    std::uint32_t size=buf.size();
    writeToSocket(size,socket);
    boost::asio::write(socket,buf,boost::asio::transfer_exactly(size));
}

template<class T>
T receiveFrom(boost::asio::ip::tcp::socket& socket)
{
    using namespace detail;
    auto size=readFromSocket<std::uint32_t>(socket);
    boost::asio::streambuf buf;
    boost::asio::read(socket,buf,boost::asio::transfer_exactly(size));
    buf.commit(size);
    boost::archive::binary_iarchive ar(buf);
    T result;
    ar>>result;
    return result;
}

#endif
