#include "spi_flash.h"
#include <string.h>

void flash_test( void ){
    uint16_t ID = FLASH_ReadDeviceID();
    printf( "DEVICE ID:%04x\r\n", ID );
    uint8_t buff[64] = { 0 };
    FLASH_Erase_Sector( 0 );
    FLASH_Write( ( uint8_t *)"hello world\r\n", 0, (uint16_t)strlen( "hello world\r\n" ) );
    FLASH_Read( buff, 0, 64 );
    for( uint16_t i = 0; i < 64; i ++ ){
        printf( "%02x ", buff[ i ] );
    }
}




