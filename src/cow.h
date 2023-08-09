#ifndef _COW_H_
#define _COW_H_

#include "server.h"

#ifndef PORT 
#define PORT 8080 
#endif
#ifndef IP 
#define IP "127.0.0.1" 
#endif

core::Server Server(PORT, IP);


#endif // !#ifndef _COW_H_
