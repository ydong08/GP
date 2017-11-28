//
// Created by new on 2016/11/23.
//

#ifndef DD_BASE_STRUCTDEF_H
#define DD_BASE_STRUCTDEF_H

#include <string>
#include <netinet/in.h>

struct AddressInfo {
    std::string ip;
    uint16_t port;
    short	addrtype;
    union {
        struct in_addr sin_addr;
        struct in6_addr sin6_addr;
    };
};

#endif //DD_BASE_STRUCTDEF_H
