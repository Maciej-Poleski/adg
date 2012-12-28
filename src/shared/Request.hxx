#include <vector>
#include <string>
#include <cstdint>

#include <boost/asio/ip/tcp.hpp>

class Address;

namespace detail
{
typedef unsigned char byte;
void writeToSocket(std::uint32_t count,boost::asio::ip::tcp::socket &socket);
void writeToSocket(const std::string &string,boost::asio::ip::tcp::socket &socket);
void writeToSocket(const Address &address,boost::asio::ip::tcp::socket &socket);

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
