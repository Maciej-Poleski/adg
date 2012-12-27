#include <vector>
#include <string>
#include <cstdint>

#include <boost/asio/ip/tcp.hpp>

namespace detail
{
    typedef unsigned char byte;
    void writeToSocket(std::uint32_t count,boost::asio::ip::tcp::socket &socket);
    void writeToSocket(const std::string &string,boost::asio::ip::tcp::socket &socket);

    std::uint32_t readUint32FromSocket(boost::asio::ip::tcp::socket &socket);
    std::string readStringFromSocket(boost::asio::ip::tcp::socket &socket);
};