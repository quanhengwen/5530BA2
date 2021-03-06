/******************************************************************/
/* ???TIM3 PWM                                                */
/* Ч?ú                                                        */
/* ??ú????200HZ ????ú60.9% ????ú30.9%?PWM      */
/* ??ún?                                                    */
/* j???úQQ:363116119                                        */
/******************************************************************/
#include "my_register.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_tim.h"
#include "stm32f4xx_rcc.h"
#include "stm32f4xx.h"
#include "tim6.h"
#include "MainTask.h"
#include "ssd1963.h"
/*****************************************************************/
/*****************************************************************/

extern struct bitDefine
{
    unsigned bit0: 1;
    unsigned bit1: 1;
    unsigned bit2: 1;
    unsigned bit3: 1;
    unsigned bit4: 1;
    unsigned bit5: 1;
    unsigned bit6: 1;
    unsigned bit7: 1;
} flagA, flagB,flagC,flagD,flagE,flagF,flagG;


vu16 battery_c;
float bc_raw;
float cbc_raw;
float c_sum;
extern vu8 pow_sw;
extern vu8 cdc_sw;
extern vu8 load_sw;
extern vu8 oct_sw; 
extern vu8 oc_test;
extern vu8 c_rec;
extern vu8 second ;
extern vu8 minute;
extern vu8 hour;
extern vu8 second1;
extern vu8 minute1;
extern vu8 hour1;
vu8 resetflag;
vu8 resdone;
float watch;
extern float crec1,crec2;
extern u8 g_mods_timeout;
extern struct MODS_T g_tModS;
u32 Tick_10ms=0;
u32 OldTick;
u8 usartocflag = 0;//涓婁綅鏈鸿繃娴佹爣蹇椾綅
u8 usartshortflag = 0;//涓婁綅鏈虹煭璺爣蹇椾綅
float shortv;
extern vu16 short_time;
extern vu8 rpow;
extern vu8 short_finish;
extern vu8 short_flag;
extern float v;
extern vu8 oc_mode;
extern vu8 test_start;
extern float static_pc;
extern float static_lv;
extern vu8 staticcdc;
extern vu8 step;
extern vu16 sendload;
//????? 3 ?????
//arrú?????c pscú??????
//???????????:Tout=((arr+1)*(psc+1))/Ft us.
//Ft=??????ê,?λ:Mhz
//?o???ˇ??? 3!
void TIM4_Int_Init(u16 arr,u16 psc)
{
    TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;
    
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4,ENABLE); //ù?? TIM3 ??
    TIM_TimeBaseInitStructure.TIM_Period = arr; //??????
    TIM_TimeBaseInitStructure.TIM_Prescaler=psc; //?????
    TIM_TimeBaseInitStructure.TIM_CounterMode=TIM_CounterMode_Up; //в?????
    TIM_TimeBaseInitStructure.TIM_ClockDivision=TIM_CKD_DIV1;
    TIM_TimeBaseInit(TIM4,&TIM_TimeBaseInitStructure);// ú?????? TIM3
    TIM_ITConfig(TIM4,TIM_IT_Update,ENABLE); //?????? 3 ?т??
    NVIC_InitStructure.NVIC_IRQChannel=TIM4_IRQn; //??? 3 ??
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0x01; //????? 1
    NVIC_InitStructure.NVIC_IRQChannelSubPriority=0x03; //Ь???? 3
    NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;
    NVIC_Init(&NVIC_InitStructure);// ü??? NVIC
    TIM_Cmd(TIM4,ENABLE); //Y????? 3
}
//??? 3 ??????
void TIM4_IRQHandler(void)
{
    static vu16 resetcount;
    static vu8 read1963;
    static vu16 scancount;
    static vu16 uocount;
    static vu16 powcount;
    static vu16 powflag;
    static vu16 finishflag;
    static float crec1,crec2;
    u8 crec[6];
    u8 *csend;
    static u8 *sendbuf;
    u8 sendlen;
    static u16 recrc;
    static u16 scrc;
	static vu8 staticloadflag,staticpowflag;
    u8 i;
//     static float crec1,crec2;
    
    if(TIM_GetITStatus(TIM4,TIM_IT_Update)==SET) //????
    {
        TIM_ClearITPendingBit(TIM4,TIM_IT_Update); //清除中断标志位
        
        if(page_sw != face_starter)
        {
             if(resetflag == 1)
             {
                 if(resetcount == 3000)
                 {
//                     sLCD_GPIO_Config();
                     sLCD_Init();
//                     sLCD_WR_REG(0xf1);
                     GUI_Init();
                     if(page_sw == face_menu)
                     {
                         ResetPow();
                     }else if(page_sw == face_cdc){
                         ResetCDC();
                     }else if(page_sw == face_r){
                         ResetR();
                     }else if(page_sw == face_load){
                         ResetLoad();
                     }else if(page_sw == face_graph){
                         ResetG();
                     }else if(page_sw == face_set){
                         ResetSET();
                     }
//                     resdone = 1;
//                     resetflag = 0;
                     resetcount = 0;
                 }else{
                     resetcount++;
                 }                
             }
         }
         if(page_sw == face_r)
		 {
			 if(step == 1)
			 {
				if(powcount < 10000)
				{
					SET_Current_Laod = set_static_lc;
					flag_Load_CC = 1;
					GPIO_SetBits(GPIOC,GPIO_Pin_10);//CC
					GPIO_ResetBits(GPIOA,GPIO_Pin_15);//支負睾諛On
					static_lv = DISS_Voltage;
					powcount++;																		
				}else{
					powcount = 0;
					
					IO_OFF();
					step = 2;
				}
			 }else if(step == 2){
				if(powcount < 10000)
				{
					SET_Voltage = set_static_pv;
					SET_Current = set_static_pc;
					GPIO_SetBits(GPIOA,GPIO_Pin_15);//支負睾諛OFF
					GPIO_ResetBits(GPIOC,GPIO_Pin_13);//詹擢支源摔远輰支欠
					GPIO_SetBits(GPIOC,GPIO_Pin_1);//詹擢支源摔远
					powcount++;
					static_pc = DISS_POW_Current;
				}else{
					GPIO_ResetBits(GPIOC,GPIO_Pin_1);//关闭电源输出
					Delay_ms(500);
				    GPIO_SetBits(GPIOC,GPIO_Pin_13);//关闭电源输出继电器				
					powcount = 0;
					step = 3;
//					IO_OFF();		
					
				}
			 }else if(step == 3){
				if(powcount < 5000)
				{
					powcount++;
				}else{
					
					powcount = 0;	
					sendload = 200;				
					SET_Current_Laod = set_init_c;
					GPIO_ResetBits(GPIOA,GPIO_Pin_15);//鎵撳紑璐熻浇
					step = 4;
				}
			 }else if(step == 4){
				OC_CHECK();
				stepcount ++;
				if(stepcount == steptime*10)
				{
					OC_ADD();
					stepcount = 0;
				}
			 }else if(step == 5){
				if(powcount < 2000)
				{
					SET_Voltage = (int)v*100+200;
					SET_Current = 1000;
					GPIO_SetBits(GPIOA,GPIO_Pin_15);//支負睾諛OFF
					GPIO_ResetBits(GPIOC,GPIO_Pin_13);//詹擢支源摔远輰支欠
					GPIO_SetBits(GPIOC,GPIO_Pin_1);//詹擢支源摔远
					powcount++;
				}else{
					GPIO_ResetBits(GPIOC,GPIO_Pin_1);//关闭电源输出
					Delay_ms(500);
				    GPIO_SetBits(GPIOC,GPIO_Pin_13);//关闭电源输出继电器				
					powcount = 0;
					step = 6;
//					IO_OFF();
				}
			 }else if(step == 6){
				if(flag_Load_CC == 1)
				{
					SET_Current_Laod = (int)(oc_data*1000)+8000; 
					flag_Load_CC = 1;                              
					GPIO_SetBits(GPIOC,GPIO_Pin_10);//CC
					GPIO_ResetBits(GPIOA,GPIO_Pin_15);//支負睾諛On
				}else if(flag_Load_CC == 0){
					GPIO_ResetBits(GPIOC,GPIO_Pin_13);
					SET_Voltage_Laod = 0;
					GPIO_SetBits(GPIOC,GPIO_Pin_10);//CV
					flag_Load_CC = 0;
					GPIO_ResetBits(GPIOA,GPIO_Pin_15);//支負睾諛On
				}
				if(((v - DISS_Voltage) > v*0.6)/* || (DISS_Current < 0.05)*/)
				{
					IO_OFF();
					step = 7;
				}else{
					short_time++;                
				}
			 }else if(step == 7){
				 if(powcount < 4000)
				{
					SET_Voltage =(int)v*100+200;
					SET_Current = 1000;
					GPIO_ResetBits(GPIOC,GPIO_Pin_13);//詹擢支源摔远輰支欠
					GPIO_SetBits(GPIOC,GPIO_Pin_1);//詹擢支源摔远
					powcount++;
	//                 shortv = DISS_Voltage;
				}else{
					GPIO_ResetBits(GPIOC,GPIO_Pin_1);//关闭电源输出
					Delay_ms(500);
				    GPIO_SetBits(GPIOC,GPIO_Pin_13);//关闭电源输出继电�
					powcount = 0;
					step = 0;
//					IO_OFF();                
				}
			 }else if(step == 0)
			 {
				 powcount = 0;
				 IO_OFF();
			 }
		}
        
		
        if(usartocflag == 1)
        {

            if(flag_Load_CC == 1)
            {   
//				load_sw = load_on;
                GPIO_SetBits(GPIOC,GPIO_Pin_10);//CC
				flag_Load_CC = 1;
                GPIO_ResetBits(GPIOA,GPIO_Pin_15);//电子负载On
            }else if(flag_Load_CC == 0){
                SET_Voltage_Laod = 0;
                GPIO_ResetBits(GPIOC,GPIO_Pin_10);//CV
                flag_Load_CC = 0;
                GPIO_ResetBits(GPIOA,GPIO_Pin_15);//电子负载On
            }
            crec2 = crec1;
            crec1 = DISS_Current;
            if(crec1 < crec2 && crec2 > 0.3)
            {     
                oc_data = crec2;
                g_tModS.TxBuf[13] = (int)(oc_data*1000)>>8;
                g_tModS.TxBuf[14] = (int)(oc_data*1000);
                SET_Current_Laod = set_init_c;
                GPIO_SetBits(GPIOA,GPIO_Pin_15);//鍏抽棴璐熻浇 
//                MODS_SendWithCRC(g_tModS.TxBuf, g_tModS.TxCount);
                t_onoff = 0;
                usartocflag = 0;
                crec1 = 0;
                crec2 = 0;
                powflag = 1;
                
            }else{
                if(flag_Load_CC == 1)
                {
                    if(uocount == 20)
                    {
                        SET_Current_Laod = SET_Current_Laod + 10;
                        uocount = 0;
                    }else{
                        uocount++;
                    }
                }                    
            }
        }
        if(powflag == 1)
        {
            if(powcount < 1000)
            {
                SET_Voltage =3000;
                SET_Current = 1000;
                GPIO_SetBits(GPIOA,GPIO_Pin_15);//电子负载OFF
                GPIO_ResetBits(GPIOC,GPIO_Pin_13);//打开电源输出继电器
                GPIO_SetBits(GPIOC,GPIO_Pin_1);//打开电源输出
                powcount++;
//                 shortv = DISS_Voltage;
            }else{
                powcount = 0;
                powflag = 0;
                GPIO_ResetBits(GPIOC,GPIO_Pin_1);//关闭电源输出
                GPIO_SetBits(GPIOC,GPIO_Pin_13);//关闭电源输出继电器 
                usartshortflag = 1;
                
            }
        }
        if(usartshortflag == 1)
        {
            if(flag_Load_CC == 1)
            {
                SET_Current_Laod = (int)(oc_data*1000)+5000; 
                GPIO_SetBits(GPIOC,GPIO_Pin_10);//CC
                flag_Load_CC = 1;                              
                GPIO_SetBits(GPIOC,GPIO_Pin_10);//CC
                GPIO_ResetBits(GPIOA,GPIO_Pin_15);//电子负载On
            }else if(flag_Load_CC == 0){
                SET_Voltage_Laod = 0;
                GPIO_SetBits(GPIOC,GPIO_Pin_10);//CV
                flag_Load_CC = 0;
                GPIO_ResetBits(GPIOA,GPIO_Pin_15);//电子负载On
            }
            if(((shortv - DISS_Voltage) > shortv*0.6) || (DISS_Current < 0.05))
            {
                IO_OFF();
                usartshortflag = 0;               
                g_tModS.TxBuf[17] = (short_time/10)>>8;
                g_tModS.TxBuf[18] = (short_time/10);
                MODS_SendWithCRC(g_tModS.TxBuf, g_tModS.TxCount);
                finishflag=1;
                short_time = 0;
            }else{
                short_time++;                
            }
        }
        if(finishflag == 1)
        {
            if(powcount < 1000)
            {
                SET_Voltage =3000;
                SET_Current = 1000;
                GPIO_ResetBits(GPIOC,GPIO_Pin_13);//打开电源输出继电器
                GPIO_SetBits(GPIOC,GPIO_Pin_1);//打开电源输出
                powcount++;
//                 shortv = DISS_Voltage;
            }else{
                SET_Current_Laod = 1000;
                powcount = 0;
                finishflag = 0;
                IO_OFF();                
            }
        }
    }    
}

