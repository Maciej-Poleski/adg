
#ifndef REQUEST_HXX
#define REQUEST_HXX

#include <vector>
#include <string>
#include <cstdint>

#include <boost/asio/ip/tcp.hpp>

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

namespace detail
{
typedef unsigned char byte;
void writeToSocket(std::uint32_t count,boost::asio::ip::tcp::socket &socket);
void writeToSocket(const std::string &string,boost::asio::ip::tcp::socket &socket);
void writeToSocket(const Address &address,boost::asio::ip::tcp::socket &socket);
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
};


// IMPLEMENTATION

namespace detail
{
template<typename T>
void writeToSocket(const T& o, boost::asio::ip::tcp::socket& socket)
{
    o.sendTo(socket);
}

template<typename T>
T readFromSocket(boost::asio::ip::tcp::socket& socket)
{
    return T::receiveFrom(socket);
}
}

#endif
