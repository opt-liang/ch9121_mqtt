#include "stm32f1xx_hal.h"
#include "cycle_queue.h"
#include "cmsis_os.h"
extern UART_HandleTypeDef huart4;
extern DMA_HandleTypeDef hdma_uart4_rx;
extern DMA_HandleTypeDef hdma_uart4_tx;
SeqCQueue   seqCQueue;
#define DEBUG_INFO(fmt,args...)  printf(fmt, ##args)

/*-------------------private variable----------------------------------*/
DataType CACHE;
USART_RECEIVETYPE UsartType;

/*-------------------public function----------------------------------*/
void MX_UART_Config( UART_HandleTypeDef *huart, int baud ){
    DEBUG_INFO("Change the baud rate:%d\r\n", baud );
    UART_REINIT:
    HAL_UART_DeInit( huart );
    huart->Init.BaudRate = baud;
    huart->Init.WordLength = UART_WORDLENGTH_8B;
    huart->Init.StopBits = UART_STOPBITS_1;
    huart->Init.Parity = UART_PARITY_NONE;
    huart->Init.Mode = UART_MODE_TX_RX;
    huart->Init.HwFlowCtl = UART_HWCONTROL_NONE;
    huart->Init.OverSampling = UART_OVERSAMPLING_16;

    if (HAL_UART_Init(huart) != HAL_OK)
    {
        DEBUG_INFO("initialization failed\r\n" );
        osDelay( 1000 );      
        goto UART_REINIT;
    }

    HAL_UART_Receive_DMA( huart, seqCQueue.currentCache, LEFTRAMSIZE );

    __HAL_UART_ENABLE_IT(huart,UART_IT_IDLE);

    if( huart->Instance == USART3 ){
      
        /* UART3_IRQn interrupt configuration */
        HAL_NVIC_SetPriority(USART3_IRQn, 5, 0);           //add code
        HAL_NVIC_EnableIRQ(USART3_IRQn);
        USART3->SR;
        HAL_NVIC_SetPriority(DMA1_Channel2_IRQn, 5, 0); //add code
        HAL_NVIC_EnableIRQ(DMA1_Channel2_IRQn);
        HAL_NVIC_SetPriority(DMA1_Channel3_IRQn, 5, 0);   //add code
        HAL_NVIC_EnableIRQ(DMA1_Channel3_IRQn);            
    }else if( huart->Instance == UART4 ){
      
        /* UART3_IRQn interrupt configuration */
        HAL_NVIC_SetPriority(UART4_IRQn, 5, 0);           //add code
        HAL_NVIC_EnableIRQ(UART4_IRQn);
        UART4->SR;
        HAL_NVIC_SetPriority(DMA2_Channel3_IRQn, 5, 0); //add code
        HAL_NVIC_EnableIRQ(DMA2_Channel3_IRQn);
        HAL_NVIC_SetPriority(DMA2_Channel4_5_IRQn, 5, 0);   //add code
        HAL_NVIC_EnableIRQ(DMA2_Channel4_5_IRQn);
    }
}

void InitPeripherals( void ){
      QueueInitiate( &seqCQueue, &UsartType.usartDMA_rxBuf[0] );
}

void UartDmaSendData( UART_HandleTypeDef* huart, uint8_t *pdata, uint16_t Length ){
    if( HAL_UART_Transmit( huart, pdata, Length, 100 ) != HAL_OK ){
        HAL_UART_DMAResume( huart );
        HAL_UART_Receive_DMA( huart, seqCQueue.currentCache, LEFTRAMSIZE );
    }
}

void UartIdleReceiveData( UART_HandleTypeDef* huart, SeqCQueue *Queue ){
    
    if( ( __HAL_UART_GET_FLAG( huart,UART_FLAG_IDLE ) != RESET) ){
        __HAL_UART_CLEAR_IDLEFLAG(huart);
        HAL_UART_DMAStop(huart);
        uint32_t temp = huart->hdmarx->Instance->CNDTR;
		if( ( LEFTRAMSIZE - temp ) != 0x00 && ( LEFTRAMSIZE - temp ) < LEFTRAMSIZE ){
            CACHE.index = Queue->currentCache;
            if( huart->Instance == UART4 ){
                CACHE.size  = ( LEFTRAMSIZE - temp ) ;
            }else if( huart->Instance == USART3 ){
                CACHE.size  = ( LEFTRAMSIZE - temp ) + 1 ;
                Queue->currentCache[ CACHE.size - 1 ] = '\0';
            }

            if( QueueAppend( Queue, CACHE ) ){
                #if 1
                    Queue->leftram += CACHE.size;
                    if( ( RECEIVEBUFLEN - Queue->leftram ) >= LEFTRAMSIZE ){
                        Queue->currentCache += CACHE.size;
                    }else{
                        Queue->leftram = 0;
                        Queue->currentCache = Queue->heapcache;
                    }
                #else
                    if( Queue->queue[Queue->front].index < (Queue->currentCache + CACHE.size) ){
                        if( (RECEIVEBUFLEN - (Queue->currentCache - Queue->heapcache + CACHE.size) ) >= LEFTRAMSIZE ){
                            Queue->currentCache += CACHE.size;
                        }else{
                            if( (Queue->queue[Queue->front].index - Queue->heapcache) >= LEFTRAMSIZE ){
                                Queue->currentCache = Queue->heapcache;
                            }
                        }
                    }else{
                        if( (Queue->queue[Queue->front].index - (Queue->currentCache + CACHE.size) ) >= LEFTRAMSIZE ){
                            Queue->currentCache += CACHE.size;
                        }
                    }
                #endif
            }
		}else{
            HAL_UART_DMAResume( huart );
        }
		
		HAL_UART_Receive_DMA( huart, Queue->currentCache, LEFTRAMSIZE );
    }
}
/********************************************************************************/


