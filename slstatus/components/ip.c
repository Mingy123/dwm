/* See LICENSE file for copyright and license details. */
#include <ifaddrs.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#if defined(__OpenBSD__)
    #include <sys/socket.h>
    #include <sys/types.h>
#elif defined(__FreeBSD__)
    #include <netinet/in.h>
    #include <sys/socket.h>
#endif

#include "../slstatus.h"
#include "../util.h"

static const char *
ip(const char *interface, unsigned short sa_family)
{
    struct ifaddrs *ifaddr, *ifa;
    int s;
    char host[NI_MAXHOST];

    if (getifaddrs(&ifaddr) < 0) {
        warn("getifaddrs:");
        return NULL;
    }

    for (ifa = ifaddr; ifa != NULL; ifa = ifa->ifa_next) {
        if (!ifa->ifa_addr)
            continue;

        s = getnameinfo(ifa->ifa_addr, sizeof(struct sockaddr_in6),
                        host, NI_MAXHOST, NULL, 0, NI_NUMERICHOST);
        if (!strcmp(ifa->ifa_name, interface) &&
            (ifa->ifa_addr->sa_family == sa_family)) {
            freeifaddrs(ifaddr);
            if (s != 0) {
                warn("getnameinfo: %s", gai_strerror(s));
                return NULL;
            }
            return bprintf("%s", host);
        }
    }

    freeifaddrs(ifaddr);

    return NULL;
}

const char *
ipv4(const char *interface)
{
    return ip(interface, AF_INET);
}

char* ipv4_shorten(const char* addr) {
    char* first = strchr(addr, '.');
    char* second = strchr(first+1, '.');
    size_t len = strlen(second);
    char buf[len+1+1]; // for null and prepending stuff
    buf[0] = '*';
    strcpy(buf+1, second);
    return bprintf("%s", buf);
}

const char *
ipv4_clean_short(const char *interface)
{
    const char* ans = ip(interface, AF_INET);
    if (ans == NULL) {
        return "";
    }
    char* shortened = ipv4_shorten(ans);
    size_t len = strlen(shortened);
    char buf[len+1+1]; // for null and space
    char* end = stpcpy(buf, shortened);
    end[0] = ' ';
    end[1] = 0;
    return bprintf("%s", buf);
}

const char *
ipv4_clean(const char *interface)
{
    const char* ans = ip(interface, AF_INET);
    if (ans == NULL) {
        return "";
    }
    size_t len = strlen(ans);
    char buf[len+1+1]; // for null and space
    char* end = stpcpy(buf, ans);
    end[0] = ' ';
    end[1] = 0;
    return bprintf("%s", buf);
}

const char *
ipv6(const char *interface)
{
    return ip(interface, AF_INET6);
}