//void TIM6_Config(void)
//{
//	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
//	/* TIM3 ??? ---------------------------------------------------
//   TIM3 ????(TIM3CLK) ??? APB2 ?? (PCLK2)    
//    => TIM3CLK = PCLK2 = SystemCoreClock
//   TIM3CLK = SystemCoreClock, Prescaler = 0, TIM3 counter clock = SystemCoreClock
//   SystemCoreClock ?48 MHz */
//  /* TIM16 ???? */
//  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM6,ENABLE);
//	
//  /* Time ??????*/
//  TIM_TimeBaseStructure.TIM_Prescaler = 4800-1;//?????
//  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  /* Time ????????????*/
//  TIM_TimeBaseStructure.TIM_Period = 5000;
//  TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
////  TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;

//  TIM_TimeBaseInit(TIM6,&TIM_TimeBaseStructure);
//	TIM_ITConfig(TIM6,TIM_IT_Update,ENABLE);//??????т??
//	TIM_SetAutoreload(TIM6, 0xFF);//??PWM??ê
//	TIM6_NVIC_Config();
//  /* TIM3 ?????*/
//  TIM_Cmd(TIM6, ENABLE);
//}
///***********************************************************************/
//static void TIM6_NVIC_Config(void)
//{
//	NVIC_InitTypeDef NVIC_InitStructure; 
//	
//	/* Enable the USART1 Interrupt */
//	NVIC_InitStructure.NVIC_IRQChannel = TIM6_DAC_IRQn;	 
//	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x01;
//	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x03;
//	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
//	NVIC_Init(&NVIC_InitStructure);
//}
/*****************************************************************/
/*****************************************************************/
void TIM6_Config(void)
{
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	/* TIM3 的配置 ---------------------------------------------------
   TIM3 输入时钟(TIM3CLK) 设置为 APB2 时钟 (PCLK2)    
    => TIM3CLK = PCLK2 = SystemCoreClock
   TIM3CLK = SystemCoreClock, Prescaler = 0, TIM3 counter clock = SystemCoreClock
   SystemCoreClock 为48 MHz */
  /* TIM16 时钟使能 */
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM6,ENABLE);
	
  /* Time 定时基础设置*/
  TIM_TimeBaseStructure.TIM_Prescaler = 2;//时钟预分频
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  /* Time 定时设置为上升沿计算模式*/
  TIM_TimeBaseStructure.TIM_Period = 0;
  TIM_TimeBaseStructure.TIM_ClockDivision = 0;
  TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;

  TIM_TimeBaseInit(TIM6,&TIM_TimeBaseStructure);
	TIM_ITConfig(TIM6,TIM_IT_Update,ENABLE);//开启定时器更新中断
	TIM_SetAutoreload(TIM6, 0xFF);//设置PWM分辨率
	TIM6_NVIC_Config();
  /* TIM3 计算器使能*/
  TIM_Cmd(TIM6, ENABLE);
}
/***********************************************************************/
static void TIM6_NVIC_Config(void)
{
	NVIC_InitTypeDef NVIC_InitStructure; 
	
	/* Enable the USART1 Interrupt */
	NVIC_InitStructure.NVIC_IRQChannel = TIM6_DAC_IRQn;	 
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}
//????? 3 ?????
//arrú?????c pscú??????
//???????????:Tout=((arr+1)*(psc+1))/Ft us.
//Ft=??????ê,?λ:Mhz
//?o???ˇ??? 3!
void TIM3_Int_Init(u16 arr,u16 psc)
{
    TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;
    
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3,ENABLE); //ù?? TIM3 ??
    TIM_TimeBaseInitStructure.TIM_Period = arr; //??????
    TIM_TimeBaseInitStructure.TIM_Prescaler=psc; //?????
    TIM_TimeBaseInitStructure.TIM_CounterMode=TIM_CounterMode_Up; //в?????
    TIM_TimeBaseInitStructure.TIM_ClockDivision=TIM_CKD_DIV1;
    TIM_TimeBaseInit(TIM3,&TIM_TimeBaseInitStructure);// ú?????? TIM3
    TIM_ITConfig(TIM3,TIM_IT_Update,ENABLE); //?????? 3 ?т??
    NVIC_InitStructure.NVIC_IRQChannel=TIM3_IRQn; //??? 3 ??
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0x01; //????? 1
    NVIC_InitStructure.NVIC_IRQChannelSubPriority=0x03; //Ь???? 3
    NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;
    NVIC_Init(&NVIC_InitStructure);// ü??? NVIC
    TIM_Cmd(TIM3,ENABLE); //Y????? 3
}
//??? 3 ??????
void TIM3_IRQHandler(void)
{
    static vu8 calert = 0;
    static vu16 resetcount;
    static vu8 read1963;
    static vu16 scancount;
    static vu32 ctime,dctime;
    
    if(TIM_GetITStatus(TIM3,TIM_IT_Update)==SET) //????
    {
        TIM_ClearITPendingBit(TIM3,TIM_IT_Update); //??????λ
//         if(page_sw != face_starter)
//         {
//              if(resetflag == 1)
//              {
//                  if(resetcount == 1)
//                  {
//                      LCD_Initializtion();
//                      sLCD_WR_REG(0xf1);
//                      GUI_Init();
//                      if(page_sw == face_menu)
//                      {
//                          ResetPow();
//                      }else if(page_sw == face_cdc){
//                          ResetCDC();
//                      }else if(page_sw == face_r){
//                          ResetR();
//                      }else if(page_sw == face_load){
//                          ResetLoad();
//                      }else if(page_sw == face_graph){
//                          ResetG();
//                      }else if(page_sw == face_set){
//                          ResetSET();
//                      }
//                      resdone = 1;
// //                     resetflag = 0;
//                      resetcount = 0;
//                  }else{
//                      resetcount++;
//                  }                
//              }
//          }
        switch(page_sw)
        {
            case face_menu:
            {
                if(pow_sw == pow_on)
                {
                    bc_raw += DISS_POW_Current * 1000 * 1/3600;
                }else{
                    bc_raw = 0;
                }
            }break;
            case face_cdc:
            {
                if(mode_sw == mode_pow && cdc_sw == cdc_on)
                {
                    ctime++;
                    second = ctime%60;//绉�
                    minute = (ctime/60)%60;//鍒�
                    hour   = ctime/3600;//鏃�
                    cbc_raw += DISS_POW_Current * 1000 * 1/3600;
                    bc_raw = 0;
//                    bc_raw += DISS_POW_Current * 1000 * 1/3600;
                }else if(mode_sw == mode_load && cdc_sw == cdc_on){
                    dctime++;
                    second1 = dctime%60;//绉�
                    minute1 = (dctime/60)%60;//鍒�
                    hour1   = dctime/3600;//鏃�
                    bc_raw += DISS_Current * 1000 * 1/3600;
//                    c_sum += DISS_Current * 1000 * 1/3600;
                    cbc_raw = 0;
                }else if(cdc_sw == cdc_off){
                    bc_raw = 0;
                    cbc_raw = 0;
                    c_sum = 0;
                    ctime=0;
                    dctime=0;
                }
            }break;
            case face_load:
            {
                if(load_sw == load_on)
                {
                    if(alert_flag == 1)
                    {
                        calert ++;
                        if(calert == 3)
                        {
                            t_onoff = 0;
                            GPIO_SetBits(GPIOA,GPIO_Pin_15);//????OFF
                            mode_sw = 0;
                            load_sw = load_off;
                            calert = 0;                                
                        }
                    }
                    bc_raw += DISS_Current * 1000 * 1/3600;
                }else{
                    bc_raw = 0;
                }
            }break;
            case face_graph:
            {
                if(mode_sw == mode_pow)
                {
                    if(pow_sw == pow_on)
                    {
                        bc_raw += DISS_POW_Current * 1000 * 1/3600;
                    }else if(mode_sw == mode_pow && cdc_sw == cdc_on)
                    {
                        bc_raw += DISS_POW_Current * 1000 * 1/3600;
                    }
                    else{
                        bc_raw = 0;
                    }
                }               
                
                if(mode_sw == mode_load)
                {
                    if(load_sw == load_on)
                    {
                        bc_raw += DISS_Current * 1000 * 1/3600;
                    }else{
                        bc_raw = 0;
                    }
                }
            }break;
            case face_r:
            {
                if(oct_sw == oct_on)
                {
                    if(alert_flag == 1)
                    {
                        calert ++;
                        if(calert == 3)
                        {
                            ocstop = 1;
                            calert = 0;
                        }
                    }
                }
            }break;
        }
//         GPIO_ResetBits(GPIOD,GPIO_Pin_12);
//         TM1650_SET_LED(0x48,0x71);
//         TM1650_SET_LED(0x68,0xF2);//PASS?
    }    
    
}

