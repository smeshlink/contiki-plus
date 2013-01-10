CONTIKI_PROJECT = co2-sensor
all: $(CONTIKI_PROJECT)

PROJECT_SOURCEFILES += co2.c
# IPv6
UIP_CONF_IPV6 = 1
# RPL
UIP_CONF_RPL = 0
# Address type: set to 0 for using ds2401, 1 for EUI64_ADDRESS defined in params.c.
#UIP_CONF_EUI64 = 0

# variable for this Makefile
WITH_COAP = 7

# REST framework, requires WITH_COAP
ifeq ($(WITH_COAP), 7)
${info INFO: compiling with CoAP-07}
CFLAGS += -DWITH_COAP=7
CFLAGS += -DREST=coap_rest_implementation
CFLAGS += -DUIP_CONF_TCP=0
APPS += er-coap-07
else ifeq ($(WITH_COAP), 3)
${info INFO: compiling with CoAP-03}
CFLAGS += -DWITH_COAP=3
CFLAGS += -DREST=coap_rest_implementation
CFLAGS += -DUIP_CONF_TCP=0
APPS += er-coap-03
else
${info INFO: compiling with HTTP}
CFLAGS += -DWITH_HTTP
CFLAGS += -DREST=http_rest_implementation
CFLAGS += -DUIP_CONF_TCP=1
APPS += er-http-engine
endif

APPS += erbium
APPS += rs232-sensor

CONTIKI = ../..
include $(CONTIKI)/Makefile.include