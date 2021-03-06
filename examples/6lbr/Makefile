CONTIKI_PROJECT = cetic-6lbr

# Default mode selection

CETIC_6LBR_SMARTBRIDGE=0
CETIC_6LBR_TRANSPARENTBRIDGE=0
CETIC_6LBR_ROUTER=0
CETIC_6LBR_6LR=0
CETIC_6LBR_ONE_ITF=0
CETIC_6LBR_LEARN_RPL_MAC=1

$(CONTIKI_PROJECT):

CONTIKI_WITH_IPV6 = 1

CONTIKI = ../..

APPDIRS = apps

PROJECTDIRS += 6lbr apps/coapserver ../6lbr-demo/apps/udp-client ../6lbr-demo/apps/coap

CFLAGS += -DPROJECT_CONF_H=\"project-conf.h\"

ifeq ($(TARGET),)
TARGET=native
endif

-include platform/$(TARGET)/Makefile.$(TARGET)
ifneq ("$(wildcard platform/$(TARGET)/6lbr-conf-$(TARGET).h)","")
CFLAGS+=-DCETIC_6LBR_PLATFORM_CONF=\"6lbr-conf-$(TARGET).h\"
endif

CFLAGS +=-DCETIC_6LBR_SMARTBRIDGE=$(CETIC_6LBR_SMARTBRIDGE)
CFLAGS +=-DCETIC_6LBR_TRANSPARENTBRIDGE=$(CETIC_6LBR_TRANSPARENTBRIDGE)
CFLAGS +=-DCETIC_6LBR_ROUTER=$(CETIC_6LBR_ROUTER)
CFLAGS +=-DCETIC_6LBR_6LR=$(CETIC_6LBR_6LR)
CFLAGS +=-DCETIC_6LBR_ONE_ITF=$(CETIC_6LBR_ONE_ITF)
CFLAGS +=-DCETIC_6LBR_LEARN_RPL_MAC=$(CETIC_6LBR_LEARN_RPL_MAC)

CONTIKI_WITH_RPL ?= 1

ifeq ($(CETIC_6LBR_TRANSPARENTBRIDGE),1)
CONTIKI_WITH_RPL = 0
endif

ifeq ($(CETIC_6LBR_NDP_ROUTER),1)
CONTIKI_WITH_RPL = 0
endif

ifneq ($(WITH_WEBSERVER),0)
APPS+=6lbr-webserver
endif

ifneq ($(WITH_WEBSERVER_LITE),0)
APPS+=6lbr-webserver-lite
endif

ifneq ($(WITH_UDPSERVER),0)
APPS+=udp-server
endif

ifneq ($(WITH_UDPCLIENT),0)
CFLAGS += -DUDPCLIENT=1 -DUDP_CLIENT_AUTOSTART
PROJECT_SOURCEFILES += udp-client.c 
endif

ifneq ($(WITH_NODE_INFO),0)
APPS+=node-info
endif

ifneq ($(WITH_NVM_PROXY),0)
APPS+=nvm-proxy
endif

ifneq ($(WITH_COAPSERVER),0)
CFLAGS += -DWITH_COAPSERVER=1
CFLAGS += -DREST=coap_rest_implementation
APPS += er-coap
APPS += rest-engine
PROJECT_SOURCEFILES += coap-server.c block-transfer.c \
	device-resource.c config-stack-resource.c \
	coap-common.c core-interface.c binding-table-resource.c linked-batch-resource.c coap-push.c
endif

PROJECT_SOURCEFILES += log-6lbr.c rio.c packet-filter.c sicslow-ethernet.c nvm-config.c llsec-wrapper.c
MODULES += core/net/llsec/noncoresec

tools:
	cd tools && $(MAKE)

6lbr-demo:
	pushd test; ../package/usr/bin/6lbr conf-templates/cooja-1-router.conf; popd

all_native:
	@echo "Target is deprecated, use 'make TARGET=native all' instead"

all_econotag:
	@echo "Target is deprecated, use 'make TARGET=econotag all' instead"

all_red-io:
	@echo "Target is deprecated, use 'make TARGET=native all' instead"

all_cc2538dk:
	@echo "Target is deprecated, use 'make TARGET=cc2538dk all' instead"

PKG_VERSION=0
VERSION=$(shell echo $(word 3,$(shell awk '/CETIC_6LBR_VERSION/' project-conf.h)))

DEB=6lbr-$(VERSION)-$(PKG_VERSION)-deb
IPK=6lbr-$(VERSION)-$(PKG_VERSION)-ipk

install:
	install -d /etc/6lbr
	cp -r package/etc/6lbr /etc
	install -d /etc/init.d
	install package/etc/init.d/* /etc/init.d
	install -d /usr/bin
	install package/usr/bin/* /usr/bin
	install -d /usr/lib/6lbr
	cp -r package/usr/lib/6lbr /usr/lib
	install -d /usr/lib/6lbr/bin
	install bin/* /usr/lib/6lbr/bin
ifneq ($(PLUGINS_FILES),)
	install $(PLUGINS_FILES) /usr/lib/6lbr/plugins
endif
	install tools/nvm_tool /usr/lib/6lbr/bin

ipk:
	rm -rf $(IPK)
	rm -f $(IPK).ipk
	cp -R package $(IPK)
	mkdir -p $(IPK)/usr/lib/6lbr/bin
	cp bin/* $(IPK)/usr/lib/6lbr/bin
ifneq ($(PLUGINS_FILES),)
	cp $(PLUGINS_FILES) $(IPK)/usr/lib/6lbr/plugins
endif
	cp tools/nvm_tool $(IPK)/usr/lib/6lbr/bin
	cd $(IPK) && tar zcvf ../$(IPK)/data.tar.gz --exclude='*~' *
	cd angstrom && tar zcvfh ../$(IPK)/control.tar.gz --exclude='*~' *
	echo 2.0 > $(IPK)/debian-binary
	cd $(IPK) && ar rc ../cetic-6lbr_$(VERSION)-$(PKG_VERSION)_armv7a.ipk debian-binary data.tar.gz control.tar.gz

tools:
	cd tools && $(MAKE)

6lbr-demo:
	pushd test; ../package/usr/bin/6lbr conf-templates/cooja-1-router.conf; popd

include $(CONTIKI)/Makefile.include

.PHONY: all tools 6lbr-demo