void TIM5_Int_Init(u16 arr,u16 psc)
{
    TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;
    
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM5,ENABLE); //脵使艤 TIM3 时讚
    TIM_TimeBaseInitStructure.TIM_Period = arr; //財织讟装諛值
    TIM_TimeBaseInitStructure.TIM_Prescaler=psc; //吱时欠貣频
    TIM_TimeBaseInitStructure.TIM_CounterMode=TIM_CounterMode_Up; //胁蕪輪私模式
    TIM_TimeBaseInitStructure.TIM_ClockDivision=TIM_CKD_DIV1;
    TIM_TimeBaseInit(TIM5,&TIM_TimeBaseInitStructure);// 脷缘始郫吱时欠 TIM3
    TIM_ITConfig(TIM5,TIM_IT_Update,ENABLE); //脹諍循吱时欠 3 偌褌讗讖
    NVIC_InitStructure.NVIC_IRQChannel=TIM5_IRQn; //吱时欠 3 讗讖
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0x01; //葊占詤袌芏 1
    NVIC_InitStructure.NVIC_IRQChannelSubPriority=0x02; //鞋应詤袌芏 3
    NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;
    NVIC_Init(&NVIC_InitStructure);// 脺缘始郫 NVIC
    TIM_Cmd(TIM5,ENABLE); //脻使艤吱时欠 3
}


