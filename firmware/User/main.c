/********************************** (C) COPYRIGHT *******************************
 * File Name          : main.c
 * Author             : WCH
 * Version            : V1.0.0
 * Date               : 2023/12/25
 * Description        : Main program body.
 *********************************************************************************
 * Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
 * Attention: This software (modified or not) and binary are used for 
 * microcontroller manufactured by Nanjing Qinheng Microelectronics.
 *******************************************************************************/

/*
 *@Note
 *Multiprocessor communication mode routine:
 *Master:USART1_Tx(PD5)\USART1_Rx(PD6).
 *This routine demonstrates that USART1 receives the data sent by CH341 and inverts
 *it and sends it (baud rate 115200).
 *
 *Hardware connection:PD5 -- Rx
 *                     PD6 -- Tx
 *
 */

#include "debug.h"


/* Global define */


/* Global Variable */
vu8 val;

/*********************************************************************
 * @fn      init_gpio
 *
 * @brief   Initializes PC1,2 & 4 and PA1.
 *
 * @return  none
 */
void init_gpio(void)
{
    ADC_InitTypeDef ADC_InitStructure = {0};
    GPIO_InitTypeDef  GPIO_InitStructure = {0};

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC | RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOD, ENABLE);

    /* Port C1=led2 2=led3 4=led4 */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_4;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_30MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(GPIOC, &GPIO_InitStructure);

    /* Port D6=led1 D4=led5*/
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_4;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_30MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(GPIOD, &GPIO_InitStructure);

    /* Port A0*/

     RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
     RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
     RCC_ADCCLKConfig(RCC_PCLK2_Div8);

     GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
     GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
     GPIO_Init(GPIOA, &GPIO_InitStructure);
     
     ADC_DeInit(ADC1);
     ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;
     ADC_InitStructure.ADC_ScanConvMode = DISABLE;
     ADC_InitStructure.ADC_ContinuousConvMode = ENABLE; 
     ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None; 
     ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
     ADC_InitStructure.ADC_NbrOfChannel = 1; 
     ADC_Init(ADC1, &ADC_InitStructure);

     ADC_RegularChannelConfig(ADC1, ADC_Channel_0, 1, ADC_SampleTime_241Cycles);

     ADC_Cmd(ADC1, ENABLE);
     ADC_ResetCalibration(ADC1);
     while(ADC_GetResetCalibrationStatus(ADC1));
     ADC_StartCalibration(ADC1);
     while(ADC_GetCalibrationStatus(ADC1));

     ADC_SoftwareStartConvCmd(ADC1, ENABLE);


}

/*********************************************************************
 * @fn      blink
 *
 * @brief   toggle leds
 *
 * @return  none
 */
