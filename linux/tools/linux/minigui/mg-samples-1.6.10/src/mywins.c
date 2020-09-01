/* 
** $Id: mywins.c,v 1.15 2007-10-25 08:12:27 weiym Exp $
**
** Listing 20.1
**
** button.c: Sample program for MiniGUI Programming Guide
**     Usage of mywins Interface.
**
** Copyright (C) 2004 ~ 2007 Feynman Software.
**
** License: GPL
*/

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

#include <minigui/common.h>
#include <minigui/minigui.h>
#include <minigui/gdi.h>
#include <minigui/window.h>
#include <minigui/control.h>
#include <minigui/mywindows.h>
#include <minigui/mgext.h>

#define IDC_MMB                 101
#define IDC_MWM                 102
#define IDC_TCB                 103
#define IDC_EEW                 104
#define IDC_STW                 105
#define IDC_COLOR               106
#define IDC_PROMPT              107
#define IDC_HELPMSG             108
#define IDC_MENU                109
#define IDC_ENTRY               110

#define IDC_PROGRESS            200

#define IDC_WB1                 210
#define IDC_WB2                 211
#define IDC_WB3                 212

#define MSG_INITBOX     MSG_USER+16
#define MSG_PROGRESS    MSG_USER+32

static DLGTEMPLATE DlgYourTaste =
{
    WS_BORDER | WS_CAPTION,
    WS_EX_NOCLOSEBOX ,
    0, 0, 370, 280,
#ifdef _LANG_ZHCN
    "mywindow �ӿ�ʵ��",
#else
    "Examples for mywindow interfaces",
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
        16, 10, 230, 215,
        IDC_STATIC,
#ifdef _LANG_ZHCN
        "MyWins ����",
#else
        "MyWins Examples",
#endif
        0
    },
    {
        "button",
        WS_VISIBLE | BS_PUSHBUTTON ,
        36, 38, 200, 20,
        IDC_MMB,
#ifdef _LANG_ZHCN
        "myMessageBox ����",
#else
        "myMessageBox example",
#endif
        0
    },
    {
        "button",
        WS_VISIBLE | BS_PUSHBUTTON, 
        36, 64, 200, 20, 
        IDC_MWM, 
#ifdef _LANG_ZHCN
        "myWinMessage ����",
#else
        "myWinMessage example",
#endif
        0
    },
    {
        "button",
        WS_VISIBLE | BS_PUSHBUTTON,
        36, 90, 200, 20,
        IDC_TCB,
#ifdef _LANG_ZHCN
        "myWinTernary ����",
#else
        "myWinTernary example",
#endif
        0
    },
    {
        "button",
        WS_VISIBLE | BS_PUSHBUTTON,
        36, 116, 200, 20,
        IDC_EEW,
#ifdef _LANG_ZNCH
        "errorWindow ����",
#else
        "errorWindow example",
#endif
        0
    },
    {
        "button",
        WS_VISIBLE | BS_PUSHBUTTON,
        36, 142, 200, 20,
        IDC_STW,
#ifdef _LANG_ZHCN
        "StatusWin ����",
#else
        "StatusWin example",
#endif
        0
    },
    {
        "button",
        WS_VISIBLE | BS_PUSHBUTTON, 
        250, 40, 100, 25,
        IDC_COLOR,
#ifdef _LANG_ZHCN
        "ѡ��",
#else
        "Choice",
#endif
        0
    },
    {
        "button",
        WS_VISIBLE | BS_PUSHBUTTON,
        36, 168, 200, 20,
        IDC_PROMPT,
#ifdef _LANG_ZHCN
        "ProcessWin ���ӣ�",
#else
        "ProcessWin example!",
#endif 
        0
    },
    {
        "button",
        WS_VISIBLE | BS_PUSHBUTTON ,
        250, 120, 100, 25,
        IDC_HELPMSG,
#ifdef _LANG_ZHCN
        "����",
#else
        "Help",
#endif
        0
    },
    {
        "button",
        WS_VISIBLE | BS_PUSHBUTTON ,
        36, 194, 96, 20,
        IDC_MENU,
#ifdef _LANG_ZHCN
        "�˵�",
#else
        "Menu",
#endif
        0
    },
    {
        "button",
        WS_VISIBLE | BS_PUSHBUTTON ,
        140, 194, 96, 20,
        IDC_ENTRY,
#ifdef _LANG_ZHCN
        "���",
#else
        "Entry",
#endif
        0
    },
    {
        "button",
        WS_VISIBLE | BS_DEFPUSHBUTTON | WS_TABSTOP | WS_GROUP,
        250, 190, 100, 25,
        IDOK, 
#ifdef _LANG_ZHCN
        "�˳�",
#else
        "Exit",
#endif
        0
    },
    {
        NULL
    }
};


