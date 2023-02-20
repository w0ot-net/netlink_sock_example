#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <linux/netlink.h>
#include <unistd.h>

#define MAX_PAYLOAD 1024 

int main() {
    int sock_fd;
    struct nlmsghdr *nlh = NULL;
    struct iovec iov;
    struct msghdr msg;
    struct sockaddr_nl src_addr, dest_addr;

    printf("[+] got pid: %d\n", getpid());

    sock_fd = socket(AF_NETLINK, SOCK_RAW, NETLINK_USERSOCK);
    if (sock_fd < 0) {
        perror("Error creating socket");
        return -1;
    }   
    printf("[+] netlink socket created\n");

    // prepare the src_addr structure
    memset(&src_addr, 0, sizeof(src_addr));
    src_addr.nl_family = AF_NETLINK;
    src_addr.nl_pid = getpid(); //pthread_self();
    src_addr.nl_groups = 0; // 0 means no multicast

    if (bind(sock_fd, (struct sockaddr*) &src_addr, sizeof(src_addr)) < 0) {
        perror("Error binding socket");
        return -1;
    }
    printf("[+] netlink socket bound\n");

    // pepare the dest_addr structure
    memset(&dest_addr, 0, sizeof(dest_addr));
    dest_addr.nl_family = AF_NETLINK;
    dest_addr.nl_pid = getpid();
    dest_addr.nl_groups = 0; // unicast       
    
    // prepapre the nlmsghdr
    nlh = (struct nlmsghdr *)malloc(NLMSG_SPACE(MAX_PAYLOAD));
    memset(nlh, 0, NLMSG_SPACE(MAX_PAYLOAD));
    nlh->nlmsg_len = NLMSG_SPACE(MAX_PAYLOAD);
    nlh->nlmsg_pid = getpid();
    nlh->nlmsg_flags = 0;
    

    // prepare msg and iovev
    memset(&msg, 0, sizeof(struct msghdr));
    iov.iov_base = (void *)nlh;
    iov.iov_len = nlh->nlmsg_len;
    msg.msg_name = (void *)&dest_addr;
    msg.msg_namelen = sizeof(dest_addr);
    msg.msg_iov = &iov;
    msg.msg_iovlen = 1;

    // recv 4eva
    printf("[+] listening for messages\n");
    while (1) {
        recvmsg(sock_fd, &msg, 0);
        printf("Received message payload: %s\n", NLMSG_DATA(nlh));
    }
}
