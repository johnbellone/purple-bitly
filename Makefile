## --- Local Configuration (Edit, if needed)
PURPLE_CFLAGS = $(shell pkg-config --cflags purple)
PIDGIN_CFLAGS = $(shell pkg-config --cflags purple)
CURL_CFLAGS = $(shell pkg-config --cflags libcurl)
PURPLE_LIBS = $(shell pkg-config --libs purple)
PIDGIN_LIBS = $(shell pkg-config --libs purple)
CURL_LIBS = $(shell pkg-config --libs libcurl)


## --- Main Code (MODIFY, ONLY IF YOU KNOW WHAT YOU'RE DOING)
.PHONY: all clean distclean install remove uninstall
SOURCES = bitly.c

all: bitly.so

bitly.so: ${SOURCES}
	${CC} ${PURPLE_CFLAGS} ${PIDGIN_CFLAGS} ${CURL_CFLAGS} -Wall -I. -g -O2 -pipe ${SOURCES} \
	    ${PURPLE_LIBS} ${PIDGIN_LIBS} ${CURL_LIBS} -o $@ -shared -fPIC -DPIC

clean:
	rm -f bitly.so

distclean: clean

install: all
	mkdir -p $(DESTDIR)/usr/lib/purple-2
	install -m644 -s bitly.so $(DESTDIR)/usr/lib/purple-2

remove: uninstall
uninstall:
	rm -f $(DESTDIR)/usr/lib/purple-2/bitly.so
	-rmdir -p $(DESTDIR)/usr/lib/purple-2
