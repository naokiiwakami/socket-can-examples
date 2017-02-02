#include <stdio.h>
#include <sys/socket.h>
#include <linux/can.h>

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
    struct can_frame send_frame;

    /*
     * Range of CAN ID for a standard frame is [0..2047].
     */
    send_frame.can_id = 0x5AD;
    send_frame.can_dlc = 4;
    send_frame.data[0] = 0xde;
    send_frame.data[1] = 0xad;
    send_frame.data[2] = 0xbe;
    send_frame.data[3] = 0xef;
    
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
        // monitor this by logic analyzer
        nbytes = write(s, &send_frame, sizeof(struct can_frame));
        printf("write result=%d\n", nbytes);
        
        /**
         * You can also send an extended CAN frame by setting flag CAN_EFF_FLAG(=0x80000000U)
         * to the can_id field.
         * Available flags for can_id are:
         * #define CAN_EFF_FLAG 0x80000000U // EFF/SFF is set in the MSB
         * #define CAN_RTR_FLAG 0x40000000U // remote transmission request
         * #define CAN_ERR_FLAG 0x20000000U // error frame 
         */
        send_frame.can_id = 0xD06F00D | CAN_EFF_FLAG;
    }
}
