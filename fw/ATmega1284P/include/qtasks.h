/*This file is part of the QuarkTS distribution.*/
#ifndef QTASKS_H
    #define QTASKS_H

    #include "qtypes.h"
    #include "qstimers.h"
    #include "qlists.h"
    
    #if (Q_QUEUES == 1)
        #include "qqueues.h"
    #endif

    #if ( Q_FSM == 1 )
        #include "qfsm.h"
    #endif  

    #if ( Q_ATCLI == 1 )
        #include "qatcli.h"
    #endif

    #ifdef __cplusplus
    extern "C" {
    #endif

    typedef enum {  qTriggerNULL, 
                    byTimeElapsed, 
                    byNotificationQueued, 
                    byNotificationSimple, 
                    byQueueReceiver, 
                    byQueueFull, 
                    byQueueCount, 
                    byQueueEmpty, 
                    byEventFlags,
                    bySchedulingRelease, 
                    byNoReadyTasks
                }qTrigger_t;

    #define qTrigger_NotificationSimple     ( byNotificationSimple )
    #define qTrigger_NotificationQueued     ( byNotificationQueued )
    #define qTrigger_TimeElapsed            ( byTimeElapsed )
    #define qTrigger_QueueReceiver          ( byQueueReceiver )
    #define qTrigger_QueueFull              ( byQueueFull )
    #define qTrigger_QueueCount             ( byQueueCount )
    #define qTrigger_QueueEmpty             ( byQueueEmpty )
    #define qTrigger_byEventFlags           ( byEventFlags )
    #define qTrigger_SchedulingRelease      ( bySchedulingRelease )
    #define qTrigger_NoReadyTasks           ( byNoReadyTasks )

    typedef qUINT32_t qNotifier_t;       
    typedef qUINT32_t qTask_Flag_t; 
    #define QMAX_NOTIFICATION_VALUE         ( 0xFFFFFFFFuL )

    typedef enum{
        qUndefinedGlobalState,
        qReady,
        qWaiting,
        qSuspended,
        qRunning
    }qTask_GlobalState_t;

    #if ( Q_TASK_EVENT_FLAGS == 1 )
        /*The task Bit-Flag definitions*/
        #define QEVENTFLAG_01               ( 0x00001000uL )
        #define QEVENTFLAG_02               ( 0x00002000uL )
        #define QEVENTFLAG_03               ( 0x00004000uL )
        #define QEVENTFLAG_04               ( 0x00008000uL )
        #define QEVENTFLAG_05               ( 0x00010000uL )
        #define QEVENTFLAG_06               ( 0x00020000uL )
        #define QEVENTFLAG_07               ( 0x00040000uL )
        #define QEVENTFLAG_08               ( 0x00080000uL )
        #define QEVENTFLAG_09               ( 0x00100000uL )
        #define QEVENTFLAG_10               ( 0x00200000uL )
        #define QEVENTFLAG_11               ( 0x00400000uL )
        #define QEVENTFLAG_12               ( 0x00800000uL )
        #define QEVENTFLAG_13               ( 0x01000000uL )
        #define QEVENTFLAG_14               ( 0x02000000uL )
        #define QEVENTFLAG_15               ( 0x04000000uL )
        #define QEVENTFLAG_16               ( 0x08000000uL )
        #define QEVENTFLAG_17               ( 0x10000000uL )
        #define QEVENTFLAG_18               ( 0x20000000uL )
        #define QEVENTFLAG_19               ( 0x40000000uL )
        #define QEVENTFLAG_20               ( 0x80000000uL )

        #define QEVENTFLAG_CLEAR            ( qFalse )
        #define QEVENTFLAG_SET              ( qTrue )
    #endif

    typedef struct{
        /* TaskData (Storage-Pointer):
        Task arguments defined at the time of its creation.
        */
        void *TaskData;
        /* EventData:
        Associated data of the event. Specific data will reside here according
        to the event source. This field will only have a NULL value when the 
        trigger is <byTimeElapsed> or <byPriority>.
         */
        void *EventData;
        /* Trigger:
        This flag indicates the event source that triggers the task execution.
        
         
        This flag can only have the following values:        
        
        - byTimeElapsed : When the time specified for the task elapsed.
                
        - byNotificationQueued: When there is a queued notification in the FIFO
                                priority queue. For this trigger, the dispacher 
                                performs a dequeue operation automatically. A 
                                pointer to the extracted event data will be available 
                                in the <EventData> field.
        
        - byNotificationSimple: When the execution chain does, according to a 
                        requirement of asynchronous notification event prompted 
                        by qSendEvent. A pointer to the dequeued data will be 
                        available in the <EventData> field.
        
        - byQueueReceiver: When there are elements available in the attached qQueue,
                        the scheduler make a data dequeue (auto-receive) from the
                        front. A pointer to the received data will be 
                        available in the <EventData> field.
        
        - byQueueFull: When the  attached qQueue is full. A pointer to the 
                         queue will be available in the <EventData> field.
         
        - byQueueCount: When the element-count of the  attached qQueue reaches
                         the specified value. A pointer to the queue will 
                         be available in the <EventData> field.
        
        - byQueueEmpty: When the  attached qQueue is empty.  A pointer to the 
                        queue will be available in the <EventData> field.
        
        - byNoReadyTasks: Only when the Idle Task is triggered.
        */
        qTrigger_t Trigger;
        /* FirstCall:
        This flag indicates that a task is running for the first time. 
        This flag can be used for data initialization purposes.
        */
        qBool_t FirstCall;
        /* FirstIteration:
        Indicates whether current pass is the first iteration of the task. 
        This flag will be only set when time-elapsed events occurs and the
        Iteration counter has been parameterized. Asynchronous events never change
        the task iteration counter, consequently doesn't have effect in this flag 
        */
        qBool_t FirstIteration;
        /* LastIteration:
        Indicates whether current pass is the last iteration of the task. 
        This flag will be only set when time-elapsed events occurs and the
        Iteration counter has been parameterized. Asynchronous events never change
        the task iteration counter, consequently doesn't have effect in this flag 
        */
        qBool_t LastIteration;
        /* StartDelay:
        The number of epochs between current system time and point in time when the 
        task was marked as Ready.
        Can be used to keep track when current task's execution took place relative 
        to when it was scheduled
        A value of 0 (zero) indicates that task started right on time per schedule.
        This parameter will be only available on timed tasks. when Trigger == byTimeElapsed
        */
        qClock_t StartDelay;
    }_qEvent_t_/*, *const qEvent_t*/;  
    typedef const _qEvent_t_ *qEvent_t;

    typedef void (*qTaskFcn_t)( qEvent_t arg );  

    /* Please don't access any members of this structure directly */
    typedef struct{ /*Task node definition*/
        struct _qTask_Private_s{    /*Task control block - TCB*/
            qNode_MinimalFields;
            void *TaskData, *AsyncData;             /*< The task storage pointers. */
            qTaskFcn_t Callback;                    /*< The callback function representing the task activities. */
            #if ( Q_FSM == 1 )
                qSM_t *StateMachine;                /*< The pointer to the attached state-machine. */
            #endif
            #if ( Q_QUEUES == 1 )
                qQueue_t *Queue;                    /*< The pointer to the attached queue. */
                qUINT32_t QueueCount;               /*< The item-count threshold */
            #endif
            qSTimer_t timer;                        /*< To handle the task timming*/
            #if ( Q_TASK_COUNT_CYCLES == 1 )
                qCycles_t Cycles;                   /*< The current number of executions performed by the task. */
            #endif
            #if ( Q_PRESERVE_TASK_ENTRY_ORDER == 1 )
                size_t Entry;                       /*< To allow the OS maintain the task entry order. */
            #endif
            qIteration_t Iterations;                /*< Hold the number of iterations. */
            volatile qNotifier_t Notification;      /*< The notification value. */          
            volatile qTask_Flag_t Flags;            /*< Task flags (core and eventflags)*/
            qTrigger_t Trigger;                     /*< The event source that put the task in a qReady state. */
            qPriority_t Priority;                   /*< The task priority. */
        }qPrivate;
    }qTask_t;

    #if ( Q_QUEUES == 1 )
        typedef enum {
            qQueueMode_Receiver  = 4,
            qQueueMode_Full = 8,
            qQueueMode_Count = 16,
            qQueueMode_Empty = 32,
        }qQueueLinkMode_t;
    #endif

    typedef enum{qTask_NotifyNULL, qTask_NotifySimple, qTask_NotifyQueued}qTask_NotifyMode_t;
    
    qBool_t qTask_Notification_Send( qTask_t * const Task, void* eventdata );
    qBool_t qTask_Notification_Queue( qTask_t * const Task, void* eventdata );
    qBool_t qTask_HasPendingNotifications( const qTask_t * const Task  );
    qState_t qTask_Get_State( const qTask_t * const Task);
    #if ( Q_TASK_COUNT_CYCLES == 1 )
        qCycles_t qTask_Get_Cycles( const qTask_t * const Task );   
    #endif
    qTask_GlobalState_t qTask_Get_GlobalState( const qTask_t * const Task );
    void qTask_Set_Time( qTask_t * const Task, const qTime_t Value );
    void qTask_Set_Iterations( qTask_t * const Task, const qIteration_t Value );
    void qTask_Set_Priority( qTask_t * const Task, const qPriority_t Value );
    void qTask_Set_Callback( qTask_t * const Task, const qTaskFcn_t CallbackFcn );
    void qTask_Set_State( qTask_t * const Task, const qState_t State );
    void qTask_Set_Data( qTask_t * const Task, void* arg );
    void qTask_ClearTimeElapsed( qTask_t * const Task );
    qTask_t* qTask_Self( void );

    /*void qTask_Suspend( qTask_t * const Task )

    Put the task into a disabled state.    

    Parameters:

        - Task : A pointer to the task node.
    */
    #define qTask_Suspend( pTask_ )     qTask_Set_State( (pTask_), qDisabled )
    #define qTask_Disable( pTask_ )     qTask_Set_State( (pTask_), qDisabled )
    /*void qTask_Resume( qTask_t * const Task )

    Put the task into a enabled state.    

    Parameters:

    - Task : A pointer to the task node.
    */
    #define qTask_Resume( pTask_ )      qTask_Set_State( (pTask_), qEnabled )
    #define qTask_Enable( pTask_ )      qTask_Set_State( (pTask_), qEnabled )
 
    /*void qTask_ASleep( qTask_t * const Task )

    Put the task into a qSLEEP state. The task can't be triggered
    by the lower precedence events.    
 
    Note: Only the higher precedence events (Queued Notifications) can
          wake up the task.

    Parameters:

    - Task : A pointer to the task node.
    */
    #define qTask_ASleep( pTask_ )      qTask_Set_State( (pTask_), qAsleep )
    /*void qTask_Awake( qTask_t * const Task )

    Put the task into a normal operation state. Here the task
    will be able to catch any kind of events.
 
    Parameters:

    - Task : A pointer to the task node.
    */    
    #define qTask_Awake( pTask_ )       qTask_Set_State( (pTask_), qAwake )
    /*qBool_t qTask_IsEnabled( const qTask_t * const Task )

    Retrieve the enabled/disabled state

    Parameters:

        Pointer to the task node.

    Return value:

        True if the task in on Enabled state, otherwise returns false.
    */  
    #define qTask_IsEnabled( pTask_ )   ( qEnabled == qTask_Get_State( (pTask_) ) )

    #if ( Q_QUEUES == 1 )
        qBool_t qTask_Attach_Queue( qTask_t * const Task, qQueue_t * const Queue, const qQueueLinkMode_t Mode, const qUINT16_t arg );
    #endif 

    #if ( Q_FSM == 1 ) 
        qBool_t qTask_Attach_StateMachine( qTask_t * const Task, qSM_t * const StateMachine );
    #endif

    #if ( Q_TASK_EVENT_FLAGS == 1 )
        void qTask_EventFlags_Modify( qTask_t * const Task, qTask_Flag_t flags, qBool_t action );
        qTask_Flag_t qTask_EventFlags_Read( const qTask_t * const Task ); 
        qBool_t qTask_EventFlags_Check( qTask_t * const Task, qTask_Flag_t FlagsToCheck, qBool_t ClearOnExit, qBool_t CheckForAll );
    #endif
    
    #ifdef __cplusplus
    }
    #endif

#endif /* QTASKS_H */
