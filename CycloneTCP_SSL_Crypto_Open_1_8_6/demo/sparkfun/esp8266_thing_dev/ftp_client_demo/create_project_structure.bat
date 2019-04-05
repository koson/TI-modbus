md src\common
md src\cyclone_tcp
md src\cyclone_tcp\core
md src\cyclone_tcp\drivers
md src\cyclone_tcp\drivers\wifi
md src\cyclone_tcp\ipv4
md src\cyclone_tcp\ipv6
md src\cyclone_tcp\ppp
md src\cyclone_tcp\mibs
md src\cyclone_tcp\dns
md src\cyclone_tcp\mdns
md src\cyclone_tcp\dns_sd
md src\cyclone_tcp\netbios
md src\cyclone_tcp\dhcp
md src\cyclone_tcp\dhcpv6
md src\cyclone_tcp\ftp

REM Common Sources
copy ..\..\..\..\common\cpu_endian.c src\common
copy ..\..\..\..\common\os_port_freertos.c src\common
copy ..\..\..\..\common\date_time.c src\common
copy ..\..\..\..\common\str.c src\common
copy ..\..\..\..\common\debug.c src\common

REM Common Headers
copy ..\..\..\..\common\cpu_endian.h src\common
copy ..\..\..\..\common\compiler_port.h src\common
copy ..\..\..\..\common\os_port.h src\common
copy ..\..\..\..\common\os_port_freertos.h src\common
copy ..\..\..\..\common\date_time.h src\common
copy ..\..\..\..\common\str.h src\common
copy ..\..\..\..\common\error.h src\common
copy ..\..\..\..\common\debug.h src\common

REM CycloneTCP Sources
copy ..\..\..\..\cyclone_tcp\drivers\wifi\esp8266_driver.c src\cyclone_tcp\drivers\wifi
copy ..\..\..\..\cyclone_tcp\core\*.c src\cyclone_tcp\core
copy ..\..\..\..\cyclone_tcp\ipv4\*.c src\cyclone_tcp\ipv4
copy ..\..\..\..\cyclone_tcp\ipv6\*.c src\cyclone_tcp\ipv6
copy ..\..\..\..\cyclone_tcp\dns\*.c src\cyclone_tcp\dns
copy ..\..\..\..\cyclone_tcp\mdns\*.c src\cyclone_tcp\mdns
copy ..\..\..\..\cyclone_tcp\netbios\*.c src\cyclone_tcp\netbios
copy ..\..\..\..\cyclone_tcp\dhcp\*.c src\cyclone_tcp\dhcp
copy ..\..\..\..\cyclone_tcp\ftp\ftp_client.c src\cyclone_tcp\ftp

REM CycloneTCP Headers
copy ..\..\..\..\cyclone_tcp\drivers\wifi\esp8266_driver.h src\cyclone_tcp\drivers\wifi
copy ..\..\..\..\cyclone_tcp\core\*.h src\cyclone_tcp\core
copy ..\..\..\..\cyclone_tcp\ipv4\*.h src\cyclone_tcp\ipv4
copy ..\..\..\..\cyclone_tcp\ipv6\*.h src\cyclone_tcp\ipv6
copy ..\..\..\..\cyclone_tcp\ppp\*.h src\cyclone_tcp\ppp
copy ..\..\..\..\cyclone_tcp\mibs\*.h src\cyclone_tcp\mibs
copy ..\..\..\..\cyclone_tcp\dns\*.h src\cyclone_tcp\dns
copy ..\..\..\..\cyclone_tcp\mdns\*.h src\cyclone_tcp\mdns
copy ..\..\..\..\cyclone_tcp\dns_sd\*.h src\cyclone_tcp\dns_sd
copy ..\..\..\..\cyclone_tcp\netbios\*.h src\cyclone_tcp\netbios
copy ..\..\..\..\cyclone_tcp\dhcp\*.h src\cyclone_tcp\dhcp
copy ..\..\..\..\cyclone_tcp\dhcpv6\*.h src\cyclone_tcp\dhcpv6
copy ..\..\..\..\cyclone_tcp\ftp\ftp_client.h src\cyclone_tcp\ftp