static char * Flavor [] = {
#ifdef _LANG_ZHCN
        "��ɫ",
        "��ɫ",
        "ѡ����ϲ������ɫ��",
#else
        "Red",
        "Blue",
        "Choice you like color!",
#endif
        NULL
    };

static char *Ternary [] = {
#ifdef _LANG_ZHCN
        "��ϲ��",
        "��ϲ��",
        "�ǳ�ϲ��",
#else
        "I like it",
        "I don't like it",
        "I like it very much",
#endif
        NULL
};

#ifdef _LANG_ZHCN
static char* prompts [] = {
    "myMessageBox ��һ���򵥵���Ϣ�� , ����ʹ��printf���ĸ�ʽ��",
    "myWinMessage ��һ����ʾ����,����һ����ť��",
    "��ϲ�������Ϸ��?",
    "���Ǹ������õ���ʾ���ڣ�",
    "ϵͳ���ڳ�ʼ��,���Ե�...",
    0
};
#else
static char* prompts [] = {
    "myMessageBox is a simple messagebox window and have printf like format.",
    "myWinMessage is a message Window include a button.",
    "Would you like this game?",
    "This is a very usefull window!",
    "The System is initialize , please wait...",
    0
};
#endif

static int pListItem = 0;
static  char * Pmenu [] = {
#ifdef _LANG_ZHCN
        "ѡ��һ",
        "ѡ���",
        "ѡ����",
        "ѡ����",
#else
        "Item 1",
        "Item 2",
        "Item 3",
        "Item 4",
#endif
        NULL
};

static myWINBUTTON MenuItems[] = {
    {
#ifdef _LANG_ZHCN
        "�ַ���1",
#else
        "string 1",
#endif
        IDC_WB1,
        WS_VISIBLE | BS_PUSHBUTTON  
    },
    {
#ifdef _LANG_ZHCN
        "�ַ���2",
#else
        "string 2",
#endif
        IDC_WB2,
        WS_VISIBLE | BS_PUSHBUTTON  
    },
    {
#ifdef _LANG_ZHCN
        "�ַ���3",
#else
        "string 3",
#endif
        IDC_WB3,
        WS_VISIBLE | BS_PUSHBUTTON  
    },
    {
    NULL
    }
};

static myWINENTRY EntryItems [] = {
    {
#ifdef _LANG_ZHCN
        "�༭��1",
#else
        "Entry 1",
#endif
        &Pmenu[0] ,
        80,
        ES_LEFT ,
    },
    {
#ifdef _LANG_ZHCN
        "�༭��2",
#else
        "Entry 2",
#endif
        &Pmenu[1] ,
        80,
        ES_LEFT ,
    },
    {
#ifdef _LANG_ZHCN
        "�༭��3",
#else
        "Entry 3",
#endif
        &Pmenu[2] ,
        80,
        ES_LEFT ,
    },
    {
    NULL
    }
};


static void my_notif_proc (HWND hwnd, int id, int nc, DWORD add_data)
{
    if (nc == BN_CLICKED) {
        switch (id)
        {
            case IDC_MMB:
                myMessageBox(GetParent (hwnd) , MB_OK , "myMessageBox:" , "NOTE: \n %s \n" , prompts [id - IDC_MMB]);
            break;    
            case IDC_MWM:
                myWinMessage(GetParent (hwnd) , "myWinMessage:" , "my button!" , "NOTE: \n %s \n" , prompts [id - IDC_MMB]);
            break;
            case IDC_COLOR:
                myWinChoice(GetParent (hwnd) , "myWinChoice" , Flavor[0] , Flavor[1] , "NOTE: \n %s \n" , Flavor[2]); 
            break;
            case IDC_TCB:
                myWinTernary(GetParent (hwnd) , "myWinTernary" , Ternary[0] , Ternary[1] , Ternary[2] , "NOTE: \n %s \n" , prompts [id - IDC_MMB]); 
            break;
            case IDC_EEW:
               errorWindow(GetParent (hwnd) , prompts [id - IDC_MMB] , "ErrorWindow");       
            break;
            case IDC_STW:
                PostMessage(GetParent (hwnd) , MSG_INITBOX , id , 0);
            break;
            case IDC_PROMPT:
                PostMessage(GetParent (hwnd) , MSG_PROGRESS , IDC_PROGRESS , 0);
            break;
            case IDC_HELPMSG:
                myWinHelpMessage(GetParent (hwnd) , 300 , 200 , "HelpMessage", 
            "We use the SpinBox control in this Help Message Box.\n\n"
            "You can click the up arrow of the control to scroll up "
            "the message, and click the down arrow of the control to scroll down. "
            "You can also scroll the message by typing ArrowDown and ArrowUp keys.\n\n"
            "In your application, you can call 'myWinHelpMessage' function "
            "to build a Help Message box like this.\n\n"
            "The Help Message Box is useful for some PDA-like applications.\n\n"
            "The SpinBox control allways have the fixed width and height. "
            "You can read the source of 'ext/control/spinbox.c' to know how to "
            "build such a control.\n\n"
            "If you want to know how to use this control, please read the "
            "source of 'ext/mywins/helpwin.c' in the MiniGUI source tree.");
            break;

            case IDC_MENU:
                myWinMenu(GetParent (hwnd) , "Menu" , "Items" , 80 , 200 , Pmenu , &pListItem , MenuItems);
            break;
            case IDC_ENTRY:
                myWinEntries(GetParent (hwnd) , "Entry" , "Entries" , 120 , 80 , TRUE , EntryItems , MenuItems); 
            break;
        }
    }
}

