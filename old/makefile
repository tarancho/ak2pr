#

LDLFAGS = /link /debug
LDFLAGS = $(LDFLAGS) gdi32.lib user32.lib comdlg32.lib shell32.lib advapi32.lib

CFLAGS = /Zi
#CFLAGS = /Ox

OBJS = main.obj string.obj
OBJSOL = OLmpr.obj mime.obj

all : fmpr.exe mpr.exe install
fmpr.exe : $(OBJS)
	cl $(CFLAGS) -o $@ $(OBJS) $(LDFLAGS)
mpr.exe : mpr.c mime.c
	gcc -o mpr.exe mpr.c -L/msdev/vc/lib -lshell32 -luser32
mpr.exe $(OBJS) : makefile

OLmpr.exe : $(OBJSOL)
	cl $(CFLAGS) -o $@ $(OBJSOL) $(LDFLAGS)
install :
	cp fmpr.exe mpr.exe /usr/local/mule2/bin/.
