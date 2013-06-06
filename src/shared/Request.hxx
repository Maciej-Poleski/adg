
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
 * @param socket strumień na który zostanie wysłany obiekt
 */
template<class T,class SyncWriteStream>
void sendTo(const T& o, SyncWriteStream& socket);

/**
 * Globalna funkcja realizująca żądanie transmisji przychodzącej dowolnego
 * obiektu dowolnego typu
 *
 * @param socket strumień z którego zostanie odebrany obiekt
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

template<class SyncReadStream>
std::uint32_t readUint32FromSocket(SyncReadStream &socket);
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

template<class SyncWriteStream>
void writeUint32ToSocket(std::uint32_t count, SyncWriteStream& socket)
{
    Uint32Union u;
    u.ndc=count;
    boost::asio::write(socket,boost::asio::buffer(u.b));
}

template<class SyncReadStream>
std::uint32_t readUint32FromSocket(SyncReadStream& socket)
{
    Uint32Union u;
    boost::asio::read(socket,boost::asio::buffer(u.b),boost::asio::transfer_exactly(4));
    return u.ndc;
}

};


#endif
