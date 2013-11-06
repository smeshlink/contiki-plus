CONTIKI_PROJECT=slip-radio
all: $(CONTIKI_PROJECT)
APPS = slip-cmd

ifeq ($(TARGET),)
  -include Makefile.target
endif

CONTIKI=../../..

# IPv6
UIP_CONF_IPV6 = 1
# RPL
UIP_CONF_RPL = 0
# Address type: set to 0 for using ds2401, 1 for EUI64_ADDRESS defined in params.c.
#UIP_CONF_EUI64 = 0
# Low power mode 
CONF_LOWPOWER = 1

#WITH_UIP6=1
#UIP_CONF_IPV6=1
#UIP_CONF_RPL=0

#linker optimizations
SMALL=1

CFLAGS += -DPROJECT_CONF_H=\"project-conf.h\" -DSLIP_RADIO=1
PROJECT_SOURCEFILES += slip-net.c no-framer.c
ifeq ($(TARGET),sky)
  PROJECT_SOURCEFILES += slip-radio-cc2420.c slip-radio-sky-sensors.c
else
	PROJECT_SOURCEFILES += slip-radio-rf230.c
endif

include $(CONTIKI)/Makefile.include
