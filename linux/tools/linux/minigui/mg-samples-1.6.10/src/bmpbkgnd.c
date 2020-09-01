/* 
** $Id: bmpbkgnd.c,v 1.18 2007-10-25 07:56:45 weiym Exp $
**
** Listing 5.2
**
** bmpbkgnd.c: Sample program for MiniGUI Programming Guide
**             Create a dialog box filling background with bitmap.
**
** Copyright (C) 2003 ~ 2007 Feynman Software.
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

#ifdef _LANG_ZHCN
static DLGTEMPLATE DlgBoxInputLen =
{
    WS_BORDER | WS_CAPTION, 
    WS_EX_NONE,
    0, 0, 600, 420, 
    "请输入长度",
    0, 0,
    4, NULL,
    0
};

#define IDC_SIZE_MM     100
#define IDC_SIZE_INCH   110

static CTRLDATA CtrlInputLen [] =
{ 
    {
        CTRL_STATIC,
        WS_VISIBLE | SS_SIMPLE,
        400, 10, 200, 18, 
        IDC_STATIC, 
        "请输入长度（单位：毫米）",
        0,
        WS_EX_TRANSPARENT
    },
    {
        CTRL_EDIT,
        WS_VISIBLE | WS_TABSTOP | WS_BORDER,
        400, 40, 200, 24,
        IDC_SIZE_MM,
        NULL,
        0
    },
    {
        CTRL_STATIC,
        WS_VISIBLE | SS_SIMPLE,
        400, 70, 200, 18, 
        IDC_SIZE_INCH, 
        "相当于 0.00 英寸",
        0,
        WS_EX_TRANSPARENT
    },
    {
        CTRL_BUTTON,
        WS_TABSTOP | WS_VISIBLE | BS_DEFPUSHBUTTON, 
        430, 100, 60, 25,
        IDOK, 
        "确定",
        0
    }
};
#else
static DLGTEMPLATE DlgBoxInputLen =
{
    WS_BORDER | WS_CAPTION, 
    WS_EX_NONE,
    0, 0, 620, 420, 
    "Please input the length",
    0, 0,
    4, NULL,
    0
};

#define IDC_SIZE_MM     100
#define IDC_SIZE_INCH   110

static CTRLDATA CtrlInputLen [] =
{ 
    {
        CTRL_STATIC,
        WS_VISIBLE | SS_SIMPLE,
        400, 10, 200, 18, 
        IDC_STATIC, 
        "Please input the length (mm)",
        0,
        WS_EX_TRANSPARENT
    },
    {
        CTRL_EDIT,
        WS_VISIBLE | WS_TABSTOP | WS_BORDER,
        400, 40, 200, 24,
        IDC_SIZE_MM,
        NULL,
        0
    },
    {
        CTRL_STATIC,
        WS_VISIBLE | SS_SIMPLE,
        400, 70, 200, 18, 
        IDC_SIZE_INCH, 
        "Equivalent to 0.00 inches",
        0,
        WS_EX_TRANSPARENT
    },
    {
        CTRL_BUTTON,
        WS_TABSTOP | WS_VISIBLE | BS_DEFPUSHBUTTON, 
        470, 110, 60, 25,
        IDOK, 
        "OK",
        0
    }
};
#endif

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
        sprintf (buff, "Equivalent to %.5f inches", len);
#endif
        SetDlgItemText (GetParent (hwnd), IDC_SIZE_INCH, buff);
    }
}       

static BITMAP bmp_bkgnd;
static int InputLenDialogBoxProc (HWND hDlg, int message, WPARAM wParam, LPARAM lParam)
{
    switch (message) {
    case MSG_INITDIALOG:
        SetWindowAdditionalData (hDlg, lParam);
        SendDlgItemMessage (hDlg, IDC_SIZE_MM, EM_LIMITTEXT, 15, 0);
        SetNotificationCallback (GetDlgItem (hDlg, IDC_SIZE_MM), my_notif_proc);
        return 1;
        
    case MSG_ERASEBKGND:
        {
            HDC hdc = (HDC)wParam;
            const RECT* clip = (const RECT*) lParam;
            BOOL fGetDC = FALSE;
            RECT rcTemp;
                    
            if (hdc == 0) {
                hdc = GetClientDC (hDlg);
                fGetDC = TRUE;
            }       
                    
            if (clip) {
                rcTemp = *clip;
                ScreenToClient (hDlg, &rcTemp.left, &rcTemp.top);
                ScreenToClient (hDlg, &rcTemp.right, &rcTemp.bottom);
                IncludeClipRect (hdc, &rcTemp);
            }

            FillBoxWithBitmap (hdc, 0, 0, 0, 0, &bmp_bkgnd);

            if (fGetDC)
                ReleaseDC (hdc);
            return 0;
        }

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
    JoinLayer(NAME_DEF_LAYER , "bmpbkgnd" , 0 , 0);
#endif
    
    if (LoadBitmap (HDC_SCREEN, &bmp_bkgnd, "bkgnd.jpg"))
         return 1;

    if (InputLenDialogBox (HWND_DESKTOP, &length) == IDOK)
        printf ("The length is %.5f mm.\n", length);

    UnloadBitmap (&bmp_bkgnd);
    return 0;
}

#ifndef _LITE_VERSION
#include <minigui/dti.c>
#endif

