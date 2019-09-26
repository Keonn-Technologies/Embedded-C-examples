ifeq ($(PLATFORM),EMBEDDED)
ENDIANNESS = BIG
include ../arch/ARM/ixp42x/module.mk
STRIP ?= $(TOOLS_PATH)/strip
else
STRIP ?= strip
endif

HEADERS += headers/serial_reader_imp.h
HEADERS += headers/tm_config.h
HEADERS += headers/tm_reader.h
HEADERS += headers/tmr_filter.h
HEADERS += headers/tmr_gen2.h
HEADERS += headers/tmr_gpio.h
HEADERS += headers/tmr_ipx.h
HEADERS += headers/tmr_iso180006b.h
HEADERS += headers/tmr_params.h
HEADERS += headers/tmr_read_plan.h
HEADERS += headers/tmr_region.h
HEADERS += headers/tmr_serial_reader.h
HEADERS += headers/tmr_serial_transport.h
HEADERS += headers/tmr_status.h
HEADERS += headers/tmr_tag_auth.h
HEADERS += headers/tmr_tag_data.h
HEADERS += headers/tmr_tag_lock_action.h
HEADERS += headers/tmr_tag_protocol.h
HEADERS += headers/tmr_tagop.h
HEADERS += headers/tmr_types.h
HEADERS += headers/tmr_utils.h

DBG ?= -g
CWARN = -W -Wall
# Add -Wextra to chase down warnings that might appear on build server, but not dev machines
#CWARN += -Wextra
CFLAGS += -D TMR_ENABLE_SERIAL_READER_ONLY=1
CFLAGS += -I. -Iheaders $(DBG) $(CWARN)

# Position-independent code required for shared libraries
CFLAGS += -fPIC

ifneq ($(PLATFORM),EMBEDDED)
PROGS += demo
endif
PROGS += filter
PROGS += firmwareload
PROGS += locktag
PROGS += multireadasync
PROGS += read
PROGS += readasync
PROGS += readintoarray
PROGS += blockpermalock
PROGS += blockwrite
PROGS += embeddedreadtid
PROGS += licensekey
PROGS += multiprotocolread
PROGS += savedconfig
PROGS += writetag
PROGS += readasynctrack
PROGS += readasyncfilter
PROGS += readasyncfilter-ISO18k-6b
PROGS += serialtime
PROGS += tagdir
PROGS += fastid
PROGS += denatranIAVcustomtagoperations
PROGS += readerstats
PROGS += readerInfo
PROGS += antennalist
PROGS += securereaddata
PROGS += readstopTrigger
PROGS += rebootReader
PROGS += readasyncGPIOControl
PROGS += readcustomtransport
PROGS += savedreadplanconfig
PROGS += loadsaveconfiguration
PROGS += bap
PROGS += readallmembanks-GEN2
PROGS += gpiocommands
PROGS += SL900Agetcalibrationdata
PROGS += SL900Agetsensorvalue
PROGS += SL900Asetcalibrationdata
PROGS += SL900Asetsfeparameters
PROGS += authenticate
PROGS += readbuffer
PROGS += untraceable
PROGS += autonomousmode


all: $(PROGS)

STATIC_LIB = libmercuryapi.so.1
LIB = $(STATIC_LIB)

$(OBJS):

read.o: $(HEADERS) $(LIB)
read: read.o $(LIB)
	$(CC) $(CFLAGS) -o $@ $^ -lpthread

blockpermalock.o: $(HEADERS) $(LIB)
blockpermalock: blockpermalock.o $(LIB)
	$(CC) $(CFLAGS) -o $@ $^ -lpthread

blockwrite.o: $(HEADERS) $(LIB)
blockwrite: blockwrite.o $(LIB)
	$(CC) $(CFLAGS) -o $@ $^ -lpthread

embeddedreadtid.o: $(HEADERS) $(LIB)
embeddedreadtid: embeddedreadtid.o $(LIB)
	$(CC) $(CFLAGS) -o $@ $^ -lpthread

