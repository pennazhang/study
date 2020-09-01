/*
** $Id: monthcal.c,v 1.12 2007-10-25 07:56:45 weiym Exp $
**
** monthcal.c: Sample program for MiniGUI Programming Guide
**      Usage of MONTHCALENDAR control.
**
** Copyright (C) 2004 ~ 2007 Feynman Software.
**
** License: GPL
*/


#include <stdlib.h>
#include <time.h>

#include <minigui/common.h>
#include <minigui/minigui.h>
#include <minigui/gdi.h>
#include <minigui/window.h>
#include <minigui/control.h>
#include <minigui/mywindows.h>
#include <minigui/mgext.h>


#define IDC_MC                100
#define IDC_OK                200

CTRLDATA CtrlTime[]=
{
        {
                "monthcalendar",
#ifdef _LANG_ZHCN
                WS_CHILD | WS_VISIBLE | MCS_NOTIFY | MCS_CHN,
#else
                WS_CHILD | WS_VISIBLE | MCS_NOTIFY | MCS_ENG_L,
#endif
                10, 10, 220, 180,
                IDC_MC,
                "",
                0
        },
        {
                "button",
                WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
                10, 195, 50, 22,
                IDC_OK,
#ifdef _LANG_ZHCN
                "确定",
#else
                "OK",
#endif
                0
        }
};

DLGTEMPLATE DlgTime = 
{
        WS_VISIBLE | WS_CAPTION | WS_BORDER,
        WS_EX_NONE,
        0, 0, 240, 250,
#ifdef _LANG_ZHCN
        "日期",
#else
        "Date",
#endif
        0, 0,
        2, CtrlTime,
        0
};

/*******************************************************************************/

static int TimeWinProc(HWND hDlg, int message, WPARAM wParam, LPARAM lParam)
{
    switch (message) {
        case MSG_INITDIALOG:
        break;

        case MSG_COMMAND:
        {
            int id = LOWORD(wParam);
	    if (id == IDC_OK) {
		    char info[100];
		    SYSTEMTIME date;
		    SendMessage (GetDlgItem(hDlg, IDC_MC), MCM_GETCURDATE, 0, (LPARAM)&date);
#ifdef _LANG_ZHCN
		    sprintf (info, "您选定的日期是：%d年%d月%d日。", date.year, date.month, date.day);
		    MessageBox (hDlg, info, "日期", MB_OK);
#else
		    sprintf (info, "The date you selected is: %d.%d.%d", date.year, date.month, date.day);
		    MessageBox (hDlg, info, "Date", MB_OK);
#endif
		    EndDialog (hDlg, 0);
	    }
        }
        break;

    case MSG_CLOSE:
        {
	    EndDialog (hDlg, 0);
        }
        return 0;
    }

    return DefaultDialogProc (hDlg, message, wParam, lParam);
}

/*******************************************************************************/

int MiniGUIMain (int argc, const char* argv[])
{
#ifdef _MGRM_PROCESSES
    JoinLayer(NAME_DEF_LAYER , "monthcal" , 0 , 0);
#endif
    
    if (!InitMiniGUIExt()) {
        return 2;
    }

    DlgTime.controls = CtrlTime;
    
    DialogBoxIndirectParam (&DlgTime, HWND_DESKTOP, TimeWinProc, 0L);

    MiniGUIExtCleanUp ();

    return 0;
}

#ifndef _LITE_VERSION
#include <minigui/dti.c>
#endif

