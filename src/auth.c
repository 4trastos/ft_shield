#include "ft_shield.h"

// Hash SHA256 de "903100"
static const unsigned char	g_hash[32] = {
	0xd4, 0x78, 0xff, 0xfe, 0x8d, 0xe2, 0xcb, 0xde,
	0x1c, 0xb0, 0x6a, 0xd6, 0x63, 0xd8, 0x13, 0xdb,
	0xf1, 0x4a, 0x0e, 0xc4, 0x79, 0x42, 0x62, 0x9b,
	0x36, 0xb4, 0xf3, 0x4c, 0xd4, 0x77, 0x6c, 0xcd
};

void    ft_sha256(const char *input, size_t len, unsigned char output[32]);

int	ft_auth(int client_fd)
{
	char			buffer[256];
	unsigned char	hash[32];
	int				bytes;
	int				attempts;
	int				i;

	attempts = 0;
	while (attempts < 3)
	{
		write(client_fd, "Keycode: ", 9);

		memset(buffer, 0, sizeof(buffer));
		bytes = read(client_fd, buffer, sizeof(buffer) - 1);
		if (bytes <= 0)
			return (-1);

		if (buffer[bytes - 1] == '\n')
			buffer[--bytes] = '\0';
		if (bytes > 0 && buffer[bytes - 1] == '\r')
			buffer[--bytes] = '\0';

		ft_sha256(buffer, bytes, hash);

		i = 0;
		while (i < 32 && hash[i] == g_hash[i])
			i++;
		if (i == 32)
			return (0);

		attempts++;
	}
	return (-1);
}

int ft_verify_hash(const unsigned char *incoming_hash)
{
    int i = 0;

    while (i < 32 && incoming_hash[i] == g_hash[i])
        i++;
    
    if (i == 32)
        return (0);                 // OK
    return (-1);                    // KO
}