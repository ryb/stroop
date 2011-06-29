CROSS_COMPILE=/opt/local/devkitpro/devkitGP2X/bin/arm-linux-
SDL_BASE=/opt/local/gp2x/bin/arm-linux-
LDFLAGS=-static

CC=$(CROSS_COMPILE)gcc
CXX=$(CROSS_COMPILE)g++
STRIP=$(CROSS_COMPILE)strip

#CFLAGS=`$(SDL_BASE)sdl-config --cflags` -O2 -Wall -Werror
CFLAGS=-I"/opt/local/gp2x/include" -I"/opt/local/gp2x/include/SDL"
CXXFLAGS=`$(SDL_BASE)sdl-config --cflags` -O2 -Wall -Werror
#LIBS=`$(SDL_BASE)sdl-config --libs`
LIBS = -L"/opt/local/gp2x/lib" -lSDL -lSDL_gfx -lSDL_ttf -lfreetype -lSDL -lSDL_image -ljpeg -lpng12 -lz -lSDL_mixer -lvorbisidec -lmikmod -lmad -lSDL -lgcc -lm -lexpat -lpthread -ldl

TARGET=stroop.gpe
OBJS=main.c

ALL_TARGETS=$(TARGET)

all: $(ALL_TARGETS)


$(TARGET): $(OBJS)
	$(CC) $(LDFLAGS) -o $(TARGET) $(OBJS) $(LIBS) $(CFLAGS)
	$(STRIP) $(TARGET)

clean:
	rm *.o $(TARGET)