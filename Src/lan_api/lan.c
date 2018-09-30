#include "stm32f1xx_hal.h"
#include "uart_driver.h"
#include "cmsis_os.h"
#include "key_value.h"
#include "lan.h"
#include <stdio.h>
#include <string.h>
#include "common.h"
#include "uart_driver.h"
#include "cycle_queue.h"

#define LAN_DEBUG 1

#if LAN_DEBUG

#define LAN_INFO( fmt, args... ) 	printf( fmt, ##args )//KEY_VALUE_INFO(fmt, ##args)
#else
#define LAN_INFO( fmt, args... )
#endif

extern UART_HandleTypeDef huart4;

void LanReboot( void ){
    HAL_GPIO_WritePin( LAN_RESET_GPIO_Port, LAN_RESET_Pin, GPIO_PIN_RESET );
    osDelay(5);
    HAL_GPIO_WritePin( LAN_RESET_GPIO_Port, LAN_RESET_Pin, GPIO_PIN_SET);
}

void LanEnterConfig( void ){
    MX_UART_Config( &huart4, 9600);
    HAL_GPIO_WritePin( LAN_CONFIG_GPIO_Port, LAN_CONFIG_Pin, GPIO_PIN_RESET );
}

void LanExitConfig( void ){
    HAL_GPIO_WritePin( LAN_CONFIG_GPIO_Port, LAN_CONFIG_Pin, GPIO_PIN_SET );
    MX_UART_Config( &huart4, 921600 );
}

void LanSendData( uint8_t *pdata, uint16_t Length ){
    UartDmaSendData( &huart4, pdata, Length );
}

bool GetLanFlashParameter( uint8_t *lan_parameter ){
    uint32_t array[8] = { 0 };
    uint8_t i = 0;
    if( get_key_value( "lan_baud", UINT32, ( uint8_t *)(&array[ i++ ]) ) &&\
    get_key_value( "lan_packlen", UINT32, ( uint8_t *)(&array[ i++ ]) ) &&\
    get_key_value( "lan_serconf", UINT32, ( uint8_t *)(&array[ i++ ]) ) &&\
    get_key_value( "lan_tcp_mode", UINT32, ( uint8_t *)(&array[ i++ ]) ) &&\
    get_key_value( "lan_dhcp", UINT32, ( uint8_t *)(&array[ i++ ]) ) &&\
    get_key_value( "lan_timeout", UINT32, ( uint8_t *)(&array[ i++ ]) ) &&\
    get_key_value( "lan_port", UINT32, ( uint8_t *)(&array[ i++ ]) ) &&\
    get_key_value( "lan_ip", UINT32, ( uint8_t *)(&array[ i ]) ) ){
        i = 0;
        memcpy( lan_parameter+0 , (uint8_t *)(&array[i++]), 4 );
        memcpy( lan_parameter+4 , (uint8_t *)(&array[i++]), 4 );
        memcpy( lan_parameter+8 , (uint8_t *)(&array[i++]), 3 );
        memcpy( lan_parameter+11, (uint8_t *)(&array[i++]), 1 );
        memcpy( lan_parameter+12, (uint8_t *)(&array[i++]), 1 );
        memcpy( lan_parameter+13, (uint8_t *)(&array[i++]), 1 );
        memcpy( lan_parameter+14, (uint8_t *)(&array[i++]), 2 );
        memcpy( lan_parameter+16, (uint8_t *)(&array[i]), 4 );
        LAN_INFO( "key_value get LAN parameter successful\r\n" );
    }else{
        while( true ){
            LAN_INFO( "key_value get LAN parameter failed\r\n" );
            osDelay( 1000 );
        }
    }
    return true;
}

const uint8_t GetLanConfigCmd[ 8 ][ 5 ] = {\
    {0x57, 0xab, 0x71, 4, 0}, \
    {0x57, 0xab, 0x75, 4, 4}, \
    {0x57, 0xab, 0x72, 3, 8},\
    {0x57, 0xab, 0x60, 1, 11},\
    {0x57, 0xab, 0x83, 1, 12}, \
    {0x57, 0xab, 0x73, 1, 13}, \
    {0x57, 0xab, 0x66, 2, 14},\
    {0x57, 0xab, 0x65, 4, 16}
};

bool GetLanRealParameter( uint8_t *lan_parameter ){

    LanEnterConfig();
    
    uint8_t cycle_count = 0;
    DataType    queuetemp;
    
    for( uint8_t index = 0 ; index < 8; ){
        
        RESEND:
        
        LanSendData( (uint8_t *)GetLanConfigCmd[ index ], 3 );
        
        if( isAck() ){
            cycle_count = 0;
            bool stat = false;
            while( QueueDelete( &seqCQueue, &queuetemp ) ){
                if( queuetemp.size == GetLanConfigCmd[ index ][ 3 ] ){
                    memcpy( lan_parameter+GetLanConfigCmd[ index ][ 4 ], queuetemp.index, queuetemp.size );
                    index ++;
                    stat = true;
                }
            }
            if( !stat ){
                LAN_INFO( "Get LAN PARAMETER QUEUE LEN ERROR\r\n" );
                goto RESEND;
            }
        }else{
            cycle_count ++;
            if( cycle_count >= 10 ){
                LAN_INFO( "Get LAN PARAMETER TIMES EXCEED 5 times\r\n" );
                return false;
            }
            goto RESEND;
        }
    }

    LanExitConfig();
    
    LAN_INFO( "GET LAN PARAMETER SUCCESSFUL\r\n" );

    return true;
}

