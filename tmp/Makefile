CFLAGS = -Wall -g `pkg-config glib-2.0 --cflags`
LDFLAGS = `pkg-config glib-2.0 --libs` `pcre-config --libs`

test_xml: main.c gmarkup-dom.c
	gcc $(CFLAGS) $^ -o $@ $(LDFLAGS)
