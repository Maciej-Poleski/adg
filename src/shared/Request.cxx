#include "Request.hxx"
#include "Address.hxx"
#include "../ConnCS/Post.hxx"

#include <boost/asio/write.hpp>
#include <boost/asio/read.hpp>

void checkDiscussionId(DiscussionId id)
{
    if(id==0)
        throw std::logic_error("Discussion ID can not be 0");
}

void checkDiscussionVersion(DiscussionVersion version)
{
    if(version==0)
        throw std::logic_error("Discussion version can not be 0");
}

void checkDiscussionListVersion(DiscussionListVersion version)
{
    if(version==0)
        throw std::logic_error("Version of discussion list can not be 0");
}

void checkDiscussionName(const std::string& name)
{
    if(name.empty())
        throw std::logic_error("Discussion name can not be empty");
}

void checkPostId(PostId id)
{
    if(id==0)
        throw std::logic_error("Post ID can not be 0");
}

namespace detail
{

// template<>
// Address readFromSocket< Address >(boost::asio::ip::tcp::socket& socket)
// {
//     Uint32Union ua;
//     Uint16Union up;
//     boost::asio::read(socket,boost::asio::buffer(ua.b),boost::asio::transfer_exactly(4));
//     boost::asio::read(socket,boost::asio::buffer(up.b),boost::asio::transfer_exactly(2));
//     return {ua.ndc,up.ndc};
// }
//
// template<>
// std::uint32_t readFromSocket< std::uint32_t >(boost::asio::ip::tcp::socket& socket)
// {
//     Uint32Union u;
//     boost::asio::read(socket,boost::asio::buffer(u.b),boost::asio::transfer_exactly(4));
//     return u.ndc;
// }
//
// template<>
// std::string readFromSocket< std::string >(boost::asio::ip::tcp::socket& socket)
// {
//     auto size=readUint32FromSocket(socket);
//     std::vector<char> rawResult(size);
//     boost::asio::read(socket,boost::asio::buffer(rawResult,size),boost::asio::transfer_exactly(size));
//     return std::string(rawResult.begin(),rawResult.end());
// }
//
// template<>
// byte readFromSocket< byte >(boost::asio::ip::tcp::socket& socket)
// {
//     byte v[1];
//     boost::asio::read(socket,boost::asio::buffer(v),
//                       boost::asio::transfer_exactly(1));
//     return *v;
// }
//

};

