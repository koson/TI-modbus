RESULT ?= ftp_server_demo

DEFINES = \
	-DSTM32F4XX \
	-DUSE_STM32F4_DISCOVERY \
	-DUSE_STDPERIPH_DRIVER \
	-D_WINSOCK_H \
	-D__error_t_defined

INCLUDES = \
	-I../src \
	-I../../../../../third_party/cmsis/include \
	-I../../../../../third_party/st/devices/stm32f4xx \
	-I../../../../../third_party/st/boards \
	-I../../../../../third_party/st/boards/common \
	-I../../../../../third_party/st/boards/stm32f4_discovery \
	-I../../../../../third_party/st/stm32f4xx_std_periph_lib/inc \
	-I../../../../../third_party/freertos/include \
	-I../../../../../third_party/freertos/portable/gcc/arm_cm4f \
	-I../../../../../third_party/fatfs \
	-I../../../../../common \
	-I../../../../../cyclone_tcp \
	-I../../../../../cyclone_ssl \
	-I../../../../../cyclone_crypto

ASM_SOURCES = \
	./startup_stm32f4xx.S

C_SOURCES = \
	../src/system_stm32f4xx.c \
	../src/stm32f4xx_it.c \
	./syscalls.c \
	../src/main.c \
	../src/fatfs_drv.c \
	../src/debug.c \
	../../../../../common/cpu_endian.c \
	../../../../../common/os_port_freertos.c \
	../../../../../common/fs_port_fatfs.c \
	../../../../../common/date_time.c \
	../../../../../common/str.c \
	../../../../../common/path.c \
	../../../../../cyclone_tcp/core/net.c \
	../../../../../cyclone_tcp/core/net_mem.c \
	../../../../../cyclone_tcp/drivers/mac/stm32f4x7_eth_driver.c \
	../../../../../cyclone_tcp/drivers/phy/lan8720_driver.c \
	../../../../../cyclone_tcp/core/nic.c \
	../../../../../cyclone_tcp/core/ethernet.c \
	../../../../../cyclone_tcp/ipv4/arp.c \
	../../../../../cyclone_tcp/ipv4/ipv4.c \
	../../../../../cyclone_tcp/ipv4/ipv4_frag.c \
	../../../../../cyclone_tcp/ipv4/icmp.c \
	../../../../../cyclone_tcp/ipv4/igmp.c \
	../../../../../cyclone_tcp/ipv6/ipv6.c \
	../../../../../cyclone_tcp/ipv6/ipv6_frag.c \
	../../../../../cyclone_tcp/ipv6/ipv6_misc.c \
	../../../../../cyclone_tcp/ipv6/ipv6_pmtu.c \
	../../../../../cyclone_tcp/ipv6/icmpv6.c \
	../../../../../cyclone_tcp/ipv6/mld.c \
	../../../../../cyclone_tcp/ipv6/ndp.c \
	../../../../../cyclone_tcp/ipv6/ndp_cache.c \
	../../../../../cyclone_tcp/ipv6/ndp_misc.c \
	../../../../../cyclone_tcp/ipv6/slaac.c \
	../../../../../cyclone_tcp/core/ip.c \
	../../../../../cyclone_tcp/core/tcp.c \
	../../../../../cyclone_tcp/core/tcp_fsm.c \
	../../../../../cyclone_tcp/core/tcp_misc.c \
	../../../../../cyclone_tcp/core/tcp_timer.c \
	../../../../../cyclone_tcp/core/udp.c \
	../../../../../cyclone_tcp/core/socket.c \
	../../../../../cyclone_tcp/core/bsd_socket.c \
	../../../../../cyclone_tcp/core/raw_socket.c \
	../../../../../cyclone_tcp/dns/dns_cache.c \
	../../../../../cyclone_tcp/dns/dns_client.c \
	../../../../../cyclone_tcp/dns/dns_common.c \
	../../../../../cyclone_tcp/dns/dns_debug.c \
	../../../../../cyclone_tcp/mdns/mdns_client.c \
	../../../../../cyclone_tcp/mdns/mdns_responder.c \
	../../../../../cyclone_tcp/mdns/mdns_common.c \
	../../../../../cyclone_tcp/netbios/nbns_client.c \
	../../../../../cyclone_tcp/netbios/nbns_responder.c \
	../../../../../cyclone_tcp/netbios/nbns_common.c \
	../../../../../cyclone_tcp/dhcp/dhcp_client.c \
	../../../../../cyclone_tcp/dhcp/dhcp_common.c \
	../../../../../cyclone_tcp/dhcp/dhcp_debug.c \
	../../../../../cyclone_tcp/ftp/ftp_server.c \
	../../../../../cyclone_tcp/ftp/ftp_server_events.c \
	../../../../../cyclone_tcp/ftp/ftp_server_commands.c \
	../../../../../cyclone_tcp/ftp/ftp_server_misc.c \
	../../../../../third_party/freertos/portable/gcc/arm_cm4f/port.c \
	../../../../../third_party/freertos/croutine.c \
	../../../../../third_party/freertos/list.c \
	../../../../../third_party/freertos/queue.c \
	../../../../../third_party/freertos/tasks.c \
	../../../../../third_party/freertos/timers.c \
	../../../../../third_party/freertos/portable/memmang/heap_3.c \
	../../../../../third_party/fatfs/ff.c \
	../../../../../third_party/fatfs/option/unicode.c \
	../../../../../third_party/st/boards/stm32f4_discovery/stm32f4_discovery.c \
	../../../../../third_party/st/boards/stm32f4_discovery/stm32f4_discovery_lcd.c \
	../../../../../third_party/st/boards/stm32f4_discovery/stm32f4_discovery_sdio_sd.c \
	../../../../../third_party/st/stm32f4xx_std_periph_lib/src/misc.c \
	../../../../../third_party/st/stm32f4xx_std_periph_lib/src/stm32f4xx_adc.c \
	../../../../../third_party/st/stm32f4xx_std_periph_lib/src/stm32f4xx_can.c \
	../../../../../third_party/st/stm32f4xx_std_periph_lib/src/stm32f4xx_crc.c \
	../../../../../third_party/st/stm32f4xx_std_periph_lib/src/stm32f4xx_cryp.c \
	../../../../../third_party/st/stm32f4xx_std_periph_lib/src/stm32f4xx_cryp_aes.c \
	../../../../../third_party/st/stm32f4xx_std_periph_lib/src/stm32f4xx_cryp_des.c \
	../../../../../third_party/st/stm32f4xx_std_periph_lib/src/stm32f4xx_cryp_tdes.c \
	../../../../../third_party/st/stm32f4xx_std_periph_lib/src/stm32f4xx_dac.c \
	../../../../../third_party/st/stm32f4xx_std_periph_lib/src/stm32f4xx_dbgmcu.c \
	../../../../../third_party/st/stm32f4xx_std_periph_lib/src/stm32f4xx_dcmi.c \
	../../../../../third_party/st/stm32f4xx_std_periph_lib/src/stm32f4xx_dma.c \
	../../../../../third_party/st/stm32f4xx_std_periph_lib/src/stm32f4xx_exti.c \
	../../../../../third_party/st/stm32f4xx_std_periph_lib/src/stm32f4xx_flash.c \
	../../../../../third_party/st/stm32f4xx_std_periph_lib/src/stm32f4xx_fsmc.c \
	../../../../../third_party/st/stm32f4xx_std_periph_lib/src/stm32f4xx_gpio.c \
	../../../../../third_party/st/stm32f4xx_std_periph_lib/src/stm32f4xx_hash.c \
	../../../../../third_party/st/stm32f4xx_std_periph_lib/src/stm32f4xx_hash_md5.c \
	../../../../../third_party/st/stm32f4xx_std_periph_lib/src/stm32f4xx_hash_sha1.c \
	../../../../../third_party/st/stm32f4xx_std_periph_lib/src/stm32f4xx_i2c.c \
	../../../../../third_party/st/stm32f4xx_std_periph_lib/src/stm32f4xx_iwdg.c \
	../../../../../third_party/st/stm32f4xx_std_periph_lib/src/stm32f4xx_pwr.c \
	../../../../../third_party/st/stm32f4xx_std_periph_lib/src/stm32f4xx_rcc.c \
	../../../../../third_party/st/stm32f4xx_std_periph_lib/src/stm32f4xx_rng.c \
	../../../../../third_party/st/stm32f4xx_std_periph_lib/src/stm32f4xx_rtc.c \
	../../../../../third_party/st/stm32f4xx_std_periph_lib/src/stm32f4xx_sdio.c \
	../../../../../third_party/st/stm32f4xx_std_periph_lib/src/stm32f4xx_spi.c \
	../../../../../third_party/st/stm32f4xx_std_periph_lib/src/stm32f4xx_syscfg.c \
	../../../../../third_party/st/stm32f4xx_std_periph_lib/src/stm32f4xx_tim.c \
	../../../../../third_party/st/stm32f4xx_std_periph_lib/src/stm32f4xx_usart.c \
	../../../../../third_party/st/stm32f4xx_std_periph_lib/src/stm32f4xx_wwdg.c

