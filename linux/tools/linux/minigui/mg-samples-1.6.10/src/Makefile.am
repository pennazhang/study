COMMON_PROGS=helloworld mycontrol dialogbox simplekey \
                static button combobox edit listbox menubutton progressbar \
				trackbar newtoolbar propsheet \
                scrollbar painter capture bitblt stretchblt loadbmp drawicon \
                createicon caretdemo cursordemo input bmpbkgnd skindemo \
                scrollwnd scrollwnd2 scrollview animation iconview gridview gridview_ext \
                timeeditor listview spinbox monthcal coolbar treeview mywins

if USE_NEWGAL
NEWGAL_PROGS=
else
NEWGAL_PROGS=
endif

if MGRM_PROCESSES
LITE_PROGS=scrnsaver mginit
else
LITE_PROGS=
endif

noinst_PROGRAMS=$(COMMON_PROGS) $(NEWGAL_PROGS) $(LITE_PROGS)

helloworld_SOURCES=helloworld.c
mycontrol_SOURCES=mycontrol.c
dialogbox_SOURCES=dialogbox.c
input_SOURCES=input.c
edit_SOURCES=edit.c
listbox_SOURCES=listbox.c
bmpbkgnd_SOURCES=bmpbkgnd.c
bitblt_SOURCES=bitblt.c
stretchblt_SOURCES=stretchblt.c
loadbmp_SOURCES=loadbmp.c
caretdemo_SOURCES=caretdemo.c
cursordemo_SOURCES=cursordemo.c
createicon_SOURCES=createicon.c
drawicon_SOURCES=drawicon.c
simplekey_SOURCES=simplekey.c
capture_SOURCES=capture.c
painter_SOURCES=painter.c
scrollbar_SOURCES=scrollbar.c
static_SOURCES=static.c
button_SOURCES=button.c
combobox_SOURCES=combobox.c
menubutton_SOURCES=menubutton.c
progressbar_SOURCES=progressbar.c
trackbar_SOURCES=trackbar.c
newtoolbar_SOURCES=newtoolbar.c
propsheet_SOURCES=propsheet.c
listview_SOURCES=listview.c
spinbox_SOURCES=spinbox.c
monthcal_SOURCES=monthcal.c
coolbar_SOURCES=coolbar.c
timeeditor_SOURCES=timeeditor.c
treeview_SOURCES=treeview.c
scrollwnd_SOURCES=scrollwnd.c
scrollwnd2_SOURCES=scrollwnd2.c
scrollview_SOURCES=scrollview.c
iconview_SOURCES=iconview.c
gridview_SOURCES=gridview.c
gridview_ext_SOURCES=gridview_ext.c
mywins_SOURCES=mywins.c

mginit_SOURCES=mginit.c
scrnsaver_SOURCES=scrnsaver.c
skindemo_SOURCES=skindemo.c

listview_LDADD=-lmgext
iconview_LDADD=-lmgext
spinbox_LDADD=-lmgext
monthcal_LDADD=-lmgext
coolbar_LDADD=-lmgext
timeeditor_LDADD=-lmgext
treeview_LDADD=-lmgext
animation_LDADD=-lmgext
skindemo_LDADD=-lmgext
gridview_LDADD=-lmgext
gridview_ext_LDADD=-lmgext
mywins_LDADD=-lmgext

SUBDIRS=res
EXTRA_DIST=bkgnd.jpg new2.jpg new3.jpg myicon.ico question.ico close.png  pause.png  progress-bk.png  stop.png     timer.png      volume.png \
main.png   play.png   progress.png     sysmenu.png  volume-bk.png banner.gif 9.gif


