#include "qqueues.h"

#if (Q_QUEUES == 1)

static void qQueue_CopyDataToQueue( qQueue_t * const obj, const void *ItemToQueue, const qBool_t xPosition );
static void qQueue_MoveReader( qQueue_t * const obj );
static void qQueue_CopyDataFromQueue( qQueue_t * const obj, void * const pvBuffer );


qBool_t qQueue_IsReady( const qQueue_t * const obj ){
    qBool_t RetValue = qFalse;

    if( NULL != obj ){
        if( NULL != obj->qPrivate.head ){
            RetValue = qTrue;
        }
    }
    return RetValue;
}
/*============================================================================*/
/*qBool_t qQueue_Setup( qQueue_t * const obj, void* DataArea, size_t ItemSize, size_t ItemsCount )
 
Configures a Queue. Here, the RAM used to hold the queue data <DataArea>
is statically allocated at compile time by the application writer.
 
Parameters:

    - obj : a pointer to the Queue object.
    - DataArea :  data block or array of data.
    - ItemSize : The size, in bytes, of one single item in the queue.
    - ItemsCount : The maximum number of items the queue can hold.

Return value:

    qTrue on success, otherwise returns qFalse.

 */
qBool_t qQueue_Setup( qQueue_t * const obj, void* DataArea, size_t ItemSize, size_t ItemsCount ){
    qBool_t RetValue = qFalse;

    if( ( NULL != obj ) && ( NULL != DataArea ) && ( ItemSize > 0u ) && ( ItemsCount > 0u) ){
        obj->qPrivate.ItemsCount = ItemsCount;   /* Initialise the queue members*/
        obj->qPrivate.ItemSize = ItemSize;
        /*cstat -MISRAC2012-Rule-11.5 -CERT-EXP36-C_b*/
        /* Set the head to the start of the storage area */
        obj->qPrivate.head = DataArea; /* MISRAC2012-Rule-11.5,CERT-EXP36-C_b deviation allowed */
        /*cstat +MISRAC2012-Rule-11.5 +CERT-EXP36-C_b*/
        qQueue_Reset( obj );
        RetValue = qTrue;
    }
    return RetValue;
}
/*============================================================================*/
/*void qQueue_Reset( qQueue_t * const obj )
 
Resets a queue to its original empty state.
 
Parameters:

    - obj : a pointer to the Queue object
*/
void qQueue_Reset( qQueue_t * const obj ){
    if ( NULL != obj ){
        qCritical_Enter();
        obj->qPrivate.tail = obj->qPrivate.head + ( obj->qPrivate.ItemsCount * obj->qPrivate.ItemSize ); 
        obj->qPrivate.ItemsWaiting = 0u;
        obj->qPrivate.writer = obj->qPrivate.head;
        obj->qPrivate.reader = obj->qPrivate.head + ( ( obj->qPrivate.ItemsCount - 1u ) * obj->qPrivate.ItemSize );
        qCritical_Exit();
    }
}
/*============================================================================*/
/*qBool_t qQueue_IsEmpty( const qQueue_t * const obj )
 
Returns the empty status of the Queue
 
Parameters:

    - obj : a pointer to the Queue object
  
Return value:

    qTrue if the Queue is empty, qFalse if it is not.
 */
qBool_t qQueue_IsEmpty( const qQueue_t * const obj ){
    qBool_t RetValue = qTrue;

    if( NULL != obj ){
        if( 0u == obj->qPrivate.ItemsWaiting ){
            RetValue = qTrue;
        }
        else{
            RetValue = qFalse;
        }  
    }
    return RetValue;   
}
/*============================================================================*/
/*size_t qQueue_Count( const qQueue_t * const obj )
 
Returns the number of items in the Queue
 
Parameters:

    - obj : a pointer to the Queue object
  
Return value:

    The number of elements in the queue
 */