HEADERS = \
	../src/os_port_config.h \
	../src/fs_port_config.h \
	../src/net_config.h \
	../src/FreeRTOSConfig.h \
	../src/ffconf.h \
	../src/stm32f4xx_conf.h \
	../src/stm32f4xx_it.h \
	../../../../../common/cpu_endian.h \
	../../../../../common/os_port.h \
	../../../../../common/os_port_freertos.h \
	../../../../../common/fs_port.h \
	../../../../../common/date_time.h \
	../../../../../common/str.h \
	../../../../../common/path.h \
	../../../../../common/error.h \
	../../../../../common/debug.h \
	../../../../../cyclone_tcp/core/net.h \
	../../../../../cyclone_tcp/core/net_mem.h \
	../../../../../cyclone_tcp/drivers/mac/stm32f4x7_eth_driver.h \
	../../../../../cyclone_tcp/drivers/phy/lan8720_driver.h \
	../../../../../cyclone_tcp/core/nic.h \
	../../../../../cyclone_tcp/core/ethernet.h \
	../../../../../cyclone_tcp/ipv4/arp.h \
	../../../../../cyclone_tcp/ipv4/ipv4.h \
	../../../../../cyclone_tcp/ipv4/ipv4_frag.h \
	../../../../../cyclone_tcp/ipv4/icmp.h \
	../../../../../cyclone_tcp/ipv4/igmp.h \
	../../../../../cyclone_tcp/ipv6/ipv6.h \
	../../../../../cyclone_tcp/ipv6/ipv6_frag.h \
	../../../../../cyclone_tcp/ipv6/ipv6_misc.h \
	../../../../../cyclone_tcp/ipv6/ipv6_pmtu.h \
	../../../../../cyclone_tcp/ipv6/icmpv6.h \
	../../../../../cyclone_tcp/ipv6/mld.h \
	../../../../../cyclone_tcp/ipv6/ndp.h \
	../../../../../cyclone_tcp/ipv6/ndp_cache.h \
	../../../../../cyclone_tcp/ipv6/ndp_misc.h \
	../../../../../cyclone_tcp/ipv6/slaac.h \
	../../../../../cyclone_tcp/core/ip.h \
	../../../../../cyclone_tcp/core/tcp.h \
	../../../../../cyclone_tcp/core/tcp_fsm.h \
	../../../../../cyclone_tcp/core/tcp_misc.h \
	../../../../../cyclone_tcp/core/tcp_timer.h \
	../../../../../cyclone_tcp/core/udp.h \
	../../../../../cyclone_tcp/core/socket.h \
	../../../../../cyclone_tcp/core/bsd_socket.h \
	../../../../../cyclone_tcp/core/raw_socket.h \
	../../../../../cyclone_tcp/dns/dns_cache.h \
	../../../../../cyclone_tcp/dns/dns_client.h \
	../../../../../cyclone_tcp/dns/dns_common.h \
	../../../../../cyclone_tcp/dns/dns_debug.h \
	../../../../../cyclone_tcp/mdns/mdns_client.h \
	../../../../../cyclone_tcp/mdns/mdns_responder.h \
	../../../../../cyclone_tcp/mdns/mdns_common.h \
	../../../../../cyclone_tcp/netbios/nbns_client.h \
	../../../../../cyclone_tcp/netbios/nbns_responder.h \
	../../../../../cyclone_tcp/netbios/nbns_common.h \
	../../../../../cyclone_tcp/dhcp/dhcp_client.h \
	../../../../../cyclone_tcp/dhcp/dhcp_common.h \
	../../../../../cyclone_tcp/dhcp/dhcp_debug.h \
	../../../../../cyclone_tcp/ftp/ftp_server.h \
	../../../../../cyclone_tcp/ftp/ftp_server_events.h \
	../../../../../cyclone_tcp/ftp/ftp_server_commands.h \
	../../../../../cyclone_tcp/ftp/ftp_server_misc.h \
	../../../../../third_party/freertos/portable/gcc/arm_cm4f/portmacro.h \
	../../../../../third_party/freertos/include/croutine.h \
	../../../../../third_party/freertos/include/FreeRTOS.h \
	../../../../../third_party/freertos/include/list.h \
	../../../../../third_party/freertos/include/mpu_wrappers.h \
	../../../../../third_party/freertos/include/portable.h \
	../../../../../third_party/freertos/include/projdefs.h \
	../../../../../third_party/freertos/include/queue.h \
	../../../../../third_party/freertos/include/semphr.h \
	../../../../../third_party/freertos/include/stack_macros.h \
	../../../../../third_party/freertos/include/task.h \
	../../../../../third_party/freertos/include/timers.h \
	../../../../../third_party/fatfs/diskio.h \
	../../../../../third_party/fatfs/ff.h \
	../../../../../third_party/fatfs/integer.h \
	../../../../../third_party/st/boards/stm32f4_discovery/stm32f4_discovery.h \
	../../../../../third_party/st/boards/stm32f4_discovery/stm32f4_discovery_lcd.h \
	../../../../../third_party/st/boards/stm32f4_discovery/stm32f4_discovery_sdio_sd.h \
	../../../../../third_party/st/stm32f4xx_std_periph_lib/inc/misc.h \
	../../../../../third_party/st/stm32f4xx_std_periph_lib/inc/stm32f4xx_adc.h \
	../../../../../third_party/st/stm32f4xx_std_periph_lib/inc/stm32f4xx_can.h \
	../../../../../third_party/st/stm32f4xx_std_periph_lib/inc/stm32f4xx_crc.h \
	../../../../../third_party/st/stm32f4xx_std_periph_lib/inc/stm32f4xx_cryp.h \
	../../../../../third_party/st/stm32f4xx_std_periph_lib/inc/stm32f4xx_dac.h \
	../../../../../third_party/st/stm32f4xx_std_periph_lib/inc/stm32f4xx_dbgmcu.h \
	../../../../../third_party/st/stm32f4xx_std_periph_lib/inc/stm32f4xx_dcmi.h \
	../../../../../third_party/st/stm32f4xx_std_periph_lib/inc/stm32f4xx_dma.h \
	../../../../../third_party/st/stm32f4xx_std_periph_lib/inc/stm32f4xx_exti.h \
	../../../../../third_party/st/stm32f4xx_std_periph_lib/inc/stm32f4xx_flash.h \
	../../../../../third_party/st/stm32f4xx_std_periph_lib/inc/stm32f4xx_fsmc.h \
	../../../../../third_party/st/stm32f4xx_std_periph_lib/inc/stm32f4xx_gpio.h \
	../../../../../third_party/st/stm32f4xx_std_periph_lib/inc/stm32f4xx_hash.h \
	../../../../../third_party/st/stm32f4xx_std_periph_lib/inc/stm32f4xx_i2c.h \
	../../../../../third_party/st/stm32f4xx_std_periph_lib/inc/stm32f4xx_iwdg.h \
	../../../../../third_party/st/stm32f4xx_std_periph_lib/inc/stm32f4xx_pwr.h \
	../../../../../third_party/st/stm32f4xx_std_periph_lib/inc/stm32f4xx_rcc.h \
	../../../../../third_party/st/stm32f4xx_std_periph_lib/inc/stm32f4xx_rng.h \
	../../../../../third_party/st/stm32f4xx_std_periph_lib/inc/stm32f4xx_rtc.h \
	../../../../../third_party/st/stm32f4xx_std_periph_lib/inc/stm32f4xx_sdio.h \
	../../../../../third_party/st/stm32f4xx_std_periph_lib/inc/stm32f4xx_spi.h \
	../../../../../third_party/st/stm32f4xx_std_periph_lib/inc/stm32f4xx_syscfg.h \
	../../../../../third_party/st/stm32f4xx_std_periph_lib/inc/stm32f4xx_tim.h \
	../../../../../third_party/st/stm32f4xx_std_periph_lib/inc/stm32f4xx_usart.h \
	../../../../../third_party/st/stm32f4xx_std_periph_lib/inc/stm32f4xx_wwdg.h

