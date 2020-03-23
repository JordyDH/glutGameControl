#-*-makefile-*-

TARGETS = \
    sandbox

all All: $(TARGETS)

################################################################################
INCPATH = -I/usr/local/include
LIBPATH = -L. -L/lib/x86_64-linux-gnu
LIBS = -lglut -lGLU -lGL -lGLw -lXt -lXext -lX11 -ljpeg -lm 
DEBUGFLAGS = -O3
CFLAGS = -DD32 $(DEBUGFLAGS) $(INCPATH)
SRCS = \
	sandbox.c glutGameControl.c

OBJS = $(SRCS:.c=.o)

.SUFFIXES:	.c

.c.o:	
	cc -c $< $(CFLAGS)


glutGameControl:  clean $(OBJS)
	LDOPTS= \
	cc -o glutGameControl $(CFLAGS) $(OBJS) $(LIBPATH) $(LIBS) 

sandbox:  clean $(OBJS)
	LDOPTS= \
	cc -o sandbox $(CFLAGS) $(OBJS) $(LIBPATH) $(LIBS) 

clean:
	rm -f sandbox glutGameControl $(OBJS) core

print:
	listing sandbox.h $(SRCS)
