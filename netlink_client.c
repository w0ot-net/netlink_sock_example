#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <linux/netlink.h>
#include <unistd.h>

#define MAX_PAYLOAD 1024 

int main(int argc, char **argv) {
    
    int sock_fd;
    int pid;
    struct nlmsghdr *nlh = NULL;
    struct iovec iov;
    struct msghdr msg;
    struct sockaddr_nl src_addr, dest_addr;

    if (argc < 2) {
        printf("Usage: %s <pid>\n", argv[0]);
        printf("<pid> should be the portid of the listening socket,\n"
               "which is coincidentally the process id of the server\n"
               "program in this example.\n");
        exit(-1);
    }

    pid = atoi(argv[1]);
    
    sock_fd = socket(AF_NETLINK, SOCK_RAW, NETLINK_USERSOCK);
    if (sock_fd < 0) {
        perror("[-] Error creating socket");
        return -1;
    }   
    printf("[+] netlink socket created\n");

    // pepare the dest_addr structure
    memset(&dest_addr, 0, sizeof(dest_addr));
    dest_addr.nl_family = AF_NETLINK;
    dest_addr.nl_pid = pid;  // needs to be the portid used by the receiver
    dest_addr.nl_groups = 0; // unicast

    // prepapre the nlmsghdr
    nlh = (struct nlmsghdr *)malloc(NLMSG_SPACE(MAX_PAYLOAD));
    memset(nlh, 0, NLMSG_SPACE(MAX_PAYLOAD));
    nlh->nlmsg_len = NLMSG_SPACE(MAX_PAYLOAD);
    nlh->nlmsg_pid = getpid();
    nlh->nlmsg_flags = 0;

    // prepare msg and iovev
    //strcpy((char *) NLMSG_DATA(nlh), "Hello Netlink!");
    memset(&msg, 0, sizeof(struct msghdr));
    memset(NLMSG_DATA(nlh), 0x41, MAX_PAYLOAD);
    iov.iov_base = (void *)nlh;
    iov.iov_len = nlh->nlmsg_len;
    msg.msg_name = (void *)&dest_addr;
    msg.msg_namelen = sizeof(dest_addr);
    msg.msg_iov = &iov;
    msg.msg_iovlen = 1;

    // send
    printf("[+] sending message: %s\n", (char *)NLMSG_DATA(nlh));
    if (sendmsg(sock_fd, &msg, 0 /*MSG_DONTWAIT*/) < 0) {
        perror("[-] failed to send message");
    }
}