licensekey.o: $(HEADERS) $(LIB)
licensekey: licensekey.o $(LIB)
	$(CC) $(CFLAGS) -o $@ $^ -lpthread

multiprotocolread.o: $(HEADERS) $(LIB)
multiprotocolread: multiprotocolread.o $(LIB)
	$(CC) $(CFLAGS) -o $@ $^ -lpthread

savedconfig.o: $(HEADERS) $(LIB)
savedconfig: savedconfig.o $(LIB)
	$(CC) $(CFLAGS) -o $@ $^ -lpthread

writetag.o: $(HEADERS) $(LIB)
writetag: writetag.o $(LIB)
	$(CC) $(CFLAGS) -o $@ $^ -lpthread

readintoarray.o: $(HEADERS) $(LIB)
readintoarray: readintoarray.o $(LIB)
	$(CC) $(CFLAGS) -o $@ $^ -lpthread

readasync.o: $(HEADERS) $(LIB)
readasync: readasync.o $(LIB)
	$(CC) $(CFLAGS) -o $@ $^ -lpthread

multireadasync.o: $(HEADERS) $(LIB)
multireadasync: multireadasync.o $(LIB)
	$(CC) $(CFLAGS) -o $@ $^ -lpthread

demo.o: $(HEADERS) $(LIB)
demo: demo.o $(LIB)
	$(CC) $(CFLAGS) -o $@ $^ -lreadline -lpthread

filter.o: $(HEADERS) $(LIB)
filter: filter.o $(LIB)
	$(CC) $(CFLAGS) -o $@ $^ -lpthread

firmwareload.o: $(HEADERS) $(LIB)
firmwareload: firmwareload.o $(LIB)
	$(CC) $(CFLAGS) -o $@ $^ -lpthread

./samples/locktag.o: $(HEADERS) $(LIB)
locktag: locktag.o $(LIB)
	$(CC) $(CFLAGS) -o $@ $^ -lpthread

readasynctrack.o: $(HEADERS) $(LIB)
readasynctrack: readasynctrack.o $(LIB)
	$(CC) $(CFLAGS) -o $@ $^ -lpthread

readasyncfilter.o: $(HEADERS) $(LIB)
readasyncfilter: readasyncfilter.o $(LIB)
	$(CC) $(CFLAGS) -o $@ $^ -lpthread

readasyncfilter-ISO18k-6b.o: $(HEADERS) $(LIB)
readasyncfilter-ISO18k-6b: readasyncfilter-ISO18k-6b.o $(LIB)
	$(CC) $(CFLAGS) -o $@ $^ -lpthread

serialtime.o: $(HEADERS) $(LIB)
serialtime: serialtime.o $(LIB)
	$(CC) $(CFLAGS) -o $@ $^ -lpthread

fastid.o: $(HEADERS) $(LIB)
fastid: fastid.o $(LIB)
	$(CC) $(CFLAGS) -o $@ $^ -lpthread
tagdir.o: $(HEADERS) $(LIB)
tagdir: tagdir.o $(LIB)
	$(CC) $(CFLAGS) -o $@ $^ -lpthread

onreader-tagdir.o: $(HEADERS) $(LIB)
onreader-tagdir: onreader-tagdir.o $(LIB)
	$(CC) $(CFLAGS) -o $@ $^ -lpthread

denatranIAVcustomtagoperations.o: $(HEADERS) $(LIB)
denatranIAVcustomtagoperations: denatranIAVcustomtagoperations.o $(LIB)
	$(CC) $(CFLAGS) -o $@ $^ -lpthread

readerstats.o: $(HEADERS) $(LIB)
readerstats: readerstats.o $(LIB)
	$(CC) $(CFLAGS) -o $@ $^ -lpthread

readerInfo.o: $(HEADERS) $(LIB)
readerInfo: readerInfo.o $(LIB)
	$(CC) $(CFLAGS) -o $@ $^ -lpthread

