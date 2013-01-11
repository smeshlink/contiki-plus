CONTIKI_PROJECT = camera-sample
all: $(CONTIKI_PROJECT)

PROJECT_SOURCEFILES += camera-sample.c camera-rest-server.c

# IPv6
UIP_CONF_IPV6 = 1
# RPL
UIP_CONF_RPL = 0
# Address type: set to 0 for using ds2401, 1 for EUI64_ADDRESS defined in params.c.
#UIP_CONF_EUI64 = 0

# Toggle this option to use HTTP/CoAP
#WITH_COAP = 1

ifdef WITH_COAP 
CFLAGS += -DWITH_COAP
APPS = rest-coap
else
CFLAGS += -DWITH_HTTP
APPS = rest-http
endif

APPS += camera-interface

CONTIKI = ../../..
include $(CONTIKI)/Makefile.include
