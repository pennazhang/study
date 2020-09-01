/* 
** $Id: button.c,v 1.16 2007-10-25 08:25:16 weiym Exp $
**
** Listing 20.1
**
** button.c: Sample program for MiniGUI Programming Guide
**     Usage of BUTTON control.
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

#define IDC_LAMIAN              101
#define IDC_CHOUDOUFU           102
#define IDC_JIANBING            103
#define IDC_MAHUA               104
#define IDC_SHUIJIAO            105

#define IDC_XIAN                110
#define IDC_LA                  111

#define IDC_PROMPT              200

static DLGTEMPLATE DlgYourTaste =
{
    WS_BORDER | WS_CAPTION,
    WS_EX_NONE,
    0, 0, 370, 280,
#ifdef _LANG_ZHCN
    "您喜欢吃哪种风味的小吃",
#else
    "What flavor snack do you like?",
#endif
    0, 0,
    12, NULL,
    0
};

static CTRLDATA CtrlYourTaste[] =
{ 
    {
        "static",
        WS_VISIBLE | SS_GROUPBOX, 
        16, 10, 230, 160,
        IDC_STATIC,
#ifdef _LANG_ZHCN
        "可选小吃",
#else
        "optional snack",
#endif
        0
    },
    {
        "button",
        WS_VISIBLE | BS_AUTORADIOBUTTON | BS_CHECKED | WS_TABSTOP | WS_GROUP,
        36, 38, 200, 20,
        IDC_LAMIAN,
#ifdef _LANG_ZHCN
        "西北拉面",
#else
        "Northwest pulled noodle",
#endif
        0
    },
    {
        "button",
        WS_VISIBLE | BS_AUTORADIOBUTTON, 
        36, 64, 200, 20, 
        IDC_CHOUDOUFU, 
#ifdef _LANG_ZHCN
        "长沙臭豆腐",
#else
        "Changsha bad-smelling bean curd",
#endif
        0
    },
    {
        "button",
        WS_VISIBLE | BS_AUTORADIOBUTTON,
        36, 90, 200, 20,
        IDC_JIANBING,
#ifdef _LANG_ZHCN
        "山东煎饼",
#else
        "Shandong thin pancake",
#endif
        0
    },
    {
        "button",
        WS_VISIBLE | BS_AUTORADIOBUTTON,
        36, 116, 200, 20,
        IDC_MAHUA,
#ifdef _LANG_ZHCN
        "天津麻花",
#else
        "Tianjin fired dough twist",
#endif
        0
    },
    {
        "button",
        WS_VISIBLE | BS_AUTORADIOBUTTON,
        36, 142, 200, 20,
        IDC_SHUIJIAO,
#ifdef _LANG_ZHCN
        "成都红油水饺",
#else
        "Chengdu red oil boiled dumpling ",
#endif
        0
    },
    {
        "static",
        WS_VISIBLE | SS_GROUPBOX | WS_GROUP, 
        250, 10, 100, 160,
        IDC_STATIC,
#ifdef _LANG_ZHCN
        "口味",
#else
        "Flavor",
#endif
        0
    },
    {
        "button",
        WS_VISIBLE | BS_AUTOCHECKBOX,
        260, 38, 80, 20,
        IDC_XIAN,
#ifdef _LANG_ZHCN
        "偏咸",
#else
        "Partial salty",
#endif
        0
    },
    {
        "button",
        WS_VISIBLE | BS_AUTOCHECKBOX | BS_CHECKED, 
        260, 64, 80, 20, 
        IDC_LA, 
#ifdef _LANG_ZHCN
        "偏辣",
#else
        "Partial spicy",
#endif
        0
    },
    {
        "static",
        WS_VISIBLE | SS_LEFT | WS_GROUP,
        16, 180, 360, 20,
        IDC_PROMPT,
#ifdef _LANG_ZHCN
        "西北拉面是面食中的精品，但街上的兰州拉面除外！",
#else
        "Northwest pulled noodle is competitive product in the wheaten food",
#endif
        0
    },
    {
        "button",
        WS_VISIBLE | BS_DEFPUSHBUTTON | WS_TABSTOP | WS_GROUP,
        70, 220, 70, 28,
        IDOK, 
#ifdef _LANG_ZHCN
        "确定",
#else
        "OK",
#endif
        0
    },
    {
        "button",
        WS_VISIBLE | BS_PUSHBUTTON | WS_TABSTOP,
        200, 220, 70, 28,
        IDCANCEL,
#ifdef _LANG_ZHCN
        "取消",
#else
        "Cancel",
#endif
        0
    },
};

#ifdef _LANG_ZHCN
static char* prompts [] = {
    "西北拉面是面食中的精品，但街上的兰州拉面除外！",
    "长沙臭豆腐口味很独特，一般人适应不了。",
    "山东煎饼很难嚼 :(",
    "天津麻花很脆，很香！",
    "成都的红油水饺可真好吃啊！想起来就流口水。",
};
#else
static char* prompts [] = {
    "Northwest pulled noodle is competitive product in the wheaten food",
    "Changsha bad-smelling bean curd is very unique",
    "Shandong thin pancake is difficult to chew",
    "Tianjin fired dough twist is very fragile",
    "Chengdu red oil boiled dumpling is captivating",
};
#endif

static void my_notif_proc (HWND hwnd, int id, int nc, DWORD add_data)
{
    if (nc == BN_CLICKED) {
        SetWindowText (GetDlgItem (GetParent (hwnd), IDC_PROMPT), prompts [id - IDC_LAMIAN]);
    }
}

static int DialogBoxProc2 (HWND hDlg, int message, WPARAM wParam, LPARAM lParam)
{
    switch (message) {
    case MSG_INITDIALOG:
        {
            int i;
            for (i = IDC_LAMIAN; i <= IDC_SHUIJIAO; i++)
                SetNotificationCallback (GetDlgItem (hDlg, i), my_notif_proc);
        }
        return 1;
        
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
    JoinLayer(NAME_DEF_LAYER , "button" , 0 , 0);
#endif
    
    DlgYourTaste.controls = CtrlYourTaste;
    
    DialogBoxIndirectParam (&DlgYourTaste, HWND_DESKTOP, DialogBoxProc2, 0L);

    return 0;
}

#ifndef _LITE_VERSION
#include <minigui/dti.c>
#endif

