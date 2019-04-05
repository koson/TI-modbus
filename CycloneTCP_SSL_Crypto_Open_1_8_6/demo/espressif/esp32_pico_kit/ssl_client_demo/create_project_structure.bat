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
md src\cyclone_ssl
md src\cyclone_crypto
md src\cyclone_crypto\core
md src\cyclone_crypto\hash
md src\cyclone_crypto\mac
md src\cyclone_crypto\cipher
md src\cyclone_crypto\cipher_mode
md src\cyclone_crypto\aead
md src\cyclone_crypto\pkc
md src\cyclone_crypto\ecc
md src\cyclone_crypto\mpi
md src\cyclone_crypto\encoding
md src\cyclone_crypto\certificate
md src\cyclone_crypto\rng

REM Common Sources
copy ..\..\..\..\common\cpu_endian.c src\common
copy ..\..\..\..\common\os_port_freertos.c src\common
copy ..\..\..\..\common\date_time.c src\common
copy ..\..\..\..\common\str.c src\common
copy ..\..\..\..\common\resource_manager.c src\common
copy ..\..\..\..\common\debug.c src\common

REM Common Headers
copy ..\..\..\..\common\cpu_endian.h src\common
copy ..\..\..\..\common\compiler_port.h src\common
copy ..\..\..\..\common\os_port.h src\common
copy ..\..\..\..\common\os_port_freertos.h src\common
copy ..\..\..\..\common\date_time.h src\common
copy ..\..\..\..\common\str.h src\common
copy ..\..\..\..\common\resource_manager.h src\common
copy ..\..\..\..\common\error.h src\common
copy ..\..\..\..\common\debug.h src\common

REM CycloneTCP Sources
copy ..\..\..\..\cyclone_tcp\drivers\wifi\esp32_wifi_driver.c src\cyclone_tcp\drivers\wifi
copy ..\..\..\..\cyclone_tcp\core\*.c src\cyclone_tcp\core
copy ..\..\..\..\cyclone_tcp\ipv4\*.c src\cyclone_tcp\ipv4
copy ..\..\..\..\cyclone_tcp\ipv6\*.c src\cyclone_tcp\ipv6
copy ..\..\..\..\cyclone_tcp\dns\*.c src\cyclone_tcp\dns
copy ..\..\..\..\cyclone_tcp\mdns\*.c src\cyclone_tcp\mdns
copy ..\..\..\..\cyclone_tcp\netbios\*.c src\cyclone_tcp\netbios
copy ..\..\..\..\cyclone_tcp\dhcp\*.c src\cyclone_tcp\dhcp

REM CycloneTCP Headers
copy ..\..\..\..\cyclone_tcp\drivers\wifi\esp32_wifi_driver.h src\cyclone_tcp\drivers\wifi
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

REM CycloneSSL Sources
copy ..\..\..\..\cyclone_ssl\*.c src\cyclone_ssl

REM CycloneSSL Headers
copy ..\..\..\..\cyclone_ssl\*.h src\cyclone_ssl

