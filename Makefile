## --- Local Configuration (Edit, if needed)
LIBPURPLE_CFLAGS = -I/usr/include/libpurple -I/usr/local/include/libpurple
GLIB_CFLAGS = -I/usr/include/glib-2.0 -I/usr/lib/glib-2.0/include -I/usr/lib/gtk-2.0/include/ -I/usr/include -I/usr/local/include/glib-2.0 -I/usr/local/lib/glib-2.0/include -I/usr/local/include
WIN32_DEV_DIR = /root/pidgin/win32-dev
WIN32_PIDGIN_DIR = /root/pidgin/pidgin-2.3.0_win32
WIN32_CFLAGS = -I${WIN32_DEV_DIR}/gtk_2_0/include/glib-2.0 -I${WIN32_PIDGIN_DIR}/libpurple/win32 -I${WIN32_PIDGIN_DIR}/pidgin/win32 -I${WIN32_DEV_DIR}/gtk_2_0/include -I${WIN32_DEV_DIR}/gtk_2_0/include/glib-2.0 -I${WIN32_DEV_DIR}/gtk_2_0/lib/glib-2.0/include -I${WIN32_DEV_DIR}/gtk_2_0/lib/gtk-2.0/include -Wno-format
WIN32_LIBS = -L${WIN32_DEV_DIR}/gtk_2_0/lib -L${WIN32_PIDGIN_DIR}/libpurple -L${WIN32_PIDGIN_DIR}/pidgin -lglib-2.0 -lgobject-2.0 -lintl -lpidgin -lpurple -lws2_32 -L. -lgtk-win32-2.0
MACPORT_CFLAGS = -I/opt/local/include/libpurple -I/opt/local/include/glib-2.0 -I/opt/local/lib/glib-2.0/include -I/opt/local/include -arch i386 -arch ppc -dynamiclib -L/opt/local/lib -lpidgin -lpurple -lglib-2.0 -lgobject-2.0 -lintl -lz -isysroot /Developer/SDKs/MacOSX10.4u.sdk -mmacosx-version-min=10.4

#Standard stuff here
.PHONY: all clean distclean install remove uninstall
SOURCES = \
	bitly.c

all: bitly.dll bitly.so

bitly.so: ${SOURCES}
	${CC} ${LIBPURPLE_CFLAGS} -Wall ${GLIB_CFLAGS} -I. -g -O2 -pipe ${SOURCES} -o $@ -shared -fPIC -DPIC

bitly.dll: ${SOURCES}
	${CC} ${LIBPURPLE_CFLAGS} -Wall ${WIN32_CFLAGS} -I. -g -O0 -pipe ${SOURCES} -o $@ -shared -mno-cygwin ${WIN32_LIBS}
	upx $@

clean:
	rm -f bitly.dll bitly.so

distclean: clean

install:
	mkdir -p $(DESTDIR)/usr/lib/purple-2
	install -m644 -s bitly.so $(DESTDIR)/usr/lib/purple-2

remove: uninstall
uninstall:
	rm -f $(DESTDIR)/usr/lib/purple-2/bitly.so
	-rmdir -p $(DESTDIR)/usr/lib/purple-2
