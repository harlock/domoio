#ifndef DOMOIO_H
#define DOMOIO_H
#include <cstdlib>
#include <cstdio>
#include <iostream>
#include <sstream>
#include <time.h>
#include <boost/foreach.hpp>
#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include <boost/thread.hpp>
#include <boost/tokenizer.hpp>
#include <boost/algorithm/string/trim.hpp>
#include "config.h"
#include "exceptions.h"
#include "database.h"
#include "models.h"
#include "crypt.h"
#define CLIENT_BUFFER_MAX_LENGTH 1024
#define SERVER_THREADS 1
#define LOG std::cout

int test_crypt(void);



namespace domoio {


  int run(void);


  void init_domoio(void);
  void exit_domoio(void);
  /*
   * Domoio Server
   */


  // Connection

  class Connection {
  public:
    void dispatch_request(std::string);

    virtual bool create_session(int) { return false; }
    virtual bool send(std::string) { return false; }
    virtual bool close() { return false; }
    virtual bool login(const char *) {return false; }

  };


  // Device Connection

  class DeviceConnection : public Connection {
  public:
    DeviceConnection(boost::asio::io_service&);
    ~DeviceConnection();

    boost::asio::ip::tcp::socket& get_socket(void);
    void start();
    bool send(std::string);
    void read();
    bool login(const char *);
    bool is_logged_in();
    bool is_session_started();
    bool close();
    bool create_session(int);
  private:
    boost::asio::ip::tcp::socket socket;
    char data[CLIENT_BUFFER_MAX_LENGTH];

    Device *device;
    domoio::crypto::BlockCipher *block_cipher;
    bool session_started;
    bool logged_in;
    bool disconnected;

    void handle_read(const boost::system::error_code&, size_t );
    bool send_raw(const char*, int);
    bool send_crypted(const char*, int);
    void handle_write(const boost::system::error_code&);
  };



  // Control Connection

  class ControlConnection : public Connection {
  public:
    ControlConnection(boost::asio::io_service&);
    ~ControlConnection();

    boost::asio::local::stream_protocol::socket& get_socket(void) { return socket; }
    void start();

    void read();
    bool close();
    bool send(std::string);

    bool create_session(int);
    bool login(const char *);
  private:
    boost::asio::local::stream_protocol::socket socket;
    char data[CLIENT_BUFFER_MAX_LENGTH];
    bool disconnected;

    void handle_read(const boost::system::error_code&, size_t );
    void handle_write(const boost::system::error_code&);
  };


  // Domoio Device Protocol Server

  class Server {
  public:
    Server(boost::asio::io_service&, short);

  private:
    void start_accept(void);
    void handle_accept(DeviceConnection*, const boost::system::error_code&);

    boost::asio::io_service& io_service;
    boost::asio::ip::tcp::acceptor acceptor;
  };


  // Control Server
  class ControlServer {
  public:
    ControlServer(boost::asio::io_service& , const std::string&);

  private:
    void start_accept(void);
    void handle_accept(ControlConnection*, const boost::system::error_code&);


    boost::asio::io_service& io_service;
    boost::asio::local::stream_protocol::acceptor acceptor;
  };

  //-------------------------------------------------------------

  bool run_server(void);
  bool start_server(void);
  bool stop_server(void);




  /*
   * Commands
   */
  typedef std::vector<std::string>* CommandParams;
  typedef void (*CommandCallback)(Connection*, CommandParams);

  class CommandDef {
  public:
    CommandDef(CommandCallback callback_, int argc_, std::string help_) : callback(callback_), argc(argc_), help(help_) {}

    CommandCallback callback;
    int argc;
    std::string help;
  };

  bool register_server_command(std::string, CommandDef*);

}



#endif //DOMOIO_H