ASM_OBJECTS = $(patsubst %.S, %.o, $(ASM_SOURCES))

C_OBJECTS = $(patsubst %.c, %.o, $(C_SOURCES))

OBJ_DIR = obj

LINKER_SCRIPT = stm32f407_flash.ld

CFLAGS += -fno-common -Wall -Os -g3
CFLAGS += -mcpu=cortex-m4 -mthumb -mfpu=fpv4-sp-d16 -mfloat-abi=softfp
CFLAGS += -ffunction-sections -fdata-sections -Wl,--gc-sections
CFLAGS += $(DEFINES)
CFLAGS += $(INCLUDES)

CROSS_COMPILE ?= arm-none-eabi-
CC = $(CROSS_COMPILE)gcc
LD = $(CROSS_COMPILE)ld
OBJDUMP = $(CROSS_COMPILE)objdump
OBJCOPY = $(CROSS_COMPILE)objcopy
SIZE = $(CROSS_COMPILE)size

THIS_MAKEFILE := $(lastword $(MAKEFILE_LIST))

all: build size

install: build size program

build: $(RESULT).elf $(RESULT).lst $(RESULT).bin $(RESULT).hex
	
$(RESULT).elf: $(ASM_OBJECTS) $(C_OBJECTS) $(HEADERS) $(LINKER_SCRIPT) $(THIS_MAKEFILE)
	$(CC) -Wl,-M=$(RESULT).map -Wl,-T$(LINKER_SCRIPT) $(CFLAGS) $(addprefix $(OBJ_DIR)/, $(notdir $(ASM_OBJECTS))) $(addprefix $(OBJ_DIR)/, $(notdir $(C_OBJECTS))) -o $@

