#include "domoio.h"


namespace domoio {

  DeviceConnection::DeviceConnection(boost::asio::io_service& io_service) : socket(io_service) {

  }

  boost::asio::ip::tcp::socket& DeviceConnection::get_socket(void) {
    return socket;
  }

  void DeviceConnection::start(){
    this->send("Hey, protocol=1.0\n");
    this->read();
  }

  bool DeviceConnection::send(std::string msg) {
    boost::asio::async_write(socket,
                             boost::asio::buffer(msg.c_str(), msg.length() + 1),
                             boost::bind(&DeviceConnection::handle_write, this, boost::asio::placeholders::error)
                             );
    return true;
  }

  void DeviceConnection::read() {
    memset(data, 0, CLIENT_BUFFER_MAX_LENGTH);
    this->socket.async_read_some(boost::asio::buffer(data, CLIENT_BUFFER_MAX_LENGTH),
                                 boost::bind(&DeviceConnection::handle_read, this,
                                             boost::asio::placeholders::error,
                                             boost::asio::placeholders::bytes_transferred));

  }




  void DeviceConnection::handle_read(const boost::system::error_code& error, size_t bytes_transferred) {
    if (!error) {
      this->send(data);
    } else {
      delete this;
    }
  }



  void DeviceConnection::handle_write(const boost::system::error_code& error) {
    if (!error) {
      this->read();
    } else {
      delete this;
    }
  }


}