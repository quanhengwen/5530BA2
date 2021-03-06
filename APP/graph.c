/******************************************************************/
/* 名称：                                                  				*/
/* 效果：                                                        */
/* 内容：曲线图界面                                             */
/* 作者：                                                 */
/* 联系方式QQ:                                        */


#include "MainTask.h"
#include  "gui.h"
#include "DIALOG.h"
#include "my_register.h" 
#include "tm1650.h"
#include "stdio.h"
#include "stdlib.h"
#include "key.h"
#include "string.h"
#include "beep.h"

WM_HWIN hWinG;
extern vu8 double_sw;
WM_HWIN DrawGraph(void);
/*********************************************************************
*
*       Defines
*
**********************************************************************
*/
#define ID_WINDOW_3     (GUI_ID_USER + 0x22)
//#define ID_TEXT_0     	(GUI_ID_USER + 0x13)
//#define ID_TEXT_1     	(GUI_ID_USER + 0x14)
//#define ID_TEXT_3     	(GUI_ID_USER + 0x15)
//#define ID_TEXT_4     	(GUI_ID_USER + 0x16)
//#define ID_TEXT_6     	(GUI_ID_USER + 0x17)
#define ID_BUTTON_18    (GUI_ID_USER + 0x23)
#define ID_BUTTON_19    (GUI_ID_USER + 0x24)
#define ID_BUTTON_20    (GUI_ID_USER + 0x25)
#define ID_BUTTON_21   	(GUI_ID_USER + 0x26)
#define ID_BUTTON_22   	(GUI_ID_USER + 0x27)
#define ID_BUTTON_23    (GUI_ID_USER + 0x28)
#define ID_GRAPH_0      (GUI_ID_USER + 0x29)
#define ID_TEXT_128     (GUI_ID_USER + 0x11F)

#define ID_TimerTime3    4
// USER START (Optionally insert additional defines)
// USER END

/*********************************************************************
*
*       Static data
*
**********************************************************************
*/
static GRAPH_SCALE_Handle hScaleV; //--------------锛�1锛�
static GRAPH_SCALE_Handle hScaleH;
static GRAPH_DATA_Handle VData;
static GRAPH_DATA_Handle IData;
//static GRAPH_DATA_Handle  ahData;
// USER START (Optionally insert additional static data)
// USER END

/*********************************************************************
*
*       _aDialogCreate
*/
static const GUI_WIDGET_CREATE_INFO _aDialogCreate3[] = {
  { WINDOW_CreateIndirect, "G", ID_WINDOW_3, 0, 0, 480, 272, 0, 0x0, 0 },
//  { TEXT_CreateIndirect, "Text", ID_TEXT_0, 28, 74, 64, 32, 0, 0x64, 0 },
//  { TEXT_CreateIndirect, "Text", ID_TEXT_1, 28, 144, 64, 32, 0, 0x64, 0 },
//  { TEXT_CreateIndirect, "Text", ID_TEXT_3, 217, 74, 32, 30, 0, 0x0, 0 },
//  { TEXT_CreateIndirect, "Text", ID_TEXT_4, 239, 145, 32, 32, 0, 0x0, 0 },
//  { TEXT_CreateIndirect, "Text", ID_TEXT_6, 217, 152, 32, 20, 0, 0x0, 0 },
  
//   { BUTTON_CreateIndirect, "Button", ID_BUTTON_18, 3, 226, 77, 45, 0, 0x0, 0 },
  { BUTTON_CreateIndirect, "Button", ID_BUTTON_19, 83, 226, 77, 45, 0, 0x0, 0 },
  { BUTTON_CreateIndirect, "Button", ID_BUTTON_20, 163, 226, 77, 45, 0, 0x0, 0 },
  { BUTTON_CreateIndirect, "Button", ID_BUTTON_21, 243, 226, 77, 45, 0, 0x0, 0 },
  { BUTTON_CreateIndirect, "Button", ID_BUTTON_22, 323, 226, 77, 45, 0, 0x0, 0 },
  { BUTTON_CreateIndirect, "Button", ID_BUTTON_23, 403, 226, 77, 45, 0, 0x0, 0 },
  { GRAPH_CreateIndirect,  "Graph",  ID_GRAPH_0,   100, 31, 350, 193, 0, 0x0, 0 },
  { TEXT_CreateIndirect,   "Text",   ID_TEXT_128, 300, 2, 80, 20, 0, 0x0, 0 },
  // USER START (Optionally insert additional widgets)
  // USER END
};

