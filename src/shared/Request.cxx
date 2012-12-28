#include "Request.hxx"
#include "Address.hxx"

#include <boost/asio/write.hpp>
#include <boost/asio/read.hpp>

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

union Uint16Union
{
    std::uint16_t ndc;
    struct
    {
        byte b[2];
    };
};

void writeToSocket(uint32_t count, boost::asio::ip::tcp::socket& socket)
{
    Uint32Union u;
    u.ndc=count;
    boost::asio::write(socket,boost::asio::buffer(u.b));
}

void writeToSocket(const std::string& string, boost::asio::ip::tcp::socket& socket)
{
    writeToSocket(string.size(),socket);
    boost::asio::write(socket,boost::asio::buffer(string));
}

void writeToSocket(const Address& address, boost::asio::ip::tcp::socket& socket)
{
    Uint32Union ua;
    Uint16Union up;
    ua.ndc=address.ip;
    up.ndc=address.port;
    boost::asio::write(socket,boost::asio::buffer(ua.b));
    boost::asio::write(socket,boost::asio::buffer(up.b));
}

std::uint32_t readUint32FromSocket(boost::asio::ip::tcp::socket& socket)
{
    return readFromSocket<std::uint32_t>(socket);
}

std::string readStringFromSocket(boost::asio::ip::tcp::socket& socket)
{
    return readFromSocket< std::string >(socket);
}

template<>
Address readFromSocket< Address >(boost::asio::ip::tcp::socket& socket)
{
    Uint32Union ua;
    Uint16Union up;
    boost::asio::read(socket,boost::asio::buffer(ua.b),boost::asio::transfer_exactly(4));
    boost::asio::read(socket,boost::asio::buffer(up.b),boost::asio::transfer_exactly(2));
    return {ua.ndc,up.ndc};
}

template<>
std::uint32_t readFromSocket< std::uint32_t >(boost::asio::ip::tcp::socket& socket)
{
    Uint32Union u;
    boost::asio::read(socket,boost::asio::buffer(u.b),boost::asio::transfer_exactly(4));
    return u.ndc;
}

template<>
std::string readFromSocket< std::string >(boost::asio::ip::tcp::socket& socket)
{
    auto size=readUint32FromSocket(socket);
    std::vector<char> rawResult(size);
    boost::asio::read(socket,boost::asio::buffer(rawResult,size),boost::asio::transfer_exactly(size));
    return std::string(rawResult.begin(),rawResult.end());
}

};
