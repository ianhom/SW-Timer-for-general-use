
/******************************************************************************
* 文件名称：SoftTimer.h
* 内容摘要：软件定时器模块头文件
* 其他说明：首先运行TimersInit函数，需向该函数提供"1ms的系统时钟"和"最大系统ms数"，
*          然后在各自的应用模块中调用CreatTimer创建定时器，该函数返回的地址为该定
*          时器的地址，可用与重启或删除定时器结点，请妥善保管。请在主循环中执行
*          ProcessTimer函数以更新定时器时间。
* 当前版本：V1.00
* 作 者：David Han, Ian
* 完成日期：2015年2月20日
******************************************************************************/
#ifndef __SOFTTIMER_H_
#define __SOFTTIMER_H_    

#ifdef __cplusplus
extern "C" {
#endif

#define SW_ERROR               (-1) /* 操作失败返回值 */
#define SW_OK                   (0) /* 操作成功返回值 */

typedef int (*TMRCALLBACK)(void *pArg);
typedef uint32 (*TMRSOURCE)(void);

/****************************************************
* 结构名：TIMER
* 描述： 软件定时器数据结构
* 参数：
     uint8        periodic;        SINGLE       单次触发
                                   PERIOIC      周期触发
     uint32       start;           0~0xFFFFFFFF 计时器起始时间
     uint32       now;             0~0xFFFFFFFF 计时器当前时间
     uint32       elapse;          0~0xFFFFFFFF 计时器已过时间
     uint32       timeout;         0~0xFFFFFFFF 计时器计时时间
     TMRCALLBACK  pfTimerCallback;              计时结束后执行的回调函数
     void        *pArg;                         回调函数的参数
* 作者： David Han, Ian
* 日期: 2015-2-20
****************************************************/
typedef struct _TIMER
{
    uint8       periodic;        /* 单次触发/周期触发 */
    uint32      start;           /* 计时器起始时间 */
    uint32      now;             /* 计时器当前时间 */
    uint32      elapse;          /* 计时器已过时间 */
    uint32      timeout;         /* 计时器计时时间 */
    TMRCALLBACK pfTimerCallback; /* 计时结束后执行的回调函数 */
    void       *pArg;            /* 回调函数的参数 */
} TIMER;


/****************************************************
* 结构名：TIMER_TABLE
* 描述： 定时器列表(链表)
* 参数：
          TIMER                data; 本计时器结点数据
          struct _TIMER_TABLE* next; 下一个定时器结点地址
* 作者： David Han, Ian
* 日期: 2015-2-20
****************************************************/
typedef struct _TIMER_TABLE
{
    TIMER                data; /* 本计时器结点数据 */
    struct _TIMER_TABLE* next; /* 下一个定时器结点地址 */
}TIMER_TABLE;

/*************************************************************************
* 函数名称：int TimersInit(TMRSOURCE pfTimer)
* 功能说明：初始化软件定时器模块
* 输入参数：TMRSOURCE pfTimer    系统1ms时钟函数
* 输出参数：无
* 返 回 值：SW_ERROR: 操作失败
*           SW_OK 操作成功
* 其它说明：无
**************************************************************************/
int TimersInit(TMRSOURCE pfTimer);


/*************************************************************************
* 函数名称：TIMER_TABLE* CreatTimer(uint32 dwTimeout, uint8 ucPeriodic, TMRCALLBACK pfTimerCallback, void *pArg)
* 功能说明：创建并启动软件定时器
* 输入参数：uint32         dwTimeout  0~0xFFFFFFFF 定时时间
           uint8          ucPeriodic SINGLE      单次触发
                                     PERIODIC    周期触发
           TMRCALLBACK    pfTimerCallback        定时结束时回调函数
            void         *pArg                   回调函数参数
            
* 输出参数：无
* 返 回 值：操作失败 : NULL
           操作成功 : 定时器模块指针
* 其它说明：创建完定时器后返回定时器结点的地址，改地址用于重启或删除该定时器
**************************************************************************/
TIMER_TABLE* CreatTimer(uint32 dwTimeout, uint8 ucPeriodic, TMRCALLBACK pfTimerCallback, void *pArg);


/*************************************************************************
* 函数名称：int KillTimer(TIMER_TABLE* ptNode)
* 功能说明：删除定时器结点
* 输入参数：TIMER_TABLE* ptNode 定时器结点地址
* 输出参数：无
* 返 回 值：SW_ERROR: 操作失败
           SW_OK 操作成功
* 其它说明：无
**************************************************************************/
int KillTimer(TIMER_TABLE* ptNode);


/*************************************************************************
* 函数名称：int ResetTimer(TIMER_TABLE* ptNode)
* 功能说明：重启定时器结点
* 输入参数：TIMER_TABLE* ptNode 定时器结点地址
* 输出参数：无
* 返 回 值：SW_ERROR: 操作失败
           SW_OK 操作成功
* 其它说明：无
**************************************************************************/
int ResetTimer(TIMER_TABLE* ptNode);


/*************************************************************************
* 函数名称：int ProcessTimer(void)
* 功能说明：更新定时器结点
* 输入参数：无
* 输出参数：无
* 返 回 值：SW_ERROR: 操作失败
           SW_OK 操作成功
* 其它说明：无
**************************************************************************/
int ProcessTimer(void);


#ifdef __cplusplus
}
#endif

#endif

/* end of file */
