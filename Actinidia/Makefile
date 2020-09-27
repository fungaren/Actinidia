FLAGS := -g -Wall -Wno-parentheses -O0 -std=c++17 \
		-DUNIT_TEST -D_GTK `pkg-config --cflags gtk+-3.0` -o actinidia
SRCS := ../Tools/Common/*.cpp Actinidia.cpp Interface.cpp
LIBS := -ljpeg -lz -lpng `pkg-config --libs gtk+-3.0` -lstdc++fs

ALL:
	g++-8 ${FLAGS} ${SRCS} ${LIBS}
clean:
	rm -f actinidia