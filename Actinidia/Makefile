FLAGS := -g -Wall -Wno-parentheses -O0 -std=c++17 \
		-D_GTK `pkg-config --cflags gtk+-3.0` -o actinidia
SRCS := ../Tools/Common/*.cpp Actinidia.cpp Interface.cpp
LIBS := -ljpeg -lz -lpng -llua5.3 `pkg-config --libs gtk+-3.0` -lstdc++fs

ALL:
	g++-8 ${FLAGS} ${SRCS} ${LIBS}
clean:
	rm -f actinidia