antennalist.o: $(HEADERS) $(LIB)
antennalist: antennalist.o $(LIB)
	$(CC) $(CFLAGS) -o $@ $^ -lpthread

securereaddata.o: $(HEADERS) $(LIB)
securereaddata: securereaddata.o $(LIB)
	$(CC) $(CFLAGS) -o $@ $^ -lpthread

readstopTrigger.o: $(HEADERS) $(LIB)
readstopTrigger: readstopTrigger.o $(LIB)
	$(CC) $(CFLAGS) -o $@ $^ -lpthread

rebootReader.o: $(HEADERS) $(LIB)
rebootReader: rebootReader.o $(LIB)
	$(CC) $(CFLAGS) -o $@ $^ -lpthread

readasyncGPIOControl.o: $(HEADERS) $(LIB)
readasyncGPIOControl: readasyncGPIOControl.o $(LIB)
	$(CC) $(CFLAGS) -o $@ $^ -lpthread

readcustomtransport.o: $(HEADERS) $(LIB)
readcustomtransport: readcustomtransport.o $(LIB)
	$(CC) $(CFLAGS) -o $@ $^ -lpthread	

savedreadplanconfig.o: $(HEADERS) $(LIB)
savedreadplanconfig: savedreadplanconfig.o $(LIB)
	$(CC) $(CFLAGS) -o $@ $^ -lpthread	

loadsaveconfiguration.o: $(HEADERS) $(LIB)
loadsaveconfiguration: loadsaveconfiguration.o $(LIB)
	$(CC) $(CFLAGS) -o $@ $^	 -lpthread

bap.o: $(HEADERS) $(LIB)
bap: bap.o $(LIB)
	$(CC) $(CFLAGS) -o $@ $^ -lpthread	

readallmembanks-GEN2.o: $(HEADERS) $(LIB)
readallmembanks-GEN2: readallmembanks-GEN2.o $(LIB)
	$(CC) $(CFLAGS) -o $@ $^ -lpthread	

gpiocommands.o: $(HEADERS) $(LIB)
gpiocommands: gpiocommands.o $(LIB)
	$(CC) $(CFLAGS) -o $@ $^ -lpthread	

SL900Agetcalibrationdata.o: $(HEADERS) $(LIB)
SL900Agetcalibrationdata: SL900Agetcalibrationdata.o $(LIB)
	$(CC) $(CFLAGS) -o $@ $^ -lpthread	

SL900Agetsensorvalue.o: $(HEADERS) $(LIB)
SL900Agetsensorvalue: SL900Agetsensorvalue.o $(LIB)
	$(CC) $(CFLAGS) -o $@ $^	 -lpthread

SL900Asetcalibrationdata.o: $(HEADERS) $(LIB)
SL900Asetcalibrationdata: SL900Asetcalibrationdata.o $(LIB)
	$(CC) $(CFLAGS) -o $@ $^ -lpthread	

SL900Asetsfeparameters.o: $(HEADERS) $(LIB)
SL900Asetsfeparameters: SL900Asetsfeparameters.o $(LIB)
	$(CC) $(CFLAGS) -o $@ $^ -lpthread

authenticate.o: $(HEADERS) $(LIB)
authenticate: authenticate.o $(LIB)
	$(CC) $(CFLAGS) -o $@ $^ -lpthread

readbuffer.o: $(HEADERS) $(LIB)
readbuffer: readbuffer.o $(LIB)
	$(CC) $(CFLAGS) -o $@ $^ -lpthread

untraceable.o: $(HEADERS) $(LIB)
untraceable: untraceable.o $(LIB)
	$(CC) $(CFLAGS) -o $@ $^ -lpthread

autonomousmode.o: $(HEADERS) $(LIB)
autonomousmode: autonomousmode.o $(LIB)
	$(CC) $(CFLAGS) -o $@ $^ -lpthread		

.PHONY: clean
clean:
	rm -f $(PROGS) *.o

# What serial ports exist?
list-linux-ports:
	echo /dev/tty{O}*
