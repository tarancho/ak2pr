# -*- mode: makefile; coding: sjis-unix; -*-
# Time-stamp: <2000-11-05 12:33:33 tfuruka1>
#
# $Id: makefile,v 1.1 2001/02/05 17:56:23 tfuruka1 Exp $
# $Log: makefile,v $
# Revision 1.1  2001/02/05 17:56:23  tfuruka1
# Initial revision
#
# Revision 1.2  2000/11/05 03:50:02  tfuruka1
# Cygnusのmakeでコンパイルできるように修正。
#
# Revision 1.1  2000/11/04 13:43:01  tfuruka1
# Initial revision
#
# Revision 1.3  1999/12/16 08:40:03  tfuruka1
# ● ディレクトリ構造の階層を一段浅くしたので，関連する部分を修正した。
# ● ソースのアーカイブ名を変更した。
#
# Revision 1.2  1999/12/16 05:21:04  tfuruka1
# TAG(etags)ファイルを作成する処理を追加した。
#
# Revision 1.1  1999/12/16 04:56:15  tfuruka1
# Initial revision
#
#
.SUFFIXES:	.obj .c
.c.obj:
	$(CC) $(CFLAGS) -c -o$@ $<

CC = cl
ETAGS = /usr/local/Meadow/1.10/bin/etags

DLL_OBJS = dllmain.obj dbg.obj
SV_OBJS = main.obj init.obj wndproc.obj ak2pr.res setup.obj queue.obj\
    pThread.obj printer.obj testprint.obj prtMail.obj prtText.obj string.obj
TARGZ = ../ak2pr_src.tar.gz
BINDIR = ../bin
LIBDIR = ../lib

INSTDIR = /usr/local/meadow/site-bin

COMM_LIB = user32.lib shell32.lib gdi32.lib comdlg32.lib advapi32.lib\
	comctl32.lib wsock32.lib

CFLAGS = /Zi /W3 /MT
LDFLAGS = /debug

all : ak2pr.dll ak2prSV.exe ak2pr.exe ak2prW.exe tags

install : all
	cp ak2pr.dll ak2prSV.exe ak2pr.exe ak2prW.exe $(INSTDIR)/.

ak2prSV.exe : $(SV_OBJS)
	$(CC) $(CFLAGS) -o $@ $(SV_OBJS) /link $(LDFLAGS) ../lib/ak2pr.lib\
    $(COMM_LIB)
	cp $@ $(BINDIR)/$@
ak2pr.dll : $(DLL_OBJS)
	$(CC) $(CFLAGS) -o $@ $(DLL_OBJS) /link $(LDFLAGS) /dll\
    /def:dllmain.def $(COMM_LIB)
	cp $@ $(BINDIR)/$@
	cp ak2pr.lib $(LIBDIR)/ak2pr.lib 
ak2pr.res : ak2pr.rc
	rc ak2pr.rc
ak2pr.exe : ak2pr.obj clientCommon.obj
	$(CC) $(CFLAGS) -o $@ ak2pr.obj clientCommon.obj\
	/link $(LDFLAGS) ../lib/ak2pr.lib $(COMM_LIB)
ak2prW.exe : ak2prW.obj clientCommon.obj
	$(CC) $(CFLAGS) -o $@ ak2prW.obj clientCommon.obj\
	/link $(LDFLAGS) ../lib/ak2pr.lib $(COMM_LIB)
$(SV_OBJS) $(DLL_OBJS) : makefile ak2pr.h ak2prs.h dllmain.def
ak2pr.obj : makefile ak2pr.dll ak2prSv.exe

tags :
	$(ETAGS) *.c *.h *.C *.H

tar: clean
	tar cvzf $(TARGZ) .
clean:
	./clean.sh
	rm -f tags
	rm -f $(TARGZ)