size_t qQueue_Count( const qQueue_t * const obj ){
    size_t RetValue = 0u;

    if ( NULL != obj ){
        RetValue = obj->qPrivate.ItemsWaiting;
    } 
    return RetValue;
}
/*============================================================================*/
/*size_t qQueue_ItemsAvailable( const qQueue_t * const obj )
 
Returns the number of available slots to hold items inside the Queue
 
Parameters:

    - obj : a pointer to the Queue object
  
Return value:

    The number of available slots in the queue
 */
size_t qQueue_ItemsAvailable( const qQueue_t * const obj ){
    size_t RetValue = 0u;

    if ( NULL != obj ){
        RetValue = obj->qPrivate.ItemsCount - obj->qPrivate.ItemsWaiting;
    } 
    return RetValue;
}
/*============================================================================*/
/*qBool_t qQueue_IsFull( const qQueue_t * const obj )
 
Returns the full status of the Queue
 
Parameters:

    - obj : a pointer to the Queue object
  
Return value:

    qTrue if the Queue is full, qFalse if it is not.
 */
/*============================================================================*/
qBool_t qQueue_IsFull( const qQueue_t * const obj ){
    qBool_t RetValue = qFalse;

    if( NULL != obj ){
        if( obj->qPrivate.ItemsWaiting == obj->qPrivate.ItemsCount ){
            RetValue = qTrue;
        }
    }
    return RetValue;
}
/*============================================================================*/
/*void* qQueue_Peek( const qQueue_t * const obj )
 
Looks at the data from the front of the Queue without removing it. 
 
Parameters:

    - obj : a pointer to the Queue object
  
Return value:

    Pointer to the data, or NULL if there is nothing in the queue
 */
void* qQueue_Peek( const qQueue_t * const obj ){
    qUINT8_t *RetValue = NULL;

    if( NULL != obj ){
        if( obj->qPrivate.ItemsWaiting > 0u ){
            qCritical_Enter();
            /*cstat -MISRAC2012-Rule-11.5 -CERT-EXP36-C_b*/
            RetValue = (void*)( obj->qPrivate.reader + obj->qPrivate.ItemSize );  /*MISRAC2012-Rule-11.8 allowed*/ /* MISRAC2012-Rule-11.5,CERT-EXP36-C_b deviation allowed */
            /*cstat +MISRAC2012-Rule-11.5 +CERT-EXP36-C_b*/
            if( RetValue >= obj->qPrivate.tail ){
                RetValue = obj->qPrivate.head;
            }
            qCritical_Exit();
        }
    }
    return (void*)RetValue;
}
/*============================================================================*/
/*qBool_t qQueue_RemoveFront( qQueue_t * const obj )
 
Remove the data located at the front of the Queue
 
Parameters:

    - obj : a pointer to the Queue object
  
Return value:

    qTrue if data was removed from the Queue, otherwise returns qFalse
 */
