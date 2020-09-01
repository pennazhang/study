/*
** $Id: combobox.c,v 1.12 2007-10-25 08:31:37 weiym Exp $
** 
** Listing 23.1
**
** combobox.c: Sample program for MiniGUI Programming Guide
**      The usage of COMBOBOX control.
** 
** Copyright (C) 2004 ~ 2007 Feynman Software.
**
** License: GPL
*/

#include <stdio.h>

#include <minigui/common.h>
#include <minigui/minigui.h>
#include <minigui/gdi.h>
#include <minigui/window.h>
#include <minigui/control.h>

#define IDC_HOUR   100
#define IDC_MINUTE 110
#define IDC_SECOND 120
#define IDL_DAXIA  200

#define IDC_PROMPT 300

static CTRLDATA CtrlMyDate[] =
{ 
    {
        "static",
        WS_CHILD | SS_RIGHT | WS_VISIBLE,
         10, 20, 90, 20,
        IDC_STATIC,
#ifdef _LANG_ZHCN
        "�Ҵ�����",
#else
        "I will at ",
#endif
        0
    },
    {
        CTRL_COMBOBOX,
        WS_CHILD | WS_VISIBLE | 
            CBS_READONLY | CBS_AUTOSPIN | CBS_AUTOLOOP,
        100, 18, 40, 20,
        IDC_HOUR, 
        "",
        0
    },
    {
        "static",
        WS_CHILD | SS_CENTER | WS_VISIBLE,
        140, 20, 20, 20,
        IDC_STATIC,
#ifdef _LANG_ZHCN
        "ʱ",
#else
        ":",
#endif
        0
    },
    {
        CTRL_COMBOBOX,
        WS_CHILD | WS_VISIBLE | 
            CBS_READONLY | CBS_AUTOSPIN | CBS_AUTOLOOP,
        160, 18, 40, 20,
        IDC_MINUTE,
        "",
        0
    },
    {
        "static",
        WS_CHILD | SS_CENTER | WS_VISIBLE,
        200, 20, 30, 20,
        IDC_STATIC,
#ifdef _LANG_ZHCN
        "ȥ��",
#else
        " meet ",
#endif
        0
    },
    {
        CTRL_COMBOBOX,
        WS_VISIBLE | CBS_DROPDOWNLIST | CBS_NOTIFY,
        240, 15, 100, 25,
        IDL_DAXIA,
        "",
        80
    },
#ifdef _LANG_ZHCN
    {
        "static",
        WS_CHILD | SS_RIGHT | WS_VISIBLE,
        10, 50, 280, 20,
        IDC_PROMPT,
        "��Ƨ",
        0
    },
#else
    {
        "static",
        WS_CHILD | SS_RIGHT | WS_VISIBLE,
        10, 50, 330, 30,
        IDC_PROMPT,
        "Zeus is the leader of the gods and god of the sky and thunder in Greek mythology.",
        0
    },
#endif
    {
        CTRL_BUTTON,
        WS_VISIBLE | BS_DEFPUSHBUTTON | WS_TABSTOP | WS_GROUP,
        10, 90, 130, 25,
        IDOK, 
#ifdef _LANG_ZHCN
        "ȷ��",
#else
        "OK",
#endif
        0
    },
    {
        "button",
        WS_VISIBLE | BS_PUSHBUTTON | WS_TABSTOP,
        210, 90, 130, 25,
        IDCANCEL,
#ifdef _LANG_ZHCN
        "ȡ��",
#else
        "Cancel",
#endif
        0
    },
};

static DLGTEMPLATE DlgMyDate =
{
    WS_BORDER | WS_CAPTION,
    WS_EX_NONE,
    100, 100, 360, 150,
#ifdef _LANG_ZHCN
    "Լ�����",
#else
    "Meeting  Plan",
#endif
    0, 0,
    TABLESIZE(CtrlMyDate),
	NULL,
    0
};

#ifdef _LANG_ZHCN
static const char* daxia [] =
{
    "��ҩʦ",
    "ŷ����",
    "�λ�ү",
    "���߹�",
    "�ܲ�ͨ",
    "����",
    "����",
};

