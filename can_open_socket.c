#include <string.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <linux/can.h>
#include <linux/if.h>

int can_open_socket(const char *interface_name)
{
    /**
     * Like TCP/IP, you first need to open a socket for communicating over a
     * CAN network. Since SocketCAN implements a new protocol family, you
     * need to pass PF_CAN as the first argument to the socket(2) system
     * call. Currently, there are two CAN protocols to choose from, the raw
     * socket protocol and the broadcast manager (BCM). So to open a socket.
     *
     * In this example, we create a raw CAN socket.
     */
    int s = socket(PF_CAN, SOCK_RAW, CAN_RAW);

    /**
     * The sockaddr_can structure has an interface index like the
     * PF_PACKET socket, that also binds to a specific interface:
     *
     *   struct sockaddr_can {
     *       sa_family_t can_family;
     *       int         can_ifindex;
     *       union {
     *           // transport protocol class address info (e.g. ISOTP)
     *           struct { canid_t rx_id, tx_id; } tp;
     *
     *           // reserved for future CAN protocols address information
     *       } can_addr;
     *   }
     */
    /* resolve interface index by name */
    struct ifreq ifr;
    strcpy(ifr.ifr_name, interface_name);
    ioctl(s, SIOCGIFINDEX, &ifr); // SIOCGIFINDEX is name -> if_index mapping. See /usr/include/linux/sockios.h.
    
	struct sockaddr_can addr;
    addr.can_family = AF_CAN;
    addr.can_ifindex = ifr.ifr_ifindex;

    bind(s, (struct sockaddr *) &addr, sizeof(addr));

    return s;
}

