CFLAGS += -DESP32_DEV_KIT_C -D_WINSOCK_H -D__error_t_defined

COMPONENT_SRCDIRS := . \
	common \
	cyclone_tcp/drivers/mac \
	cyclone_tcp/drivers/phy \
	cyclone_tcp/drivers/wifi \
	cyclone_tcp/core \
	cyclone_tcp/dhcp \
	cyclone_tcp/dns \
	cyclone_tcp/ipv4 \
	cyclone_tcp/ipv6 \
	cyclone_tcp/netbios \
	cyclone_tcp/mqtt \
	cyclone_tcp/web_socket \
	cyclone_ssl \
	cyclone_crypto/core \
	cyclone_crypto/hash \
	cyclone_crypto/mac \
	cyclone_crypto/cipher \
	cyclone_crypto/cipher_mode \
	cyclone_crypto/aead \
	cyclone_crypto/pkc \
	cyclone_crypto/ecc \
	cyclone_crypto/mpi \
	cyclone_crypto/encoding \
	cyclone_crypto/certificate \
	cyclone_crypto/rng

COMPONENT_ADD_INCLUDEDIRS := . \
	common \
	cyclone_tcp \
	cyclone_ssl \
	cyclone_crypto
