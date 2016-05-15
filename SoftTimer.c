

    /******************************************************************************
    * 文件名称：SoftTimer.c
    * 内容摘要：软件定时器模块
    * 其他说明：首先运行TimersInit函数，需向该函数提供"1ms的系统时钟"和"最大系统ms数"，
    *           然后在各自的应用模块中调用CreatTimer创建定时器，该函数返回的地址为该定
    *           时器的地址，可用与重启或删除定时器结点，请妥善保管。请在主循环中执行
    *           ProcessTimer函数以更新定时器时间。
    * 当前版本：V1.00
    * 作 者：  David Han, Ian
    * 完成日期：2015年2月20日
    ******************************************************************************/

    #include "common.h"
    #include "SoftTimer.h"

    static TIMER_TABLE* sg_ptTimeTableHead = NULL;             /* 链表表头        */
    static TMRSOURCE    sg_pfSysClk        = NULL;             /* 系统1ms时钟函数 */

    /*************************************************************************
    * 函数名称：int TimersInit(TMRSOURCE pfTimer)
    * 功能说明：初始化软件定时器模块
    * 输入参数：TMRSOURCE pfTimer  系统1ms时钟函数
    * 输出参数：无
    * 返 回 值：SW_ERROR: 操作失败
    *           SW_OK 操作成功
    * 其它说明：无
    **************************************************************************/
    int TimersInit(TMRSOURCE pfTimer)
    {
        if (NULL == pfTimer)
        {
            return SW_ERROR; /* 检查注册函数是否为空指针 */
        }
        
        sg_ptTimeTableHead = (TIMER_TABLE*)malloc(sizeof(TIMER_TABLE)); /* 申请头结点，该头节点为空节点，方便链表节点增加和删除 */
        if (NULL == sg_ptTimeTableHead)
        {
            return SW_ERROR; /* 检查是否申请成功 */
        }

        /* 申请成功后进行初始化 */
        sg_ptTimeTableHead->next = NULL;               /* 下个结点地址置空     */
        sg_pfSysClk              = (TMRSOURCE)pfTimer; /* 注册系统1ms时钟函数  */
        
        return SW_OK;
    }


    /*************************************************************************
    * 函数名称：TIMER_TABLE* CreatTimer(uint32 dwTimeout, uint8 ucPeriodic, TMRCALLBACK pfTimerCallback, void *pArg)
    * 功能说明：创建并启动软件定时器
    * 输入参数：uint32       dwTimeout  0~0xFFFFFFFF 定时时间
                uint8       ucPeriodic  SINGLE       单次触发
                                        PERIODIC     周期触发
                TMRCALLBACK pfTimerCallback          定时结束时回调函数
                void       *pArg                     回调函数参数
                
    * 输出参数：无
    * 返 回 值：操作失败 : NULL
                操作成功 : 定时器模块指针
    * 其它说明：创建完定时器后返回定时器结点的地址，改地址用于重启或删除该定时器
    **************************************************************************/
    TIMER_TABLE* CreatTimer(uint32 dwTimeout, uint8 ucPeriodic, TMRCALLBACK pfTimerCallback, void *pArg)
    {
        TIMER_TABLE* ptTimerNode;
        TIMER_TABLE* ptFind;
        if (NULL == sg_ptTimeTableHead)
        {
            return NULL; /* 检查链表头节点是否存在 */
        }

        /* 链表头结点已经存在 */
        ptTimerNode = (TIMER_TABLE*)malloc(sizeof(TIMER_TABLE)); /* 申请定时器结点 */
        if (NULL == ptTimerNode)
        {
            return NULL; /* 检查是否申请成功 */
        }

        /* 结点申请成功 */
        ptTimerNode->next                 = NULL;                    /* 下个结点地址置空 */
        ptTimerNode->data.periodic        = ucPeriodic;              /* 单次/周期触发 */
        ptTimerNode->data.start           = sg_pfSysClk();           /* 获取计时起始时间 */
        ptTimerNode->data.now             = ptTimerNode->data.start; /* 获取当前时间 */
        ptTimerNode->data.elapse          = 0;                       /* 已经过的时间 */
        ptTimerNode->data.timeout         = dwTimeout;               /* 定时时间 */
        ptTimerNode->data.pfTimerCallback = pfTimerCallback;         /* 注册定时结束回调函数 */
        ptTimerNode->data.pArg            = pArg;                    /* 回调函数参数 */

        /* 将新申请的定时器结点增加进入链表 */
        ptFind = sg_ptTimeTableHead; /* 先找链表头结点 */
        while(NULL != ptFind->next)  /* 如果当前结点不是末尾结点*/
        {
            ptFind = ptFind->next;   /* 将下一个结点的地址作为当前结点继续查找 */
        }
        /* 找到末尾结点 */
        ptFind->next= ptTimerNode;   /* 将新申请结点链接到末尾结点 */

        return ptTimerNode;          /* 操作成功，返回新申请结点地址(用于删除和重启计时) */
    }

    /*************************************************************************
    * 函数名称：int KillTimer(TIMER_TABLE* ptNode)
    * 功能说明：删除定时器结点
    * 输入参数：TIMER_TABLE* ptNode 定时器结点地址
    * 输出参数：无
    * 返 回 值：SW_ERROR: 操作失败
               SW_OK 操作成功
    * 其它说明：无
    **************************************************************************/
    int KillTimer(TIMER_TABLE* ptNode)
    {
        TIMER_TABLE* ptFind;

        if (NULL == ptNode)
        {
            return SW_ERROR; /* 检查定时器结点是否为空 */
        }

        /* 定时器结点不为空 */
        ptFind = sg_ptTimeTableHead; /* 先找到头结点 */
        while (ptFind) /* 如果不是末尾结点 */
        {
            if (ptFind->next == ptNode)      /* 检查下一结点是否为需删除结点 */
            {
                ptFind->next = ptNode->next; /* 重新链接上下结点 */
                free(ptNode);                /* 删除结点 */
                return SW_OK;                /* 操作成功，退出程序 */
            }
            ptFind = ptFind->next;           /* 继续查找下一结点 */
        }
        return SW_ERROR;                     /* 未找到，操作失败 */
    }


    /*************************************************************************
    * 函数名称：int ResetTimer(TIMER_TABLE* ptNode)
    * 功能说明：重启定时器结点
    * 输入参数：TIMER_TABLE* ptNode 定时器结点地址
    * 输出参数：无
    * 返 回 值：SW_ERROR: 操作失败
               SW_OK 操作成功
    * 其它说明：无
    **************************************************************************/
    int ResetTimer(TIMER_TABLE* ptNode)
    {
        if (NULL == ptNode)
        {
            return SW_ERROR;                /* 检查定时器结点是否为空 */
        }

        ptNode->data.start = sg_pfSysClk(); /* 更新定时器起始时间 */
        return SW_OK;                       /* 操作成功 */
    }


    /*************************************************************************
    * 函数名称：int ProcessTimer(void)
    * 功能说明：更新定时器结点
    * 输入参数：无
    * 输出参数：无
    * 返 回 值：SW_ERROR: 操作失败
                SW_OK 操作成功
    * 其它说明：无
    **************************************************************************/
    int ProcessTimer(void)
    {
        TIMER_TABLE* ptFind;
        TIMER_TABLE* ptNodeFree;
        if (NULL == sg_ptTimeTableHead)
        {
            return SW_OK; /* 没有定时器需要运行 */
        }
        ptFind = sg_ptTimeTableHead;          /* 找到第一个有效结点 */
        while(ptFind)                         /* 如果不是末尾结点 */
        {
            ptFind->data.now = sg_pfSysClk(); /* 更新时间 */

            /* 计算此刻时间与起始时间的时间差 */
            ptFind->data.elapse = ptFind->data.now - ptFind->data.start;
            
            if(ptFind->data.elapse >= ptFind->data.timeout)          /* 如果时差大于等于设定的计时时间 */
            {
                if(ptFind->data.pfTimerCallback)                     /* 且已经注册了合法的回调函数 */
                {
                    ptFind->data.pfTimerCallback(ptFind->data.pArg); /* 执行回调函数 */
                }    
                if(ptFind->data.periodic)
                {
                    ResetTimer(ptFind);                              /* 如果是周期性触发，重启定时器 */
                }
                else
                {                                                    /* 如果是单次触发，删除定时器 */
                    ptNodeFree = ptFind;
                    ptFind = ptFind->next;
                    KillTimer(ptNodeFree);
                    continue;
                }            
            }
            ptFind = ptFind->next;                                   /* 继续更新下一个定时器结点 */
        }
        return SW_OK;                                                /* 操作成功 */
    }

    /* end of file */

