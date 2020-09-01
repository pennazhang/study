/*
** $Id: scrollwnd.c,v 1.10 2007-08-30 01:20:10 xwyan Exp $
**
** scrollwnd.c: Sample program for MiniGUI Programming Guide
**      Usage of SCROLLWND control.
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


#define IDC_SCROLLWND          100
#define ID_ZOOMIN              200
#define ID_ZOOMOUT             300

static HWND hScrollWnd;
static BITMAP bmp_bkgnd;
static float current_scale = 1;

static int pic_container_proc (HWND hWnd, int message, WPARAM wParam, LPARAM lParam)
{
    switch (message) {

    case MSG_PAINT:
    {
        HDC hdc = BeginPaint (hWnd);
        FillBoxWithBitmap (hdc, 0, 0, current_scale * bmp_bkgnd.bmWidth, 
                        current_scale * bmp_bkgnd.bmHeight, &bmp_bkgnd);
        EndPaint (hWnd, hdc);
        return 0;
    }

    case MSG_ERASEBKGND:
        return 0;

    }

    return DefaultContainerProc (hWnd, message, wParam, lParam);
}

static int
ImageViewerProc (HWND hDlg, int message, WPARAM wParam, LPARAM lParam)
{

    switch (message)
    {

    case MSG_INITDIALOG:
    {
        hScrollWnd = GetDlgItem (hDlg, IDC_SCROLLWND);

        SendMessage (hScrollWnd, SVM_SETCONTAINERPROC, 0, (LPARAM)pic_container_proc);
        SendMessage (hScrollWnd, SVM_SETCONTRANGE, bmp_bkgnd.bmWidth, bmp_bkgnd.bmHeight);

        break;
    }

    case MSG_COMMAND:
    {
        int id = LOWORD(wParam);

        if (id == ID_ZOOMIN || id == ID_ZOOMOUT) {
            current_scale += (id == ID_ZOOMIN) ? 0.2 : -0.2;
            if (current_scale < 0.1)
                current_scale = 0.1;

            SendMessage (hScrollWnd, SVM_SETCONTRANGE, 
                         current_scale * bmp_bkgnd.bmWidth, 
                         current_scale * bmp_bkgnd.bmHeight);
            InvalidateRect (hScrollWnd, NULL, TRUE);
        }

        break;
    }

    case MSG_CLOSE:
        EndDialog (hDlg, 0);
        return 0;

    }

    return DefaultDialogProc (hDlg, message, wParam, lParam);
}

static CTRLDATA CtrlViewer[] =
{
    {
        "scrollwnd",
        WS_BORDER | WS_CHILD | WS_VISIBLE | WS_VSCROLL | WS_HSCROLL,
         10, 10, 300, 200,
        IDC_SCROLLWND,
        "image viewer",
        0
    },
    {
        CTRL_BUTTON,
        WS_TABSTOP | WS_VISIBLE | BS_DEFPUSHBUTTON, 
        20, 220, 60, 25,
        ID_ZOOMIN, 
        "Zoom in",
        0
    },
    {
        CTRL_BUTTON,
        WS_TABSTOP | WS_VISIBLE | BS_PUSHBUTTON, 
        220, 220, 60, 25,
        ID_ZOOMOUT, 
        "Zoom out",
        0
    }
};

static DLGTEMPLATE DlgViewer =
{
    WS_BORDER | WS_CAPTION,
    WS_EX_NONE,
    0, 0, 350, 280,
    "Image Viewer",
    0, 0,
    TABLESIZE(CtrlViewer), CtrlViewer,
    0
};

int MiniGUIMain (int argc, const char* argv[])
{
#ifdef _MGRM_PROCESSES
    JoinLayer(NAME_DEF_LAYER , "scrollwnd" , 0 , 0);
#endif
    
    if (LoadBitmap (HDC_SCREEN, &bmp_bkgnd, "bkgnd.jpg"))
         return 1;

    DialogBoxIndirectParam (&DlgViewer, HWND_DESKTOP, ImageViewerProc, 0L);

    UnloadBitmap (&bmp_bkgnd);
    return 0;
}

#ifndef _LITE_VERSION
#include <minigui/dti.c>
#endif