static const char* daxia_char [] =
{
    "��Ƨ",
    "��",
    "�ٴȱ�",
    "һ������",
    "��Ƥ����������",
    "ɵ����",
    "����",
};
#else
static const char* daxia [] =
{
    "Zeus",
    "Pan",
    "Apollo",
    "Heracles",
    "Achilles",
    "Jason",
    "Theseus",
};
static const char* daxia_char [] =
{
    "Zeus is the leader of the gods and god of the sky and thunder in Greek mythology.",
    "Pan  is the Greek god who watches over shepherds and their flocks.",
    "Apollo is a god in Greek and Roman mythology, the son of Zeus and Leto, and the twin of Artemis.",
    "Heracles was the greatest of the mythical Greek heroes, best known for his superhuman strength and many stories are told of his life.",
    "Achilles was the greatest warrior in the Trojan War.",
    "Jason is a hero of Greek mythology. His father was Aeson, the rightful king of Iolcus.",
    "Theseus was a legendary king of Athens. Theseus was considered by Athenians as the great reformer.",
};
#endif

static void daxia_notif_proc (HWND hwnd, int id, int nc, DWORD add_data)
{
    if (nc == CBN_SELCHANGE) {
        int cur_sel = SendMessage (hwnd, CB_GETCURSEL, 0, 0);
	if (cur_sel >= 0) {
		SetWindowText (GetDlgItem (GetParent(hwnd), IDC_PROMPT), daxia_char [cur_sel]);
	}
    }
}

static void prompt (HWND hDlg)
{
#ifdef _LANG_ZHCN
    char date [1024] = "���Լ�����ݣ�\n";
#else
    char date [1024] = "\n";
#endif
    int hour = SendDlgItemMessage(hDlg, IDC_HOUR, CB_GETSPINVALUE, 0, 0);
    int min = SendDlgItemMessage(hDlg, IDC_MINUTE, CB_GETSPINVALUE, 0, 0);
    int sel = SendDlgItemMessage(hDlg, IDL_DAXIA, CB_GETCURSEL, 0, 0);
#ifdef _LANG_ZHCN
    sprintf (date, "�������ڽ��� %02d:%02d ȥ���Ǹ�%s��%s", hour, min, 
		    daxia_char [sel], daxia [sel]);
    MessageBox (hDlg, date, "Լ������", MB_OK | MB_ICONINFORMATION);
#else
    sprintf (date, "You will meet %s at %02d:%02d", daxia [sel],hour, min);
    MessageBox (hDlg, date, "Meeting Plan", MB_OK | MB_ICONINFORMATION);
#endif
}

static int MyDateBoxProc (HWND hDlg, int message, WPARAM wParam, LPARAM lParam)
{
    int i;
    switch (message) {
    case MSG_INITDIALOG:
        SendDlgItemMessage(hDlg, IDC_HOUR, CB_SETSPINFORMAT, 0, (LPARAM)"%02d");
        SendDlgItemMessage(hDlg, IDC_HOUR, CB_SETSPINRANGE, 0, 23);
        SendDlgItemMessage(hDlg, IDC_HOUR, CB_SETSPINVALUE, 20, 0);
        SendDlgItemMessage(hDlg, IDC_HOUR, CB_SETSPINPACE, 1, 1);

        SendDlgItemMessage(hDlg, IDC_MINUTE, CB_SETSPINFORMAT, 0, (LPARAM)"%02d");
        SendDlgItemMessage(hDlg, IDC_MINUTE, CB_SETSPINRANGE, 0, 59);
        SendDlgItemMessage(hDlg, IDC_MINUTE, CB_SETSPINVALUE, 0, 0);
        SendDlgItemMessage(hDlg, IDC_MINUTE, CB_SETSPINPACE, 1, 2);

        for (i = 0; i < 7; i++) {
            SendDlgItemMessage(hDlg, IDL_DAXIA, CB_ADDSTRING, 0, (LPARAM)daxia [i]);
        }

        SetNotificationCallback (GetDlgItem (hDlg, IDL_DAXIA), daxia_notif_proc);
        SendDlgItemMessage(hDlg, IDL_DAXIA, CB_SETCURSEL, 0, 0);
#ifdef _LANG_ZHCN
	SetWindowText (GetDlgItem (hDlg, IDC_PROMPT), daxia_char [0]);
#endif
        return 1;
        
    case MSG_COMMAND:
        switch (wParam) {
        case IDOK:
            prompt (hDlg);
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
    JoinLayer(NAME_DEF_LAYER , "combobox" , 0 , 0);
#endif
    
    DlgMyDate.controls = CtrlMyDate;
    
    DialogBoxIndirectParam (&DlgMyDate, HWND_DESKTOP, MyDateBoxProc, 0L);

    return 0;
}

#ifndef _LITE_VERSION
#include <minigui/dti.c>
#endif

