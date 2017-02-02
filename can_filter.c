#include <stdio.h>
#include <sys/socket.h>
#include <linux/can.h>
#include <linux/can/raw.h>

#include "can_utils.h"

/**
 * This program opens a CAN socket and keeps reading imcoming CAN frames through the socket.
 */

int main(int argc, char *argv[])
{
    /**
     * In order to use SocketCAN, you first need to create a socket and bind
     * it to a CAN interface. See can_open_socket.c for steps to initialize a socket.
     */
    int s = can_open_socket("can0");

    /**
     * The reception of CAN frames using CAN_RAW sockets can be controlled
     * by defining 0 .. n filters with the CAN_RAW_FILTER socket option.
     *
     * The CAN filter structure is defined in include/linux/can.h:
     *
     *     struct can_filter {
     *             canid_t can_id;
     *             canid_t can_mask;
     *     };
     * 
     *   A filter matches, when
     *     <received_can_id> & mask == can_id & mask
     *   which is analogous to known CAN controllers hardware filter semantics.
     */
    struct can_filter rfilter[1];
    rfilter[0].can_id = 256;
    rfilter[0].can_mask = CAN_SFF_MASK;
    /*
      rfilter[1].can_id = 255;
      rfilter[1].can_mask = CAN_SFF_MASK;
    */
    setsockopt(s, SOL_CAN_RAW, CAN_RAW_FILTER, &rfilter, sizeof(rfilter));

    /**
     * The basic CAN frame structure and the sockaddr structure are defined
     * in include/linux/can.h:
     * 
     * struct can_frame {
     *     canid_t can_id;  // 32 bit CAN_ID + EFF/RTR/ERR flags
     *     __u8    can_dlc; // frame payload length in byte (0 .. 8)
     *     __u8    __pad;   // padding
     *     __u8    __res0;  // reserved / padding
     *     __u8    __res1;  // reserved / padding
     *     __u8    data[8] __attribute__((aligned(8)));
     * };
     */
    struct can_frame recv_frame;

    while (1) {
        /**
         * Reading CAN frames from a bound CAN_RAW socket consists of
         * reading a struct can_frame.
         */
        int nbytes = read(s, &recv_frame, sizeof(struct can_frame));
        if (nbytes > 0) {
            printf("result=%d id=%d data(%d)=[", nbytes, recv_frame.can_id, recv_frame.can_dlc);
            int i;
            char *comma = "";
            for (i = 0; i < recv_frame.can_dlc; ++i) {
                printf("%s%02x", comma, recv_frame.data[i]);
                comma = ",";
            }
            printf("]\n");
        }
    }
}