/*********************************************************************
*
*       Static code
*
**********************************************************************
*/

// USER START (Optionally insert additional static code)
// USER END

/*********************************************************************
*
*       _cbDialog
*/
static void _cbDialog(WM_MESSAGE * pMsg) {
  WM_HWIN hItem;
  // USER START (Optionally insert additional variables)
  // USER END
    char buf[5];
    
  switch (pMsg->MsgId) {
		
	case WM_PAINT:
	GUI_SetColor(GUI_WHITE);//设置画笔颜色
	GUI_FillRect(0,25,480,26);//画一条实线
    GUI_SetColor(GUI_GREEN);
    GUI_FillRect(50,75,75,76);
    GUI_SetColor(GUI_RED);
    GUI_FillRect(50,120,75,121);
    GUI_SetColor(GUI_ORANGE);
	GUI_SetFont(&GUI_Fontsymbol);
	GUI_UC_SetEncodeUTF8();
	GUI_SetTextMode(GUI_TM_TRANS);//设置文本模式为底色透明
	GUI_DispStringAt("Jinko", 5, 1);//SET
	GUI_SetColor(GUI_WHITE);
	GUI_SetFont(&GUI_FontHZ20S);
	GUI_UC_SetEncodeUTF8();
	GUI_SetTextMode(GUI_TM_TRANS);//设置文本模式为底色透明
	GUI_DispStringAt("娴嬮噺鏄剧ず", 130, 3);
    GUI_SetFont(&GUI_Font24_1);
    GUI_DispStringAt("V(V)", 5,65);
    GUI_DispStringAt("I(A)", 5,110);
    
    GUI_SetColor(GUI_GREEN);
    GUI_SetFont(&GUI_Fontunit);
    GUI_DispStringAt("掳",342, 2);
    GUI_SetFont(&GUI_Font24_1);
    GUI_DispStringAt("C",350, 2);
    DrawLock();
    
	break;
	case WM_TIMER://定时模块消息
	if(WM_GetTimerId(pMsg->Data.v) == ID_TimerTime3)
	{
        lockstat2 = lockstat1;
        lockstat1 = lock; 
        if(lockstat1 != lockstat2)
        {
            WM_InvalidateWindow(hWinG);
        }
        DrawGraph();
        hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_128);       
        sprintf(buf,"%.1f",temp);
        TEXT_SetText(hItem,buf);
		WM_RestartTimer(pMsg->Data.v, 200);//复位定时器数值越大刷新时间越短
	}
	break;
    
		
    case WM_INIT_DIALOG:
    //
    // Initialization of 'R'
	
    //
        hItem = pMsg->hWin;
        WINDOW_SetBkColor(hItem, GUI_BLACK);
		WM_CreateTimer(hItem,ID_TimerTime3,1000,0);//创建本窗口定时器
    //

    //    
// 		hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_18);
// 		BUTTON_SetTextColor(hItem,0,GUI_BLACK);//设置字体颜色为黑色
// 		BUTTON_SetFont      (hItem,    &GUI_FontHZ16);//设定按钮文本字体
// 		GUI_UC_SetEncodeUTF8();
// 		BUTTON_SetText(hItem,"鍏呮斁鐢�");
	
	
		hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_19);
//		BUTTON_SetTextColor(hItem,0,GUI_BLACK);//设置字体颜色为黑色
		BUTTON_SetFont      (hItem,    &GUI_FontHZ16);//设定按钮文本字体
		GUI_UC_SetEncodeUTF8();
		BUTTON_SetText(hItem,"鍐呴樆娴嬭瘯");
	
		hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_20);
//		BUTTON_SetTextColor(hItem,0,GUI_BLACK);//设置字体颜色为黑色
		BUTTON_SetFont      (hItem,    &GUI_FontHZ16);//设定按钮文本字体
		GUI_UC_SetEncodeUTF8();
		BUTTON_SetText(hItem,"绋嬫帶璐熻浇");
		
		hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_21);
