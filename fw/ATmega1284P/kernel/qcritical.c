#include "qcritical.h"

typedef struct{
    qInt_Disabler_t I_Disable;      /*< Point to the user-supplied function used to disable the hardware interrupts. */
    qInt_Restorer_t I_Restorer;     /*< Point to the user-supplied function used to restore the hardware interrupts. */
    volatile qUINT32_t IntFlags;    /*< To save the current interrupt flags before the disable action is performed. */
}qCritical_Handler_t;

static qCritical_Handler_t Critical = { NULL, NULL , 0uL};

/*============================================================================*/
/*void qEnterCritical( void )

Enter a critical section. This function invokes the <Disabler> if available.
Please see <qCritical_SetInterruptsED>

*/  
void qCritical_Enter( void ){
    if( NULL != Critical.I_Disable ){
        qInt_Disabler_t Disabler = Critical.I_Disable;

        Critical.IntFlags = Disabler();
    }
}
/*============================================================================*/
/*void qEnterCritical( void )

Enter a critical section. This function invokes the <Enabler> if available.
Please see <qCritical_SetInterruptsED>

*/ 
void qCritical_Exit( void ){
    if( NULL != Critical.I_Restorer ){
        qInt_Restorer_t Restorer = Critical.I_Restorer;

        Restorer( Critical.IntFlags );
    }
}
/*============================================================================*/
/*void qCritical_SetInterruptsED( const qInt_Restorer_t Restorer, const qInt_Disabler_t Disabler )

Set the hardware-specific code for global interrupt enable/disable. 
Setting this allows you to comunicate safely from Interrupts using queued notifications
or qQueues.

Parameters:

    - Restorer : The function with hardware specific code that enables or 
                 restores interrupts.
    - Disabler : The function with hardware specific code that disables interrupts.
*/ 
void qCritical_SetInterruptsED( const qInt_Restorer_t Restorer, const qInt_Disabler_t Disabler ){
    Critical.I_Restorer = Restorer;
    Critical.I_Disable = Disabler;
}
/*============================================================================*/
