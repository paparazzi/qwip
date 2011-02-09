PPRZ_DIR = /home/paul/paparazzi

CFLAGS += -Wall -std=gnu99 -I$(PPRZ_DIR) -I$(PPRZ_DIR)/sw/include

#
# cgi for updating interface, for running on laptop
#
cgi-bin/update_qwip.local : cgi-bin/update_qwip.c
	gcc $(CFLAGS) -g `pkg-config glib-2.0 --cflags` $^ -o $@ `pkg-config glib-2.0 --libs`
	cp $@ cgi-bin/update_qwip

#
# cgi for updating interface, for running on overo
#
OVERO_CC = /opt/paparazzi/omap/overo-oe/tmp/sysroots/i686-linux/usr/armv7a/bin/arm-angstrom-linux-gnueabi-gcc
cgi-bin/update_qwip.overo : cgi-bin/update_qwip.c
	$(OVERO_CC) $(CFLAGS) $^ -o $@

#
# ivy gateway (running on laptop) 
#
IS_SRCS = server/ivy_gs_gateway.c \
          server/gs_shm.c     \
          $(PPRZ_DIR)/sw/airborne/math/pprz_geodetic_double.c

IS_CFLAGS = $(CFLAGS) `pkg-config glib-2.0 --cflags`
IS_LDFLAGS = `pkg-config glib-2.0 --libs` `pcre-config --libs` -lglibivy -lm

server/ivy_gs_gateway: $(IS_SRCS)
	gcc $(IS_CFLAGS) $^ -o $@ $(IS_LDFLAGS)


SERVER_SRCS =  server/gs_server.c           \
                     server/gs_shm.c        \
                     server/qwip_gs_link.c  \
                     server/qwip_settings.c \
                     $(PPRZ_DIR)/sw/airborne/fms/fms_spi_link.c \
                     $(PPRZ_DIR)/sw/airborne/fms/fms_periodic.c \
                     $(PPRZ_DIR)/sw/airborne/math/pprz_geodetic_double.c

SERVER_CFLAGS = -DFMS_PERIODIC_FREQ=512
#
# pc server
#
server/pc_gs_server: $(SERVER_SRCS)
	$(CC) $(CFLAGS) `pkg-config glib-2.0 --cflags`  -DDISABLE_SPI_LINK $(SERVER_CFLAGS) $^ -o $@ -lm -levent  `pkg-config glib-2.0 --libs` `pcre-config --libs`

#
# overo server
#

server/overo_gs_server: $(SERVER_SRCS)
	$(OVERO_CC) $(CFLAGS) $(SERVER_CFLAGS) $^ -o $@ -lm -levent


OVERO_USER = poine
OVERO_HOST = auto1
OVERO_WWW  = ~/www
install: cgi-bin/update_qwip.overo server/overo_gs_server
	scp -r index.html js css favicon.ico $(OVERO_USER)@$(OVERO_HOST):$(OVERO_WWW)
	ssh $(OVERO_USER)@$(OVERO_HOST) mkdir -p /home/poine/www/cgi-bin
	scp cgi-bin/update_qwip.overo $(OVERO_USER)@$(OVERO_HOST):$(OVERO_WWW)/cgi-bin/update_qwip
	scp server/overo_gs_server $(OVERO_USER)@$(OVERO_HOST):~



clean:
	rm -rf *~ cgi-bin/update_qwip.local cgi-bin/update_qwip.overo server/ivy_gs_gateway server/overo_gs_server