//		BUTTON_SetTextColor(hItem,0,GUI_BLACK);//设置字体颜色为黑色
		BUTTON_SetFont      (hItem,    &GUI_FontHZ16);//设定按钮文本字体
		GUI_UC_SetEncodeUTF8();
		BUTTON_SetText(hItem,"绋嬫帶鐢垫簮");
		
		hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_22);        
//		BUTTON_SetTextColor(hItem,0,GUI_BLACK);//设置字体颜色为黑色
		BUTTON_SetFont      (hItem,    &GUI_FontHZ16);//设定按钮文本字体
		GUI_UC_SetEncodeUTF8();
		BUTTON_SetText(hItem,"鍏呮斁鐢�");
		
		hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_23);
        BUTTON_SetPressed(hItem,1);
//		BUTTON_SetTextColor(hItem,0,GUI_BLACK);//设置字体颜色为黑色
		BUTTON_SetFont      (hItem,&GUI_FontHZ16);//设定按钮文本字体
		GUI_UC_SetEncodeUTF8();
		BUTTON_SetText(hItem,"鏇茬嚎");
        
        hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_128);
        TEXT_SetTextColor(hItem, GUI_GREEN);//设置字体颜色
        TEXT_SetFont(hItem,&GUI_Font24_1);//设定文本字体       
        sprintf(buf,"%.1f",temp);
        TEXT_SetText(hItem,buf);
        
        hItem = WM_GetDialogItem(pMsg->hWin, ID_GRAPH_0);
        GRAPH_SetBorder(hItem, 20, 5, 20, 15);
//        GRAPH_SetVSizeY(hItem,200);
        hScaleV =GRAPH_SCALE_Create(18, GUI_TA_RIGHT, GRAPH_SCALE_CF_VERTICAL, 20);
        GRAPH_AttachScale(hItem,hScaleV);
        hScaleH =GRAPH_SCALE_Create(185, GUI_TA_HCENTER, GRAPH_SCALE_CF_HORIZONTAL, 50);
        GRAPH_AttachScale(hItem,hScaleH);
        GRAPH_SCALE_SetFactor(hScaleV,0.1);
        VData = GRAPH_DATA_YT_Create(GUI_GREEN, 500, 0, 0);//
        IData = GRAPH_DATA_YT_Create(GUI_RED, 500, 0, 0);//
        GRAPH_AttachData(hItem,VData);
        GRAPH_AttachData(hItem,IData);
        GRAPH_SetGridDistY(hItem, 20);
        GRAPH_SetGridFixedX(hItem,25);
        GRAPH_SetGridVis(hItem, 1);
        
        		
        
		
    // USER START (Optionally insert additional code for further widget initialization)
    // USER END
        break;
  // USER START (Optionally insert additional message handling)
  // USER END
    default:
        WM_DefaultProc(pMsg);
        break;
  }
}

/*********************************************************************
*
*       Public code
*
**********************************************************************
*/
/*********************************************************************
*
*       CreateR
*/
WM_HWIN CreateG(void);
WM_HWIN CreateG(void) {
  page_sw = face_graph;
//  double_sw = face_graph;
  track = face_graph;
  pass = 0;
  TM1650_SET_LED(0x68,0x70);
  GPIO_ResetBits(GPIOD,GPIO_Pin_12);//灭灯
  hWinG = GUI_CreateDialogBox(_aDialogCreate3, GUI_COUNTOF(_aDialogCreate3), _cbDialog, WM_HBKWIN, 0, 0);
  return hWinG;
}

WM_HWIN ResetG(void) {
  hWinG = GUI_CreateDialogBox(_aDialogCreate3, GUI_COUNTOF(_aDialogCreate3), _cbDialog, WM_HBKWIN, 0, 0);
  return hWinG;
}
/*********************************************************************
*
*       DrawGraph
*/

WM_HWIN DrawGraph(void){
    if(mode_sw == mode_load)
    {
        GRAPH_DATA_YT_AddValue(VData,DISS_Voltage*10);
        GRAPH_DATA_YT_AddValue(IData,DISS_Current*10);
    }else if(mode_sw == mode_pow){
        GRAPH_DATA_YT_AddValue(VData,DISS_POW_Voltage*10);
        GRAPH_DATA_YT_AddValue(IData,DISS_POW_Current*10);
    }
    
//    GUI_Delay(50);
}
// USER START (Optionally insert additional public code)
// USER END

/*************************** End of file ****************************/
