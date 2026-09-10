all: fontconvert

CC     = gcc
CFLAGS = -Wall -I/opt/homebrew/include/freetype2 -I/usr/local/include/freetype2 -I/usr/include/freetype2 -I/usr/include
LIBS   = -L/opt/homebrew/lib -L/usr/local/lib -lfreetype

fontconvert: fontconvert.c
	$(CC) $(CFLAGS) $< $(LIBS) -o $@
	strip $@

clean:
	rm -f fontconvert
