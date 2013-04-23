CONTIKI_PROJECT = hello-world
all: $(CONTIKI_PROJECT)

# IPv6
UIP_CONF_IPV6 = 1
# RPL
UIP_CONF_RPL = 1
# Address type: set to 0 for using ds2401, 1 for EUI64_ADDRESS defined in params.c.
UIP_CONF_EUI64 = 1
# Low power mode 
CONF_LOWPOWER = 0

APPS += mx-shell

CONTIKI = ../..
include $(CONTIKI)/Makefile.include