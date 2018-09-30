#include "cmsis_os.h"
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

#define LAN_APP_DEBUG 1
#if LAN_APP_DEBUG
#define LAN_APP_INFO( fmt, args... ) 	printf( fmt, ##args )//KEY_VALUE_INFO(fmt, ##args)
#else
#define LAN_APP_INFO( fmt, args... )
#endif

osThreadId  LAN_SEND_TASK_HANDLE = NULL;

void LanSendCycle( void const * argument ){

    for( ;; ){
        extern void LanReadySendMsg( void );
        LanReadySendMsg();
    }
}

void LanThread( void ){
    osThreadDef( lan_send, LanSendCycle, osPriorityNormal, 1, 512);
    LAN_SEND_TASK_HANDLE = osThreadCreate( osThread( lan_send ), NULL);
    if( LAN_SEND_TASK_HANDLE == NULL ){
      LAN_APP_INFO( "create LAN SEND thread failed\r\n" );
      while( true );
    }
}



























