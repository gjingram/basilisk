//
// TcpServer.h
//
//

#include "TcpServer.h"

TcpServer::TcpServer(boost::asio::io_service *ioService)
    : m_acceptor(new boost::asio::ip::tcp::acceptor(*ioService))
{
    m_stream.reset(new boost::asio::ip::tcp::socket(*ioService));
}

bool TcpServer::acceptConnections(std::string ipAddress, uint32_t portNum)
{
    boost::system::error_code ec;
    boost::asio::ip::tcp::resolver resolver(m_stream->get_io_service());
    boost::asio::ip::tcp::endpoint endpoint;
    bool portBound = false;
    while(!portBound)
    {
        boost::asio::ip::tcp::resolver::query query(ipAddress, std::to_string(portNum));
        endpoint = *resolver.resolve(query, ec);
        if(ec) {
            std::cout << "Error in " << __FUNCTION__ << " (" << ec.value() << ") " << ec.message() << std::endl;
            return false;
        }
        m_acceptor->open(endpoint.protocol(), ec);
        if(ec) {
            std::cout << "Error in " << __FUNCTION__ << " (" << ec.value() << ") " << ec.message() << std::endl;
            return false;
        }
        m_acceptor->set_option(boost::asio::ip::tcp::acceptor::reuse_address(false), ec);
        if(ec) {
            std::cout << "Error in " << __FUNCTION__ << " (" << ec.value() << ") " << ec.message() << std::endl;
            return false;
        }
        m_acceptor->bind(endpoint, ec);
        if(ec.value() == boost::system::errc::address_in_use) {
            portBound = false;
            portNum += 1;
            m_acceptor->close();
        }
        else if(ec)
        {
            std::cout << "Error in " << __FUNCTION__ << " (" << ec.value() << ") " << ec.message() << std::endl;
            return false;
        }
        else{
            portBound = true;
        }
    }
    
    m_acceptor->listen(boost::asio::socket_base::max_connections, ec);
    if(ec) {
        std::cout << "Error in " << __FUNCTION__ << " (" << ec.value() << ") " << ec.message() << std::endl;
        return false;
    }
    std::cout << "Listening on: " << endpoint.port() << std::endl;

    m_acceptor->accept(*m_stream, endpoint, ec);
    if(ec) {
        std::cout << "Error in " << __FUNCTION__ << " (" << ec.value() << ") " << ec.message() << std::endl;
        return false;
    }
    m_acceptor->close();
    return true;
}

bool TcpServer::close(void)
{
    boost::system::error_code ec;
    m_acceptor->close(ec);
    if(ec) {
        std::cout << "Error in " << __FUNCTION__ << " (" << ec.value() << ") " << ec.message() << std::endl;
        return false;
    }
    m_stream->shutdown(boost::asio::ip::tcp::socket::shutdown_both, ec);
    if(ec) {
        std::cout << "Error in " << __FUNCTION__ << " (" << ec.value() << ") " << ec.message() << std::endl;
        return false;
    }
    m_stream->close(ec);
    if(ec) {
        std::cout << "Error in " << __FUNCTION__ << " (" << ec.value() << ") " << ec.message() << std::endl;
        return false;
    }
    return true;
}

bool TcpServer::receiveData(std::vector<char> &data)
{
    boost::system::error_code ec;
    
    m_inboundBuffer = data;
    
    m_stream->read_some(boost::asio::buffer(m_inboundBuffer), ec);
    if(ec) {
        std::cout << "Error in " << __FUNCTION__ << " (" << ec.value() << ") " << ec.message() << std::endl;
        return false;
    }
    data = m_inboundBuffer;

    return true;
}

bool TcpServer::sendData(std::vector<char> &data)
{
    boost::system::error_code ec;
    m_outboundBuffer = data;
    boost::asio::write(*m_stream, boost::asio::buffer(m_outboundBuffer), ec);
    if(ec) {
        std::cout << "Error in " << __FUNCTION__ << " (" << ec.value() << ") " << ec.message() << std::endl;
        return false;
    }
    return true;
}

void TcpServer::clearBuffers(void)
{
    m_outboundBuffer.clear();
    m_inboundBuffer.clear();
}