static int DialogBoxProc2 (HWND hDlg, int message, WPARAM wParam, LPARAM lParam)
{
    static HWND stahwnd = 0;
    static HWND prohwnd = 0;
    static HWND tiphwnd = 0;
    static int ppos = 0;
    switch (message) {
    case MSG_INITDIALOG:
        {
            int i;
            for (i = IDC_MMB; i <= IDC_ENTRY; i++)
                SetNotificationCallback (GetDlgItem (hDlg, i), my_notif_proc);
        }
        return 1;
        
    case MSG_COMMAND:
        switch (wParam) {
        case IDOK:
            EndDialog (hDlg, wParam);
            break;
        }
        break;
    case MSG_INITBOX: 
            if (!stahwnd)
            {
               stahwnd = createStatusWin(hDlg , 400 , 50 , "status" , prompts [wParam - IDC_MMB]);
#ifdef _TIMER_UNIT_10MS
               SetTimer(hDlg , 110 , 1000);
#else
               SetTimer(hDlg , 110 , 1000*10);
#endif
            }
           break;
    case MSG_PROGRESS:
#ifdef _CTRL_PROGRESSBAR
           if (!prohwnd)
           {
               prohwnd = createProgressWin(hDlg , "Progress" , "setup...." , IDC_PROGRESS , 100);
#ifdef _TIMER_UNIT_10MS
               SetTimer(hDlg , 100  , 100);
#else
               SetTimer(hDlg , 100  , 100*10);
#endif
               SendDlgItemMessage(prohwnd , IDC_PROGRESS , PBM_SETSTEP , 10 , 0);
           }
#endif
           break;
    case MSG_MOUSEMOVE:
            {
                int pos_x , pos_y;
                RECT rc;
                pos_x = LOSWORD(lParam);
                pos_y = HISWORD(lParam);
                GetWindowRect(GetDlgItem (hDlg , IDC_COLOR) , &rc);
                
                if (PtInRect(&rc , pos_x , pos_y) && !tiphwnd)
                {
                    tiphwnd = createToolTipWin (hDlg , pos_x , pos_y , 5000 , "This is the tip window!");
                }

                if (!PtInRect(&rc , pos_x , pos_y) && tiphwnd)
                {
                   destroyToolTipWin(tiphwnd); 
                   tiphwnd = 0;
                }
                
            }
           break;
    case MSG_TIMER:
            {
#ifdef _CTRL_PROGRESSBAR
                    if (wParam == 100 && prohwnd)
                    { 
                        if (ppos < 10)
                        {
                            SendDlgItemMessage(prohwnd , IDC_PROGRESS , PBM_SETPOS , 10*ppos , 0);
                            ppos++;
                        }
                        else
                        {
                            KillTimer(hDlg , 100);
                            destroyProgressWin(prohwnd);
                            prohwnd = 0;
                            ppos = 0;
                        }
                    }
#endif
                  if (wParam == 110 && stahwnd)
                  {     
                    KillTimer(hDlg , 110);
                    destroyStatusWin (stahwnd);
                    stahwnd = 0;
                  }
            }
           break;
    }
    
    return DefaultDialogProc (hDlg, message, wParam, lParam);
}

int MiniGUIMain (int argc, const char* argv[])
{
#ifdef _MGRM_PROCESSES
    JoinLayer(NAME_DEF_LAYER , "helloworld" , 0 , 0);
#endif

    if (!InitMiniGUIExt())
        return -1;
    
    DlgYourTaste.controls = CtrlYourTaste;
    
    DialogBoxIndirectParam (&DlgYourTaste, HWND_DESKTOP, DialogBoxProc2, 0L);

    return 0;
}

#ifndef _LITE_VERSION
#include <minigui/dti.c>
#endif

