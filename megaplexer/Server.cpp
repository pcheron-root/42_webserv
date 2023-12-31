#include "Server.hpp"

// Constructors

Server::Server(void){
    return ;
}

Server::Server(std::string l_port){

    this->_l_port = l_port;
    this->set_listen_socket(l_port);
    //this->port = std::stoi(l_port);
}

Server::Server(const Server & rhs){ 

    *this = rhs; 
}

// Destructos

Server::~Server(void)
{
    return ;
}

// Operator Overload

Server &    Server::operator=(const Server & rhs) {

    this->_listen_socket = rhs.get_socket(); 
    this->_l_port = rhs.get_l_port(); 
    //this->address = rhs.address; 
    //this->addrlen = rhs.addrlen; 
    return *this;
}

std::ostream &  operator<<(std::ostream & o, Server const & rhs){

    o << "Server n: " << rhs.get_socket() << " listening on port " << rhs.get_l_port() << std::endl;
    return o;
}

// Getters - Setters

int     Server::get_socket(void) const{

    return this->_listen_socket;
}

std::string Server::get_l_port(void) const{

    return this->_l_port;
}

// Function that will set up the listening socket for the current instance

bool    Server::set_listen_socket(std::string l_port){

    int listener;
    int ret_addr;
    int test = 1;
    struct  addrinfo    hints, *ai, *p;

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;
    if ((ret_addr = getaddrinfo(NULL, l_port.c_str(), &hints, &ai)) != 0)
        return (std::cerr << "selectserver : " << gai_strerror(ret_addr) << std::endl, false);
    for (p = ai; p != NULL; p = p->ai_next)
    {
        listener = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
        if (listener < 0)
            continue;
        fcntl(listener, F_SETFL, O_NONBLOCK);
        setsockopt(listener, SOL_SOCKET, SO_REUSEADDR, &test, sizeof(int));
        if (bind(listener, p->ai_addr, p->ai_addrlen) < 0)
        {
            close(listener);
            continue;
        }
        break ;
    }
    freeaddrinfo(ai);
    if (!p || listen(listener, 10) == -1)
        return (false);
    return (this->_listen_socket = listener, true);
}
