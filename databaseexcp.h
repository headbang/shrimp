#ifndef DATABASEEXCP_H
#define DATABASEEXCP_H

#include <exception>
#include <string>

class DatabaseError:public std::exception
{
    std::string message;
public:
    Error(char* msg) { std::string message(msg); }
    const char* what() { return message.c_str(); }
    ~Error() throw();
};

#endif // DATABASEEXCP_H
