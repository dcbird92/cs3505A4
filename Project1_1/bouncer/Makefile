# use pkg-config for getting CFLAGS and LDLIBS
FFMPEG_LIBS=    libavdevice                        \
                libavformat                        \
                libavfilter                        \
                libavcodec                         \
                libswresample                      \
                libswscale                         \
                libavutil                          \

CFLAGS += -Wall -g
CFLAGS := $(shell pkg-config --cflags $(FFMPEG_LIBS)) $(CFLAGS)
LDLIBS := $(shell pkg-config --libs $(FFMPEG_LIBS)) $(LDLIBS)

EXAMPLES=	bouncer                          \

OBJS=$(addsuffix .o,$(EXAMPLES))

all: $(OBJS) $(EXAMPLES)

clean:
	rm -f *.o bouncer *.spff *.mp4 *.mpg

movie:
	ffmpeg -framerate 30 -i frame%03d.spff ballBounce.mp4
