# This file is used to build Actinidia tools on Linux
FLAGS := -Wall -D_GTK -std=c++17 -o actools
SRCS := ToolsCmd.cpp Common/ResourcePack.cpp Common/ImageMatrix.cpp Common/Compress.cpp Common/PiCanvas.cpp
LIBS := -ljpeg -lpng -lz -lstdc++fs

ALL:
	g++ -g -O0 ${FLAGS} ${SRCS} ${LIBS}
release:
	g++ -O2 -Wl,-strip-all ${FLAGS} ${SRCS} -Wl,-Bstatic ${LIBS} -Wl,-Bdynamic
clean:
	rm -f out
