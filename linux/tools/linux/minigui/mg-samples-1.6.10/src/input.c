/* 
** $Id: input.c,v 1.20 2007-08-30 01:20:10 xwyan Exp $
**
** Listing 5.2
**
** input.c: Sample program for MiniGUI Programming Guide
** 	    Use dialog box to interact with user.
**
** Copyright (C) 2004 ~ 2007 Feynman Software.
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

#define IDC_SIZE_MM     100
#define IDC_SIZE_INCH   110

static CTRLDATA CtrlInputLen [] =
{ 
    {
        CTRL_STATIC,
        WS_VISIBLE | SS_SIMPLE,
        10, 10, 380, 18, 
        IDC_STATIC, 
#ifdef _LANG_ZHCN
        "请输入长度（单位：毫米）",
#else
        "Please input the length(Unit: mm)",
#endif
        0
    },

    {
        CTRL_EDIT,
        WS_VISIBLE | WS_TABSTOP | WS_BORDER,
        10, 40, 380, 24,
        IDC_SIZE_MM,
        NULL,
        0
    },
    {
        CTRL_STATIC,
        WS_VISIBLE | SS_SIMPLE,
        10, 70, 380, 18, 
        IDC_SIZE_INCH, 
#ifdef _LANG_ZHCN
        "相当于 0.00 英寸",
#else
        "Equivalent to  0.00 inches",
#endif
        0
    },
    {
        CTRL_BUTTON,
        WS_TABSTOP | WS_VISIBLE | BS_DEFPUSHBUTTON, 
        170, 100, 60, 25,
        IDOK, 
        "OK",
        0
    }
};

static DLGTEMPLATE DlgBoxInputLen =
{
    WS_BORDER | WS_CAPTION, 
    WS_EX_NONE,
    0, 0, 400, 160, 
#ifdef _LANG_ZHCN
    "请输入长度",
#else
	"Please input the length",
#endif
    0, 0,
    sizeof(CtrlInputLen)/sizeof(CTRLDATA), NULL,
    0
};


static void my_notif_proc (HWND hwnd, int id, int nc, DWORD add_data)
{
    if (id == IDC_SIZE_MM && nc == EN_CHANGE) {
        char buff [60];
        double len;

        GetWindowText (hwnd, buff, 32);
        len = atof (buff);
        len = len / 25.4;
#ifdef _LANG_ZHCN
        sprintf (buff, "相当于 %.5f 英寸", len);
#else
        sprintf (buff,"Equivalent to %.5f inches ", len);
#endif
        SetDlgItemText (GetParent (hwnd), IDC_SIZE_INCH, buff);
    }
}       

static int InputLenDialogBoxProc (HWND hDlg, int message, WPARAM wParam, LPARAM lParam)
{
    switch (message) {
    case MSG_INITDIALOG:
        SetWindowAdditionalData (hDlg, lParam);
        SendDlgItemMessage (hDlg, IDC_SIZE_MM, EM_LIMITTEXT, 15, 0);
        SetNotificationCallback (GetDlgItem (hDlg, IDC_SIZE_MM), my_notif_proc);
        return 1;
        
    case MSG_CLOSE:
        EndDialog (hDlg, IDCANCEL);
        break;
        
    case MSG_COMMAND:
        switch (wParam) {
        case IDOK:
        {
            char buff [40];
            double* length = (double*) GetWindowAdditionalData (hDlg);
            GetWindowText (GetDlgItem (hDlg, IDC_SIZE_MM), buff, 32);
            *length = atof (buff);
        }
        case IDCANCEL:
            EndDialog (hDlg, wParam);
            break;
        }
        break;
    }
    
    return DefaultDialogProc (hDlg, message, wParam, lParam);
}

static int InputLenDialogBox (HWND hWnd, double* length)
{
    DlgBoxInputLen.controls = CtrlInputLen;
    
    return DialogBoxIndirectParam (&DlgBoxInputLen, hWnd, InputLenDialogBoxProc, (LPARAM)length);
}

int MiniGUIMain (int argc, const char* argv[])
{
    double length;

#ifdef _MGRM_PROCESSES
    JoinLayer(NAME_DEF_LAYER , "input" , 0 , 0);
#endif
    
    if (InputLenDialogBox (HWND_DESKTOP, &length) == IDOK)
        printf ("The length is %.5f mm.\n", length);

    return 0;
}

#ifndef _LITE_VERSION
#include <minigui/dti.c>
#endif

