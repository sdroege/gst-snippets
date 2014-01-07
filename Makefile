CFLAGS = -O2 -Wall $(shell pkg-config --cflags gstreamer-1.0)
LDFLAGS = $(shell pkg-config --libs gstreamer-1.0)

all: app dynamic-filter

clean:
	rm -f app dynamic-filter

app: app.c
	$(CC) -o $@ $^ $(CFLAGS) $(LDFLAGS)

dynamic-filter: dynamic-filter.c
	$(CC) -o $@ $^ $(CFLAGS) $(LDFLAGS)

