/* 
** $Id: edit.c,v 1.26 2007-10-25 07:56:45 weiym Exp $
**
** Listing 22.1
**
** edit.c: Sample program for MiniGUI Programming Guide
**             Usage of EDIT control.
**
** Copyright (C) 2004 ~ 2006 Feynman Software.
**
** License: GPL
*/

#include <stdio.h>
#include <stdlib.h>

#include <minigui/common.h>
#include <minigui/minigui.h>
#include <minigui/gdi.h>
#include <minigui/window.h>
#include <minigui/control.h>

static DLGTEMPLATE DlgBoxInputChar =
{
    WS_BORDER | WS_CAPTION, 
    WS_EX_NONE,
    0, 0, 400, 220, 
#ifdef _LANG_ZHCN
    "Çë¼üÈë×ÖÄ¸",
#else
    "Please input letters",
#endif
    0, 0,
    4, NULL,
    0
};

#define IDC_CHAR        100
#define IDC_CHARS       110

static CTRLDATA CtrlInputChar [] =
{ 
    {
        CTRL_STATIC,
        WS_VISIBLE ,
        10, 10, 380, 18, 
        IDC_STATIC, 
#ifdef _LANG_ZHCN
        "ÇëÊäÈëÒ»¸ö×ÖÄ¸:",
#else
        "Please input a letter:",
#endif
        0
    },
    {
        CTRL_SLEDIT,
        WS_VISIBLE | WS_TABSTOP | WS_BORDER | ES_CENTER,
        10, 40, 80, 25,
        IDC_CHAR,
        NULL,
        0
    },
    {
        CTRL_MLEDIT,
        WS_VISIBLE | WS_BORDER | WS_VSCROLL | ES_BASELINE | ES_AUTOWRAP,
        10, 80, 380, 70, 
        IDC_CHARS, 
        NULL,
        0
    },
    {
        CTRL_BUTTON,
        WS_TABSTOP | WS_VISIBLE | BS_DEFPUSHBUTTON, 
        170, 160, 60, 25,
        IDOK, 
        "OK",
        0
    }
};

static void my_notif_proc (HWND hwnd, int id, int nc, DWORD add_data)
{
    unsigned char buff [256] = {0};
    if (id == IDC_CHAR && nc == EN_CHANGE) {

        GetWindowText (hwnd, buff, 4);
        SendMessage (hwnd, EM_SETCARETPOS, 0, 0);
        SendMessage (GetDlgItem (GetParent (hwnd), IDC_CHARS), MSG_CHAR, buff[0], 0L);
    }
    else if (id == IDC_CHARS && nc == EN_CHANGE) {
        GetWindowText (hwnd, buff, 255);
    }
}       

static int InputCharDialogBoxProc (HWND hDlg, int message, WPARAM wParam, LPARAM lParam)
{
    HWND hwnd;

    switch (message) {
    case MSG_INITDIALOG:
        hwnd = GetDlgItem (hDlg, IDC_CHAR);
        SetNotificationCallback (hwnd, my_notif_proc);
        SendMessage (hwnd, MSG_KEYDOWN, SCANCODE_INSERT, 0L);
        return 1;
        
    case MSG_CLOSE:
        EndDialog (hDlg, IDCANCEL);
        break;
        
    case MSG_COMMAND:
        switch (wParam) {
        case IDOK:
        case IDCANCEL:
            EndDialog (hDlg, wParam);
            break;
        }
        break;
    }
    
    return DefaultDialogProc (hDlg, message, wParam, lParam);
}

int MiniGUIMain (int argc, const char* argv[])
{

#ifdef _MGRM_PROCESSES
    JoinLayer(NAME_DEF_LAYER , "edit" , 0 , 0);
#endif

#ifdef _LITE_VERSION
    if (!InitVectorialFonts ()) {
        printf ("InitVectorialFonts: error.\n");
        return 1;
    }
#endif

    DlgBoxInputChar.controls = CtrlInputChar;
    DialogBoxIndirectParam (&DlgBoxInputChar, HWND_DESKTOP, InputCharDialogBoxProc, 0L);

#ifdef _LITE_VERSION
    TermVectorialFonts ();
#endif
    return 0;
}

#ifndef _LITE_VERSION
#include <minigui/dti.c>
#endif

