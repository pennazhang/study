/*
** $Id: treeview.c,v 1.8 2007-08-30 01:20:10 xwyan Exp $
**
** treeview.c: Sample program for MiniGUI Programming Guide
**      Usage of TREEVIEW control.
**
** Copyright (C) 2004 ~ 2007 Fynman Software.
**
** License: GPL
*/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <minigui/common.h>
#include <minigui/minigui.h>
#include <minigui/gdi.h>
#include <minigui/window.h>
#include <minigui/control.h>
#include <minigui/mgext.h>

#define IDC_TREEVIEW  100

#define CHAPTER_NUM	5

#ifdef _LANG_ZHCN
static const char *chapter[] = 
{
    "第十六章 树型控件",
    "第十七章 列表型控件",
    "第十八章 月历控件",
    "第十九章 旋钮控件",
    "第二十章 酷工具栏控件",
};

static const char *section[] =
{
    "控件风格",
    "控件消息",
    "控件通知码"
};
#else
static const char *chapter[] = 
{
    "Static Control",
    "Button Control",
    "Edit Control",
    "Listbox Control",
    "Treeview Control",
};

static const char *section[] =
{
    "Styles of Control",
    "Messages of Control",
    "Sample Program"
};
#endif

static int BookProc(HWND hDlg, int message, WPARAM wParam, LPARAM lParam)
{

    switch (message) {
        case MSG_INITDIALOG:
        {
            TVITEMINFO tvItemInfo;
	    int item;
	    int i, j;

	    for (i = 0; i < CHAPTER_NUM; i++) {
		tvItemInfo.text = (char*)chapter[i];
	        item = SendMessage (GetDlgItem(hDlg, IDC_TREEVIEW), TVM_ADDITEM, 
			    0, (LPARAM)&tvItemInfo);
		for (j = 0; j < 3; j++) {
		    tvItemInfo.text = (char*)section[j];
	            SendMessage (GetDlgItem(hDlg, IDC_TREEVIEW), TVM_ADDITEM, 
			    item, (LPARAM)&tvItemInfo);
		}
	    }
        }
        break;

        case MSG_COMMAND:
        	switch (LOWORD(wParam)) {
        	case IDOK:
            	EndDialog (hDlg, wParam);
			}	
			break;
		
        case MSG_CLOSE:
	    EndDialog (hDlg, 0);
            return 0;
    }

    return DefaultDialogProc (hDlg, message, wParam, lParam);
}

static TVITEMINFO bookInfo =
{
#ifdef _LANG_ZHCN
    "MiniGUI编程指南"
#else
    "Contents"
#endif
};

static DLGTEMPLATE DlgBook =
{
    WS_BORDER | WS_CAPTION,
    WS_EX_NONE,
    0, 0, 300, 250,
#ifdef _LANG_ZHCN
    "目录",
#else
    "Book Contents",
#endif
    0, 0,
    2, NULL,
    0
};

static CTRLDATA CtrlBook[] =
{
    {
        "treeview",
        WS_BORDER | WS_CHILD | WS_VISIBLE | 
		WS_VSCROLL | WS_HSCROLL,
         10, 10, 280, 180,
        IDC_TREEVIEW,
        "treeview control",
        (LPARAM)&bookInfo
    },
    {
        "button",
        WS_VISIBLE | BS_DEFPUSHBUTTON | WS_TABSTOP | WS_GROUP,
        110, 200, 70, 25,
        IDOK, 
#ifdef _LANG_ZHCN
        "确认",
#else
        "OK",
#endif
        0
    },
};

int MiniGUIMain (int argc, const char* argv[])
{
#ifdef _MGRM_PROCESSES
    JoinLayer(NAME_DEF_LAYER , "treeview" , 0 , 0);
#endif
    
    if (!InitMiniGUIExt()) {
        return 2;
    }

    DlgBook.controls = CtrlBook;
    
    DialogBoxIndirectParam (&DlgBook, HWND_DESKTOP, BookProc, 0L);

    MiniGUIExtCleanUp ();

    return 0;
}

#ifndef _LITE_VERSION
#include <minigui/dti.c>
#endif

