# This file is used to develop custom GUI framework
# support for GTK+3.
FLAGS := -g -Wall -O0 -DUNIT_TEST \
         -D_GTK `pkg-config --cflags gtk+-3.0` -o test_gtk
SRCS := Window.cpp GdiCanvas.cpp ImageMatrix.cpp
LIBS := -ljpeg -lpng `pkg-config --libs gtk+-3.0`

ALL:
	g++ ${FLAGS} ${SRCS} ${LIBS}
clean:
	rm -f out