void blink_gpio(void)
{
    GPIO_WriteBit(GPIOC, GPIO_Pin_1, (GPIO_ReadOutputDataBit(GPIOC, GPIO_Pin_1) == Bit_SET) ? Bit_RESET : Bit_SET );
    Delay_Ms(50);

    GPIO_WriteBit(GPIOC, GPIO_Pin_2, (GPIO_ReadOutputDataBit(GPIOC, GPIO_Pin_2) == Bit_SET) ? Bit_RESET : Bit_SET );
    Delay_Ms(50);

    GPIO_WriteBit(GPIOC, GPIO_Pin_4, (GPIO_ReadOutputDataBit(GPIOC, GPIO_Pin_4) == Bit_SET) ? Bit_RESET : Bit_SET );
    Delay_Ms(50);

    GPIO_WriteBit(GPIOD, GPIO_Pin_6, (GPIO_ReadOutputDataBit(GPIOD, GPIO_Pin_6) == Bit_SET) ? Bit_RESET : Bit_SET );
    Delay_Ms(50);

    GPIO_WriteBit(GPIOD, GPIO_Pin_4, (GPIO_ReadOutputDataBit(GPIOD, GPIO_Pin_4) == Bit_SET) ? Bit_RESET : Bit_SET );
    Delay_Ms(50);

}
void Get_ADC_Val(void)
{
    uint16_t adcValue = ADC_GetConversionValue(ADC1);

    if(adcValue < 10)    
    {
        GPIO_WriteBit(GPIOD, GPIO_Pin_4, Bit_RESET ); //led5        
        GPIO_WriteBit(GPIOC, GPIO_Pin_4, Bit_RESET ); //led4
        GPIO_WriteBit(GPIOC, GPIO_Pin_2, Bit_RESET ); //led3
        GPIO_WriteBit(GPIOC, GPIO_Pin_1, Bit_RESET ); //led2
        GPIO_WriteBit(GPIOD, GPIO_Pin_6, Bit_RESET ); //led1       
    } else if(adcValue < 200) {
        GPIO_WriteBit(GPIOD, GPIO_Pin_4, Bit_SET ); //led5
        GPIO_WriteBit(GPIOC, GPIO_Pin_4, Bit_RESET ); //led4
        GPIO_WriteBit(GPIOC, GPIO_Pin_2, Bit_RESET ); //led3
        GPIO_WriteBit(GPIOC, GPIO_Pin_1, Bit_RESET ); //led2
        GPIO_WriteBit(GPIOD, GPIO_Pin_6, Bit_RESET ); //led1 
    } else if(adcValue < 400) {
        GPIO_WriteBit(GPIOD, GPIO_Pin_4, Bit_SET ); //led5
        GPIO_WriteBit(GPIOC, GPIO_Pin_4, Bit_SET ); //led4
        GPIO_WriteBit(GPIOC, GPIO_Pin_2, Bit_RESET ); //led3
        GPIO_WriteBit(GPIOC, GPIO_Pin_1, Bit_RESET ); //led2
        GPIO_WriteBit(GPIOD, GPIO_Pin_6, Bit_RESET ); //led1
    }  else if(adcValue < 600) {
        GPIO_WriteBit(GPIOD, GPIO_Pin_4, Bit_SET ); //led5
        GPIO_WriteBit(GPIOC, GPIO_Pin_4, Bit_SET ); //led4
        GPIO_WriteBit(GPIOC, GPIO_Pin_2, Bit_SET ); //led3
        GPIO_WriteBit(GPIOC, GPIO_Pin_1, Bit_RESET ); //led2
        GPIO_WriteBit(GPIOD, GPIO_Pin_6, Bit_RESET ); //led1 
    }  else if(adcValue < 800) {
        GPIO_WriteBit(GPIOD, GPIO_Pin_4, Bit_SET ); //led5
        GPIO_WriteBit(GPIOC, GPIO_Pin_4, Bit_SET ); //led4
        GPIO_WriteBit(GPIOC, GPIO_Pin_2, Bit_SET ); //led3
        GPIO_WriteBit(GPIOC, GPIO_Pin_1, Bit_SET ); //led2
        GPIO_WriteBit(GPIOD, GPIO_Pin_6, Bit_RESET ); //led1 
    } else {
        GPIO_WriteBit(GPIOD, GPIO_Pin_4, Bit_SET ); //led5
        GPIO_WriteBit(GPIOC, GPIO_Pin_4, Bit_SET ); //led4
        GPIO_WriteBit(GPIOC, GPIO_Pin_2, Bit_SET ); //led3
        GPIO_WriteBit(GPIOC, GPIO_Pin_1, Bit_SET ); //led2
        GPIO_WriteBit(GPIOD, GPIO_Pin_6, Bit_SET ); //led1  
    }
           
    // Delay_Ms(1000);
    
}


/*********************************************************************
 * @fn      main
 *
 * @brief   Main program.
 *
 * @return  none
 */
int main(void)
{
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
    SystemCoreClockUpdate();
    Delay_Init();
  /*  USART_Printf_Init(115200);
    USARTx_CFG();
    printf("helllllo")
    Delay_Ms(3000); */
    init_gpio();
    blink_gpio();

    while(1) 
    {
    Get_ADC_Val();
    }
}
