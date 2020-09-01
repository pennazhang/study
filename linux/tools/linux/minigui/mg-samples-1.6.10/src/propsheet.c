/* 
** $Id: propsheet.c,v 1.12 2007-10-25 07:56:45 weiym Exp $
**
** Listing 28.1
**
** propsheet.c: Sample program for MiniGUI Programming Guide
**      Usage of PORPSHEET control.
**
** Copyright (C) 2004 ~ 2007  Feynman Software.
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

#define PAGE_VERSION	1
#define PAGE_CPU	2
#define PAGE_MEMINFO	3
#define PAGE_PARTITION  4
#define PAGE_MINIGUI    5

#define IDC_PROPSHEET	100

#define IDC_SYSINFO	100

static DLGTEMPLATE PageSysInfo =
{
    WS_BORDER | WS_CAPTION,
    WS_EX_NONE,
    0, 0, 0, 0,
    "",
    0, 0,
    1, NULL,
    0
};

static CTRLDATA CtrlSysInfo [] =
{ 
    {
        CTRL_STATIC,
        WS_VISIBLE | SS_LEFT, 
        10, 10, 370, 160,
        IDC_SYSINFO,
        "",
        0
    }
};

static size_t read_sysinfo (const char* file, char* buff, size_t buf_len)
{
     size_t size;
     FILE* fp = fopen (file, "r");

     if (fp == NULL) return 0;

     size = fread (buff, 1, buf_len, fp);

     fclose (fp);
     return size;
}

#define BUF_LEN 10240

static void get_systeminfo (HWND hDlg)
{
    int type;
    HWND hwnd;
    char buff [BUF_LEN + 1];
    size_t size = 0;

    type = (int)GetWindowAdditionalData (hDlg);
    hwnd = GetDlgItem (hDlg, IDC_SYSINFO);

    buff [BUF_LEN] = 0;
    switch (type) {
    case PAGE_VERSION:
	    size = read_sysinfo ("/proc/version", buff, BUF_LEN);
    	    buff [size] = 0;
	    break;

    case PAGE_CPU:
	    size = read_sysinfo ("/proc/cpuinfo", buff, BUF_LEN);
    	    buff [size] = 0;
	    break;

    case PAGE_MEMINFO:
	    size = read_sysinfo ("/proc/meminfo", buff, BUF_LEN);
    	    buff [size] = 0;
	    break;

    case PAGE_PARTITION:
	    size = read_sysinfo ("/proc/partitions", buff, BUF_LEN);
    	    buff [size] = 0;
	    break;

    case PAGE_MINIGUI:
	    size = snprintf (buff, BUF_LEN,
		"MiniGUI version %d.%d.%d.\n"
		"Copyright (C) 1998-2007 Feynman Software.\n\n"
		"MiniGUI is a fast, lightweight, and cross-OS embedded "
        "GUI support system.\n\n"
		"For more information, please visit\n\n"
		"http://www.minigui.com\n",
		MINIGUI_MAJOR_VERSION, MINIGUI_MINOR_VERSION, MINIGUI_MICRO_VERSION);
	    break;
    }

    if (size) {
	    SetWindowText (hwnd, buff);
    }
}

static int SysInfoPageProc (HWND hDlg, int message, WPARAM wParam, LPARAM lParam)
{
    switch (message) {
    case MSG_INITPAGE:
	get_systeminfo (hDlg);
        break;

    case MSG_SHOWPAGE:
        return 1;

    case MSG_SHEETCMD:
	if (wParam == IDOK) 
	    get_systeminfo (hDlg);
        return 0;
    }
    
    return DefaultPageProc (hDlg, message, wParam, lParam);
}

static int PropSheetProc (HWND hDlg, int message, WPARAM wParam, LPARAM lParam)
{
    switch (message) {
        case MSG_INITDIALOG:
        {
            HWND pshwnd = GetDlgItem (hDlg, IDC_PROPSHEET);

            PageSysInfo.controls = CtrlSysInfo;
#ifdef _LANG_ZHCN
	    PageSysInfo.caption = "版本信息";
#else
	    PageSysInfo.caption = "Version Info";
#endif
			PageSysInfo.dwAddData = PAGE_VERSION;
            SendMessage (pshwnd, PSM_ADDPAGE, (WPARAM)&PageSysInfo, (LPARAM)SysInfoPageProc);
#ifdef _LANG_ZHCN
	    PageSysInfo.caption = "CPU 信息";
#else
	    PageSysInfo.caption = "CPU Info";
#endif
	    PageSysInfo.dwAddData = PAGE_CPU;
            SendMessage (pshwnd, PSM_ADDPAGE, (WPARAM)&PageSysInfo, (LPARAM)SysInfoPageProc);
#ifdef _LANG_ZHCN
	    PageSysInfo.caption = "内存信息";
#else
	    PageSysInfo.caption = "Mem Info";
#endif
	    PageSysInfo.dwAddData = PAGE_MEMINFO;
            SendMessage (pshwnd, PSM_ADDPAGE, (WPARAM)&PageSysInfo, (LPARAM)SysInfoPageProc);
#ifdef _LANG_ZHCN
	    PageSysInfo.caption = "分区信息";
#else
	    PageSysInfo.caption = "Partition Info";
#endif
	    PageSysInfo.dwAddData = PAGE_PARTITION;
            SendMessage (pshwnd, PSM_ADDPAGE, (WPARAM)&PageSysInfo, (LPARAM)SysInfoPageProc);
#ifdef _LANG_ZHCN
	    PageSysInfo.caption = "MiniGUI 信息";
#else
	    PageSysInfo.caption = "MiniGUI Info";
#endif
	    PageSysInfo.dwAddData = PAGE_MINIGUI;
            SendMessage (pshwnd, PSM_ADDPAGE, (WPARAM)&PageSysInfo, (LPARAM)SysInfoPageProc);
            break;
        }

        case MSG_COMMAND:
        switch (wParam) {
            case IDOK:
                SendDlgItemMessage (hDlg, IDC_PROPSHEET, PSM_SHEETCMD, IDOK, 0);
                break;

            case IDCANCEL:
                EndDialog (hDlg, wParam);
            break;
        }
        break;
    }

    return DefaultDialogProc (hDlg, message, wParam, lParam);
}

static DLGTEMPLATE DlgPropSheet =
{
    WS_BORDER | WS_CAPTION,
    WS_EX_NONE,
    0, 0, 410, 275,
#ifdef _LANG_ZHCN
    "系统信息",
#else
    "System Info",
#endif
    0, 0,
    3, NULL,
    0
};

static CTRLDATA CtrlPropSheet[] =
{ 
    {
        CTRL_PROPSHEET,
        WS_VISIBLE | PSS_COMPACTTAB, 
        10, 10, 390, 200,
        IDC_PROPSHEET,
        "",
        0
    },
    {
        CTRL_BUTTON,
        WS_VISIBLE | BS_DEFPUSHBUTTON | WS_TABSTOP | WS_GROUP,
        10, 220, 140, 25,
        IDOK, 
        "Refresh",
        0
    },
    {
        CTRL_BUTTON,
        WS_VISIBLE | BS_PUSHBUTTON | WS_TABSTOP,
        260, 220, 140, 25,
        IDCANCEL,
        "Close",
        0
    },
};

int MiniGUIMain (int argc, const char* argv[])
{
#ifdef _MGRM_PROCESSES
    JoinLayer(NAME_DEF_LAYER , "propsheet" , 0 , 0);
#endif
    
    DlgPropSheet.controls = CtrlPropSheet;
    
    DialogBoxIndirectParam (&DlgPropSheet, HWND_DESKTOP, PropSheetProc, 0L);

    return 0;
}

#ifndef _LITE_VERSION
#include <minigui/dti.c>
#endif

