ESPTOOL = esptool
ESPPORT = COM1
ESPBAUD = 921600

PROGRAM = web_server_demo
PROGRAM_CFLAGS = -D__error_t_defined -D_WINSOCK_H

PROGRAM_INC_DIR = src \
	src/common \
	src/cyclone_tcp

PROGRAM_SRC_DIR = src \
	src/common \
	src/cyclone_tcp/drivers/wifi \
	src/cyclone_tcp/core \
	src/cyclone_tcp/ipv4 \
	src/cyclone_tcp/ipv6 \
	src/cyclone_tcp/dhcp \
	src/cyclone_tcp/dns \
	src/cyclone_tcp/mdns \
	src/cyclone_tcp/netbios \
	src/cyclone_tcp/http

COMPONENTS = FreeRTOS lwip core open_esplibs

include $(ESP_OPEN_RTOS_PATH)/common.mk
