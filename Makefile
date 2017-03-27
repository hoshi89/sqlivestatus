all:
	mkdir -p build
	mkdir -p /usr/lib64/sqlivestatus
	mkdir -p /opt/monitor/op5/sqlivestatus
	gcc src/module.c \
	src/db.c \
	 -shared -o build/sqlivestatus.so -fPIC -I/usr/include/glib-2.0 -I/usr/lib64/glib-2.0/include
	cp build/sqlivestatus.so /usr/lib64/sqlivestatus/
	cp build/sqlivestatus.conf /opt/monitor/op5/sqlivestatus/
	cp build/sqlivestatus.cfg /opt/monitor/etc/mconf/