$(ASM_OBJECTS): | $(OBJ_DIR)

$(C_OBJECTS): | $(OBJ_DIR)

$(OBJ_DIR):
	mkdir -p $@

%.o: %.c $(HEADERS) $(THIS_MAKEFILE)
	$(CC) $(CFLAGS) -c $< -o $(addprefix $(OBJ_DIR)/, $(notdir $@))

%.o: %.S $(HEADERS) $(THIS_MAKEFILE)
	$(CC) $(CFLAGS) -c $< -o $(addprefix $(OBJ_DIR)/, $(notdir $@))

%.lst: %.elf
	$(OBJDUMP) -x -S $(RESULT).elf > $@

%.bin: %.elf
	$(OBJCOPY) -O binary $< $@

%.hex: %.elf
	$(OBJCOPY) -O ihex $< $@

size: $(RESULT).elf
	$(SIZE) $(RESULT).elf

flash:
	openocd -f board/stm32f4discovery.cfg -c "init; reset halt; flash write_image erase $(RESULT).bin 0x08000000; reset run; shutdown"

clean:
	rm -f $(RESULT).elf
	rm -f $(RESULT).bin
	rm -f $(RESULT).map
	rm -f $(RESULT).hex
	rm -f $(RESULT).lst
	rm -f $(OBJ_DIR)/*.o