void TIM5_IRQHandler(void)
{
    
    if(TIM_GetITStatus(TIM5,TIM_IT_Update)==SET) //缨远讗讖
    {
        TIM_ClearITPendingBit(TIM5,TIM_IT_Update); //娓呴櫎涓柇鏍囧織浣?
        Tick_10ms ++;
        MODS_Poll();
    }
}


void MODS_Poll(void)
{
	uint16_t addr;
	static uint16_t crc1;
    static u32 testi;
	/* 瓒呰繃3.5涓瓧绗︽椂闂村悗鎵цMODH_RxTimeOut()鍑芥暟銆傚叏灞�鍙橀噺 g_rtu_timeout = 1; 閫氱煡涓荤▼搴忓紑濮嬭В鐮?*/
//	if (g_mods_timeout == 0)	
//	{
//		return;								/* 娌℃湁瓒呮椂锛岀户缁帴鏀躲�備笉瑕佹竻闆?g_tModS.RxCount */
//	}

    testi=g_tModS.RxCount;
    testi=g_tModS.RxCount;
    testi=g_tModS.RxCount;
	if(testi>7)				/* 接收到的数据小于4个字节就认为错误 */
	{
		testi=testi;
	}
	testi=g_tModS.RxCount;
    if(testi==8)				/* 接收到的数据小于4个字节就认为错误 */
	{
		testi=testi+1;
	}
	//判断通讯接收是否超时
	if(OldTick!=Tick_10ms)
  	{  
	  OldTick=Tick_10ms;
	   if(g_mods_timeout>0)
      { 
	    g_mods_timeout--;
      }
	  if(g_mods_timeout==0 && g_tModS.RxCount>0)   //有数但超时了
      { 
		// goto err_ret;
	
      }
      else if(g_mods_timeout==0 && g_tModS.RxCount==0) //没数超时了
         return;
      else //没超时了，继续收
         return;
	}
	else   //没有到10ms，不进入解析
		return;
	//g_mods_timeout = 0;	 					/* 清标志 */

	if (g_tModS.RxCount < 4)				/* 接收到的数据小于4个字节就认为错误 */
	{
		goto err_ret;
	}

	/* 计算CRC校验和 */
// 	crc1 = CRC16(g_tModS.RxBuf, g_tModS.RxCount);
// 	if (crc1 != 0)
// 	{
// 		goto err_ret;
// 	}

// 	/* 站地址 (1字节） */
// 	addr = g_tModS.RxBuf[0];				/* 第1字节 站号 */
// 	if (addr != SADDR485)		 			/* 判断主机发送的命令地址是否符合 */
// 	{
// 		goto err_ret;
// 	}

	/* 鍒嗘瀽搴旂敤灞傚崗璁?*/
    if(g_tModS.RxBuf[2] == 0xA5)
    {
        UART_Action();
    }else{
//        usartocflag = 1;
        RecHandle();
    }
							
	
err_ret:
#if 0										/* 姝ら儴鍒嗕负浜嗕覆鍙ｆ墦鍗扮粨鏋?瀹為檯杩愮敤涓彲涓嶈 */
	g_tPrint.Rxlen = g_tModS.RxCount;
	memcpy(g_tPrint.RxBuf, g_tModS.RxBuf, g_tModS.RxCount);
#endif
	
 	g_tModS.RxCount = 0;					/* 蹇呴』娓呴浂璁℃暟鍣紝鏂逛究涓嬫甯у悓姝?*/
}

