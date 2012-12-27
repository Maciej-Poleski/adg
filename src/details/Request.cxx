#include "Request.hxx"

#include <boost/asio/write.hpp>
#include <boost/asio/read.hpp>

namespace detail
{
union CounterUnion
{
    std::uint32_t ndc;
    struct
    {
        byte b[4];
    };
};

void writeToSocket(uint32_t count, boost::asio::ip::tcp::socket& socket)
{
    CounterUnion u;
    u.ndc=count;
    boost::asio::write(socket,boost::asio::buffer(u.b));
}

void writeToSocket(const std::string& string, boost::asio::ip::tcp::socket& socket)
{
    writeToSocket(string.size(),socket);
    boost::asio::write(socket,boost::asio::buffer(string));
}

uint32_t readUint32FromSocket(boost::asio::ip::tcp::socket& socket)
{
    CounterUnion u;
    boost::asio::read(socket,boost::asio::buffer(u.b),boost::asio::transfer_exactly(4));
    return u.ndc;
}

std::string readStringFromSocket(boost::asio::ip::tcp::socket& socket)
{
    auto size=readUint32FromSocket(socket);
    std::vector<char> rawResult(size);
    boost::asio::read(socket,boost::asio::buffer(rawResult,size),boost::asio::transfer_exactly(size));
    return std::string(rawResult.begin(),rawResult.end());
}

};
