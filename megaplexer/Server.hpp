#ifndef SERVER_HPP
# define SERVER_HPP

# include <iostream>
# include <string>
# include <cstring>
# include <fcntl.h>
# include <errno.h>
# include <netdb.h>
# include <sys/socket.h>
# include <sys/socket.h>
# include <arpa/inet.h>
# include <sys/select.h>
# include <sys/types.h>
# include <dirent.h>
# include <poll.h>
# include <sys/epoll.h>
# include <sys/wait.h>
# include <sys/stat.h>
# include <signal.h>
# include <unistd.h>
# include <cstdio>
# include <vector>

class   Server{

    private :

    int _listen_socket;
    //int   _port;
    std::string   _l_port;
    //struct sockaddr_in _address; 
    //socklen_t   _addrlen;

    public :

    Server();
    Server(std::string l_port);
    Server(const Server & rhs);
    ~Server();

    Server & operator=(const Server & rhs);
 
    // Getters and setters of private value
    
    int get_socket(void) const;
    std::string get_l_port(void) const;

    // Function that will set up the listening socket for the current instance
    
    bool set_listen_socket(std::string l_port);

};

std::ostream &  operator<<(std::ostream & o, Server const & rhs);

#endif