REM CycloneCrypto Sources
copy ..\..\..\..\cyclone_crypto\hash\md5.c src\cyclone_crypto\hash
copy ..\..\..\..\cyclone_crypto\hash\sha1.c src\cyclone_crypto\hash
copy ..\..\..\..\cyclone_crypto\hash\sha224.c src\cyclone_crypto\hash
copy ..\..\..\..\cyclone_crypto\hash\sha256.c src\cyclone_crypto\hash
copy ..\..\..\..\cyclone_crypto\hash\sha384.c src\cyclone_crypto\hash
copy ..\..\..\..\cyclone_crypto\hash\sha512.c src\cyclone_crypto\hash
copy ..\..\..\..\cyclone_crypto\mac\hmac.c src\cyclone_crypto\mac
copy ..\..\..\..\cyclone_crypto\cipher\rc4.c src\cyclone_crypto\cipher
copy ..\..\..\..\cyclone_crypto\cipher\idea.c src\cyclone_crypto\cipher
copy ..\..\..\..\cyclone_crypto\cipher\des.c src\cyclone_crypto\cipher
copy ..\..\..\..\cyclone_crypto\cipher\des3.c src\cyclone_crypto\cipher
copy ..\..\..\..\cyclone_crypto\cipher\aes.c src\cyclone_crypto\cipher
copy ..\..\..\..\cyclone_crypto\cipher\camellia.c src\cyclone_crypto\cipher
copy ..\..\..\..\cyclone_crypto\cipher\seed.c src\cyclone_crypto\cipher
copy ..\..\..\..\cyclone_crypto\cipher\aria.c src\cyclone_crypto\cipher
copy ..\..\..\..\cyclone_crypto\cipher_mode\cbc.c src\cyclone_crypto\cipher_mode
copy ..\..\..\..\cyclone_crypto\aead\ccm.c src\cyclone_crypto\aead
copy ..\..\..\..\cyclone_crypto\aead\gcm.c src\cyclone_crypto\aead
copy ..\..\..\..\cyclone_crypto\cipher\chacha.c src\cyclone_crypto\cipher
copy ..\..\..\..\cyclone_crypto\mac\poly1305.c src\cyclone_crypto\mac
copy ..\..\..\..\cyclone_crypto\aead\chacha20_poly1305.c src\cyclone_crypto\aead
copy ..\..\..\..\cyclone_crypto\pkc\dh.c src\cyclone_crypto\pkc
copy ..\..\..\..\cyclone_crypto\pkc\rsa.c src\cyclone_crypto\pkc
copy ..\..\..\..\cyclone_crypto\pkc\dsa.c src\cyclone_crypto\pkc
copy ..\..\..\..\cyclone_crypto\ecc\ec.c src\cyclone_crypto\ecc
copy ..\..\..\..\cyclone_crypto\ecc\ec_curves.c src\cyclone_crypto\ecc
copy ..\..\..\..\cyclone_crypto\ecc\ecdh.c src\cyclone_crypto\ecc
copy ..\..\..\..\cyclone_crypto\ecc\ecdsa.c src\cyclone_crypto\ecc
copy ..\..\..\..\cyclone_crypto\mpi\mpi.c src\cyclone_crypto\mpi
copy ..\..\..\..\cyclone_crypto\encoding\base64.c src\cyclone_crypto\encoding
copy ..\..\..\..\cyclone_crypto\encoding\asn1.c src\cyclone_crypto\encoding
copy ..\..\..\..\cyclone_crypto\encoding\oid.c src\cyclone_crypto\encoding
copy ..\..\..\..\cyclone_crypto\certificate\pem_import.c src\cyclone_crypto\certificate
copy ..\..\..\..\cyclone_crypto\certificate\x509_cert_parse.c src\cyclone_crypto\certificate
copy ..\..\..\..\cyclone_crypto\certificate\x509_cert_validate.c src\cyclone_crypto\certificate
copy ..\..\..\..\cyclone_crypto\certificate\x509_common.c src\cyclone_crypto\certificate
copy ..\..\..\..\cyclone_crypto\rng\yarrow.c src\cyclone_crypto\rng

REM CycloneCrypto Headers
copy ..\..\..\..\cyclone_crypto\core\*.h src\cyclone_crypto\core
copy..\..\..\..\cyclone_crypto\hash\*.h src\cyclone_crypto\hash
copy..\..\..\..\cyclone_crypto\mac\*.h src\cyclone_crypto\mac
copy..\..\..\..\cyclone_crypto\cipher\*.h src\cyclone_crypto\cipher
copy..\..\..\..\cyclone_crypto\cipher_mode\*.h src\cyclone_crypto\cipher_mode
copy..\..\..\..\cyclone_crypto\aead\*.h src\cyclone_crypto\aead
copy..\..\..\..\cyclone_crypto\pkc\*.h src\cyclone_crypto\pkc
copy..\..\..\..\cyclone_crypto\ecc\*.h src\cyclone_crypto\ecc
copy..\..\..\..\cyclone_crypto\mpi\*.h src\cyclone_crypto\mpi
copy..\..\..\..\cyclone_crypto\encoding\*.h src\cyclone_crypto\encoding
copy..\..\..\..\cyclone_crypto\certificate\*.h src\cyclone_crypto\certificate
copy..\..\..\..\cyclone_crypto\rng\*.h src\cyclone_crypto\rng

REM Compile resources
..\..\..\..\utils\ResourceCompiler\bin\rc.exe resources src\res.c
