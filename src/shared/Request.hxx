
#ifndef REQUEST_HXX
#define REQUEST_HXX

#include <vector>
#include <string>
#include <cstdint>

#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/streambuf.hpp>
#include <boost/asio/write.hpp>
#include <boost/asio/read.hpp>

#include "Address.hxx"

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
template<class T,class SyncWriteStream>
void sendTo(const T& o, SyncWriteStream& socket);

/**
 * Globalna funkcja realizująca żądanie transmisji przychodzącej dowolnego
 * obiektu dowolnego typu
 *
 * @param socket gniazdo z którego zostanie odebrany obiekt
 * @return odebrany obiekt
 */
template<class T,class SyncReadStream>
T receiveFrom(SyncReadStream& socket);

class Post;

namespace detail
{
typedef unsigned char byte;

template<class SyncWriteStream>
void writeUint32ToSocket(std::uint32_t count,SyncWriteStream &socket);
// template<class SyncWriteStream>
// void writeToSocket(const std::string &string,SyncWriteStream &socket);
// template<class SyncWriteStream>
// void writeToSocket(const Address &address,SyncWriteStream &socket);
// template<class SyncWriteStream>
// void writeToSocket(byte byte,SyncWriteStream &socket);
// template<typename T,class SyncWriteStream>
// void writeToSocket(const T &o,SyncWriteStream &socket);
//
template<class SyncReadStream>
std::uint32_t readUint32FromSocket(SyncReadStream &socket);
// template<class SyncReadStream>
// std::string readStringFromSocket(SyncReadStream &socket);
//
// template<typename T,class SyncReadStream>
// T readFromSocket(SyncReadStream& socket);
// template<class SyncReadStream>
// Address readFromSocket<Address>(SyncReadStream &socket);
// template<class SyncReadStream>
// std::uint32_t readFromSocket<std::uint32_t>(SyncReadStream &socket);
// template<class SyncReadStream>
// std::string readFromSocket<std::string>(SyncReadStream &socket);
// template<class SyncReadStream>
// byte readFromSocket<byte>(SyncReadStream &socket);
};


// IMPLEMENTATION

namespace detail
{
template<typename T,class SyncWriteStream>
void writeToSocket(const T& o, SyncWriteStream& socket)
{
    sendTo(o,socket);
}

template<typename T,class SyncReadStream>
T readFromSocket(SyncReadStream& socket)
{
    return receiveFrom<T>(socket);
}
}


///////////////////////////////////  MAGIC   ///////////////////////////////////

#include <boost/archive/binary_oarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>

template<class T,class SyncWriteStream>
void sendTo(const T& o, SyncWriteStream& socket)
{
    using namespace detail;
    boost::asio::streambuf buf;
    boost::archive::binary_oarchive ar(buf);
    ar<<o;
    std::uint32_t size=buf.size();
    writeUint32ToSocket(size,socket);
    boost::asio::write(socket,buf,boost::asio::transfer_exactly(size));
}

template<class T,class SyncReadStream>
T receiveFrom(SyncReadStream& socket)
{
    using namespace detail;
    auto size=readUint32FromSocket(socket);
    boost::asio::streambuf buf;
    boost::asio::read(socket,buf,boost::asio::transfer_exactly(size));
    buf.commit(size);
    boost::archive::binary_iarchive ar(buf);
    T result;
    ar>>result;
    return result;
}

////////////////////////// TEMPLATES ///////////////////////////////////////////


namespace detail
{
union Uint32Union
{
    std::uint32_t ndc;
    struct
    {
        byte b[4];
    };
};
//
// union Uint16Union
// {
//     std::uint16_t ndc;
//     struct
//     {
//         byte b[2];
//     };
// };
//
template<class SyncWriteStream>
void writeUint32ToSocket(std::uint32_t count, SyncWriteStream& socket)
{
    Uint32Union u;
    u.ndc=count;
    boost::asio::write(socket,boost::asio::buffer(u.b));
}
//
// template<class SyncWriteStream>
// void writeToSocket(const std::string& string, SyncWriteStream& socket)
// {
//     writeToSocket(static_cast<std::uint32_t>(string.size()),socket);
//     boost::asio::write(socket,boost::asio::buffer(string));
// }
//
// template<class SyncWriteStream>
// void writeToSocket(const Address& address, SyncWriteStream& socket)
// {
//     Uint32Union ua;
//     Uint16Union up;
//     ua.ndc=address.ip;
//     up.ndc=address.port;
//     boost::asio::write(socket,boost::asio::buffer(ua.b));
//     boost::asio::write(socket,boost::asio::buffer(up.b));
// }
//
// template<class SyncWriteStream>
// void writeToSocket(byte b, SyncWriteStream& socket)
// {
//     byte v[1]= {b};
//     boost::asio::write(socket,boost::asio::buffer(v),
//                        boost::asio::transfer_exactly(1));
// }
//
template<class SyncReadStream>
std::uint32_t readUint32FromSocket(SyncReadStream& socket)
{
    Uint32Union u;
    boost::asio::read(socket,boost::asio::buffer(u.b),boost::asio::transfer_exactly(4));
    return u.ndc;
}
//
// template<class SyncReadStream>
// std::string readStringFromSocket(SyncReadStream& socket)
// {
//     return readFromSocket< std::string >(socket);
// }

};


#endif
