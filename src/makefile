# -*- mode: makefile; coding: sjis-unix; -*-
# Time-stamp: <2004-12-23 21:47:45 tfuruka1>
#
# $Id: makefile,v 1.9 2004/12/23 13:15:13 tfuruka1 Exp $
# $Log: makefile,v $
# Revision 1.9  2004/12/23 13:15:13  tfuruka1
# Version情報ダイアログを表示するようにしました。
#
# Revision 1.8  2004/08/21 11:12:49  tfuruka1
# VersionをVersionファイルから取得するようにしました。
#
# Revision 1.7  2004/08/21 01:01:54  tfuruka1
# CVSのタグからアーカイブファイルを作成する試み中
#
# Revision 1.6  2004/06/18 00:57:56  tfuruka1
# 改行コードの修正のみです。
#
# Revision 1.5  2004/06/17 15:32:21  tfuruka1
# fonts.c を追加した事による修正
#
# Revision 1.4  2004/01/11 11:11:32  tfuruka1
# Etagsのコマンドパスの変更(MeadowのVersionアップによる変更)
#
#

.SUFFIXES:	.obj .c
.c.obj:
	$(CC) $(CFLAGS) -c -o$@ $<

include version
SHELL=sh
CC = cl
ETAGS = /usr/local/Meadow/2.00/bin/etags

DLL_OBJS = dllmain.obj dbg.obj string.obj clipboard.obj
SV_OBJS = main.obj init.obj wndproc.obj ak2pr.res setup.obj queue.obj\
    pThread.obj printer.obj testprint.obj prtMail.obj prtText.obj string.obj\
	prev.obj fonts.obj version.obj
TARGZ = ../ak2pr_src.tar.gz

INSTDIR = /usr/local/meadow/site-bin

COMM_LIB = user32.lib shell32.lib gdi32.lib comdlg32.lib advapi32.lib\
	comctl32.lib wsock32.lib

CFLAGS = /Zi /W3 /MT
LDFLAGS = /debug

all : ver.h ak2pr.dll ak2prSV.exe ak2pr.exe ak2prW.exe tags

install : all
	cp ak2pr.dll ak2prSV.exe ak2pr.exe ak2prW.exe ../bin/.
	cp ak2pr.dll ak2prSV.exe ak2pr.exe ak2prW.exe $(INSTDIR)/.

release : install clean
	cd ..
	cd ..
	tar cvzf /tmp/ak2pr-$(MAKE_VERSION)-src.tar.gz\
        ak2pr/readme.txt ak2pr/bin ak2pr/src
	tar cvzf /tmp/ak2pr-$(MAKE_VERSION)-bin.tar.gz\
        ak2pr/readme.txt ak2pr/bin

ver.h : version
	echo #define MAKE_VERSION "$(MAKE_VERSION)" > $@
	echo #define TIMESTAMP "$(TIMESTAMP:"=)" >> $@
ak2prSV.exe : $(SV_OBJS)
	$(CC) $(CFLAGS) -o $@ $(SV_OBJS) /link $(LDFLAGS) ak2pr.lib $(COMM_LIB)
ak2pr.dll : $(DLL_OBJS)
	$(CC) $(CFLAGS) -o $@ $(DLL_OBJS) /link $(LDFLAGS) /dll\
    /def:dllmain.def $(COMM_LIB)
ak2pr.res : ak2pr.rc resource.h
	rc ak2pr.rc
ak2pr.exe : ak2pr.obj clientCommon.obj
	$(CC) $(CFLAGS) -o $@ ak2pr.obj clientCommon.obj\
	/link $(LDFLAGS) ak2pr.lib $(COMM_LIB)
ak2prW.exe : ak2prW.obj clientCommon.obj
	$(CC) $(CFLAGS) -o $@ ak2prW.obj clientCommon.obj\
	/link $(LDFLAGS) ak2pr.lib $(COMM_LIB)

$(SV_OBJS) $(DLL_OBJS) : makefile ak2pr.h ak2prs.h dllmain.def ver.h

ak2pr.obj : makefile ak2pr.dll ak2prSv.exe
clientCommon.obj : makefile ak2pr.dll ak2prSv.exe

tags :
	$(ETAGS) *.c *.h *.C *.H

tar: clean
	tar cvzf $(TARGZ) .
clean:
	rm -f ver.h
	rm -f *.obj
	rm -f *.aps
	rm -f *.exe
	rm -f *.dll
	rm -f *.lib
	rm -f *.exp
	rm -f *.res
	rm -f *.?~
	rm -f *.??~
	rm -f *.???~
	rm -f makefile~
	rm -f *.ilk
	rm -f *.log
	rm -f *.pdb
	rm -f *.ini
	rm -f *.RES
	rm -f tags
	rm -f $(TARGZ)
