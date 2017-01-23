#include <stdio.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <linux/can.h>
#include <linux/if.h>

int main(int argc, char *argv[])
{
    /*
     *  Like TCP/IP, you first need to open a socket for communicating over a
     * CAN network. Since SocketCAN implements a new protocol family, you
     * need to pass PF_CAN as the first argument to the socket(2) system
     * call. Currently, there are two CAN protocols to choose from, the raw
     * socket protocol and the broadcast manager (BCM). So to open a socket.
     *
     * This is an example of creating a CAN_RAW protocol socket.
     */
    int s = socket(PF_CAN, SOCK_RAW, CAN_RAW);

    /*
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
	struct sockaddr_can addr;
    struct ifreq ifr;
    strcpy(ifr.ifr_name, "can0");
    ioctl(s, SIOCGIFINDEX, &ifr);

    addr.can_family = AF_CAN;
    addr.can_ifindex = ifr.ifr_ifindex;

    bind(s, (struct sockaddr *) &addr, sizeof(addr));

    /*
     * The basic CAN frame structure and the sockaddr structure are defined
     * in include/linux/can.h:
     * 
     * struct can_frame {
     *     canid_t can_id;  // 32 bit CAN_ID + EFF/RTR/ERR flags
     *     __u8    can_dlc; // frame payload length in byte (0 .. 8)
     *     __u8    data[8] __attribute__((aligned(8)));
     * };
     *
     * But use structure canfd_frame to send and receive data.
     * struct canfd_frame {
     * 	canid_t can_id;  // 32 bit CAN_ID + EFF/RTR/ERR flags
     * 	__u8    len;     // frame payload length in byte
     * 	__u8    flags;   // additional flags for CAN FD
     * 	__u8    __res0;  // reserved / padding
     * 	__u8    __res1;  // reserved / padding
     * 	__u8    data[CANFD_MAX_DLEN] __attribute__((aligned(8)));
     * };
     */
	struct canfd_frame frame;
    
	struct iovec iov;
	struct msghdr msg;
	char ctrlmsg[CMSG_SPACE(sizeof(struct timeval)) + CMSG_SPACE(sizeof(__u32))];

	iov.iov_base = &frame;
	msg.msg_name = &addr;
	msg.msg_iov = &iov;
	msg.msg_iovlen = 1;
	msg.msg_control = &ctrlmsg;

    /* these settings may be modified by recvmsg() */
    while (1) {
        iov.iov_len = sizeof(frame);
        msg.msg_namelen = sizeof(addr);
        msg.msg_controllen = sizeof(ctrlmsg);  
        msg.msg_flags = 0;

        int nbytes = recvmsg(s, &msg, 0);
        if (nbytes > 0) {
            printf("result=%d id=%d data(%d)=[", nbytes, frame.can_id, frame.len);
            int i;
            char *comma = "";
            for (i = 0; i < frame.len; ++i) {
                printf("%s%02x", comma, frame.data[i]);
                comma = ",";
            }
            printf("]\n");
        }
    }
}