qBool_t qQueue_RemoveFront( qQueue_t * const obj ){
    qBool_t RetValue = qFalse;

    if( NULL != obj ){
        if( obj->qPrivate.ItemsWaiting > 0u ){
            qCritical_Enter();
            qQueue_MoveReader( obj );
            --obj->qPrivate.ItemsWaiting; /* remove the data. */
            qCritical_Exit();
            RetValue = qTrue;
        }
    }
    return RetValue;
}
/*============================================================================*/
static void qQueue_CopyDataToQueue( qQueue_t * const obj, const void *ItemToQueue, const qBool_t xPosition ){
    if( QUEUE_SEND_TO_BACK == xPosition ){
        (void)memcpy( (void*)obj->qPrivate.writer, ItemToQueue, obj->qPrivate.ItemSize );  /*MISRAC2012-Rule-11.8 allowed*/
        obj->qPrivate.writer += obj->qPrivate.ItemSize;
        if( obj->qPrivate.writer >= obj->qPrivate.tail ){
            obj->qPrivate.writer = obj->qPrivate.head;
        }         
    }
    else{
        (void)memcpy( (void*)obj->qPrivate.reader, ItemToQueue, obj->qPrivate.ItemSize );  /*MISRAC2012-Rule-11.8 allowed*/
        obj->qPrivate.reader -= obj->qPrivate.ItemSize;
        if( obj->qPrivate.reader < obj->qPrivate.head ){
            obj->qPrivate.reader = ( obj->qPrivate.tail - obj->qPrivate.ItemSize ); 
        }   
    }
    ++obj->qPrivate.ItemsWaiting;
}
/*==================================================================================*/
static void qQueue_MoveReader( qQueue_t * const obj ){
    obj->qPrivate.reader += obj->qPrivate.ItemSize;
    if( obj->qPrivate.reader >= obj->qPrivate.tail ){
        obj->qPrivate.reader = obj->qPrivate.head;
    }
}
/*==================================================================================*/
static void qQueue_CopyDataFromQueue( qQueue_t * const obj, void * const pvBuffer ){
    qQueue_MoveReader( obj );
    (void)memcpy( (void*)pvBuffer, (void*)obj->qPrivate.reader, obj->qPrivate.ItemSize );  /*MISRAC2012-Rule-11.8 allowed*/
}
/*============================================================================*/
/*qBool_t qQueue_Receive( qQueue_t * const obj, void *dest )
 
Receive an item from a queue (and removes it). The item is received by copy so a 
buffer of adequate size must be provided. The number of bytes copied into the buffer 
was defined when the queue was created.
 
Parameters:

    - obj : a pointer to the Queue object
    - dest: Pointer to the buffer into which the received item will be copied.
  
Return value:

    qTrue if data was retrieved from the Queue, otherwise returns qFalse
*/
qBool_t qQueue_Receive( qQueue_t * const obj, void *dest ){
    qBool_t RetValue = qFalse;
    
    if( NULL != obj ){
        size_t ItemsWaiting = obj->qPrivate.ItemsWaiting; /*to avoid side effects*/
        if( ItemsWaiting > 0u ){
            qCritical_Enter(); 
            qQueue_CopyDataFromQueue( obj, dest ); /* items available, remove one of them. */
            --obj->qPrivate.ItemsWaiting; /* remove the data. */
            qCritical_Exit();
            RetValue = qTrue;
        }        
    }
    return RetValue;
}
/*============================================================================*/
/*qBool_t qQueue_SendGeneric( qQueue_t * const obj, void *ItemToQueue, qQueue_Mode_t InsertMode )
 
Post an item to the back of the queue. The item is queued by copy, not by reference
 
Parameters:

    - obj : a pointer to the Queue object
    - ItemToQueue : A pointer to the item that is to be placed on the queue. The size of 
            the items the queue will hold was defined when the queue was created, 
            so this many bytes will be copied from <ItemToQueue> into the queue storage
            area.
    - InsertMode : Can take the value QUEUE_SEND_TO_BACK to place the item at the back 
                  of the queue, or QUEUE_SEND_TO_FRONT to place the item at the front of 
                  the queue (for high priority messages).
  
Return value:

    qTrue on successful add, qFalse if not added
*/
qBool_t qQueue_SendGeneric( qQueue_t * const obj, void *ItemToQueue, qQueue_Mode_t InsertMode ){
    qBool_t RetValue = qFalse;
    
    if( ( NULL != obj ) && ( InsertMode <= 1u ) ){
        if( obj->qPrivate.ItemsWaiting < obj->qPrivate.ItemsCount ){ /* Is there room on the queue?*/
            qCritical_Enter();
            qQueue_CopyDataToQueue( obj, ItemToQueue, (qBool_t)InsertMode );
            qCritical_Exit();
            RetValue = qTrue;
        }
    }
    return RetValue;   
}
/*============================================================================*/

#endif /* #if (Q_QUEUES == 1) */
