CONTIKI_PROJECT = mx-web
all: $(CONTIKI_PROJECT)

# IPv6
UIP_CONF_IPV6 = 1
# RPL
UIP_CONF_RPL = 0
# Address type: set to 0 for using ds2401, 1 for EUI64_ADDRESS defined in params.c.
UIP_CONF_EUI64 = 1

APPS = mx-webserver
WEBDIR = ipv6_smeshlink
#HTTPD-CFS = 1

# Use radio driver that communicates with the core MAC layer. Now the default.
#RF230BB=1
# Use static eeprom coffee file system
#COFFEE_FILES=1
# Use full eeprom coffee file system
#COFFEE_FILES=3
# Use static flash coffee file system
#COFFEE_FILES=3
# Use full flash coffee file system
#COFFEE_FILES=4
# Override default coffee file system starting address
#COFFEE_ADDRESS=0xnnnn

#CFLAGS += -DCOFFEE_FILES

CONTIKI = ../..
include $(CONTIKI)/Makefile.include