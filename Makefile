CFLAGS ?= -O2 -g -Wall
LDFLAGS ?=

GST_CFLAGS = $(shell pkg-config --cflags gstreamer-1.0)
GST_LDFLAGS = $(shell pkg-config --libs gstreamer-1.0)

GIO_CFLAGS = $(shell pkg-config --cflags gio-2.0)
GIO_LDFLAGS = $(shell pkg-config --libs gio-2.0)

all: app dynamic-filter dynamic-tee-vsink http-launch

clean:
	rm -f app dynamic-filter dynamic-tee-vsink http-launch

app: app.c
	$(CC) -o $@ $^ $(CFLAGS) $(GST_CFLAGS) $(LDFLAGS) $(GST_LDFLAGS)

dynamic-filter: dynamic-filter.c
	$(CC) -o $@ $^ $(CFLAGS) $(GST_CFLAGS) $(LDFLAGS) $(GST_LDFLAGS)

dynamic-tee-vsink: dynamic-tee-vsink.c
	$(CC) -o $@ $^ $(CFLAGS) $(GST_CFLAGS) $(LDFLAGS) $(GST_LDFLAGS)

http-launch: http-launch.c
	$(CC) -o $@ $^ $(CFLAGS) $(GST_CFLAGS) $(GIO_CFLAGS) $(LDFLAGS) $(GST_LDFLAGS) $(GIO_LDFLAGS)