const uint8_t SetLanConfigCmd[ 9 ][ 5 ] = {\
    {0x57, 0xab, 0x21, 4, 0}, \
    {0x57, 0xab, 0x25, 4, 4}, \
    {0x57, 0xab, 0x22, 3, 8}, \
    {0x57, 0xab, 0x10, 1, 11},\
    {0x57, 0xab, 0x33, 1, 12},\
    {0x57, 0xab, 0x16, 2, 14},\
    {0x57, 0xab, 0x15, 4, 16},\
    {0x57, 0xab, 0x0d, 0, 0}, \
    {0x57, 0xab, 0x02, 0, 0}
};

bool SetLanParameter( uint8_t *lan_flash_parameter ){
    
    LanEnterConfig();
    
    uint8_t cycle_count = 0;
    DataType queuetemp;
    uint8_t curr_cmd[ 32 ] = { 0 };
    
    for( uint8_t index = 0 ; index < 9; ){
        
        memcpy( curr_cmd, SetLanConfigCmd[ index ], 3 );
        memcpy( curr_cmd+3, lan_flash_parameter+SetLanConfigCmd[ index ][4], SetLanConfigCmd[ index ][3] );
        
        RESEND:
        
        LAN_INFO( "set lan config cmd is:");
        for( uint8_t i = 0; i < 3 + SetLanConfigCmd[ index ][3]; i ++ ){
            LAN_INFO( "%02x ", curr_cmd[ i ] );
        }
        LAN_INFO( "\r\n" );
        
        LanSendData( (uint8_t *)curr_cmd, 3 + SetLanConfigCmd[ index ][3] );
        
        if( isAck() ){
            cycle_count = 0;
            bool stat = false;
            while( QueueDelete( &seqCQueue, &queuetemp ) ){
                if( queuetemp.size == 1 && queuetemp.index[ 0 ] == 0xAA ){
                    if( index >= 7 ){
                        osDelay( 100 );
                    }
                    index ++;
                    stat = true;
                }
            }
            if( !stat ){
                LAN_INFO( "LAN SET PARAMETER QUEUE LEN ERROR\r\n" );
                goto RESEND;
            }
        }else{
            cycle_count ++;
            if( cycle_count >= 10 ){
                LAN_INFO( "LAN SET PARAMETER TIMES EXCEED 5 times\r\n" );
                return false;
            }
            goto RESEND;
        }
    }
    
    LanExitConfig();
    
    LAN_INFO( "Set LAN PARAMETER SUCCESSFUL\r\n" );
    
    return true;
}

bool CheckCh9121ConfigMsg( void ){
    
    uint32_t myset_port = 1885;//211.115.110.85:5991
    set_key_value( "lan_port", UINT32, ( uint8_t *)&myset_port );
    
    uint32_t lan_ip = transformatIP( "39.108.231.83" );
    set_key_value( "lan_ip", UINT32, ( uint8_t *)&lan_ip );

    uint32_t LAN_CONFIG = 0;
    if( !(get_key_value( "LAN_CONFIG", UINT32, (uint8_t *)(&LAN_CONFIG)) && LAN_CONFIG == __LAN_CONFIG__) ){
        LAN_CONFIG = __LAN_CONFIG__;
        if( set_key_value( "LAN_CONFIG", UINT32, (uint8_t *)(&LAN_CONFIG)) ){
            uint32_t lan_baud = 921600;
            uint32_t lan_packlen = 1024;
            uint32_t lan_serconf = 0x080401;
            uint32_t lan_tcp_mode = 0x01;
            uint32_t lan_dhcp = 0x01;
            uint32_t lan_timeout = 0x00;
            uint32_t lan_port = 1885;
            uint32_t lan_ip = transformatIP( "39.108.231.83" );
            if( set_key_value( "lan_baud", UINT32, ( uint8_t *)&lan_baud ) &&\
            set_key_value( "lan_packlen", UINT32, ( uint8_t *)&lan_packlen ) &&\
            set_key_value( "lan_serconf", UINT32, ( uint8_t *)&lan_serconf ) &&\
            set_key_value( "lan_tcp_mode", UINT32, ( uint8_t *)&lan_tcp_mode ) &&\
            set_key_value( "lan_dhcp", UINT32, ( uint8_t *)&lan_dhcp ) &&\
            set_key_value( "lan_timeout", UINT32, ( uint8_t *)&lan_timeout ) &&\
            set_key_value( "lan_port", UINT32, ( uint8_t *)&lan_port ) &&\
            set_key_value( "lan_ip", UINT32, ( uint8_t *)&lan_ip ) ){
                LAN_INFO( "Init default LAN flash para successful\r\n" );
            }else{
                while( true ){
                    LAN_INFO( "Init default LAN flash para error\r\n" );
                    osDelay( 1000 );
                }
            }
        }else{
            while( true ){
                LAN_INFO( "Setting the LAN flag error\r\n" );
                osDelay( 1000 );
            }
        }
    }
    
    uint8_t lan_flash_parameter[ 21 ] = { 0 };
    uint8_t lan_real_parameter[ 21 ] = { 0 };

    RECHECK:
    GetLanFlashParameter( lan_flash_parameter );
    GetLanRealParameter( lan_real_parameter );
    
    if( memcmp( lan_flash_parameter, lan_real_parameter, 21 ) != 0x00 ){
        if( !SetLanParameter( lan_flash_parameter ) ){
            LAN_INFO( "LAN SET PARAMETER ERROR\r\n" );
        }
        osDelay( 1000 );
        goto RECHECK;
    }
    
    LAN_INFO( "The configuration parameter is consistent with the default parameter\r\n" );

    return true;
}

void LAN_EXTI_Callback( void ){
    extern void NetReSetState( void );
    NetReSetState();
}



