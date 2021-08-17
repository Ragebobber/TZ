#include "stm32f10x.h"

static void PortBInit(void){
    //PB9 DIR3_Tx/Rx config
    RCC->APB2ENR |= RCC_APB2ENR_IOPBEN; // Тактирование порта В   
    
    GPIOB->CRH &= ~GPIO_CRH_MODE9_0; // Устанавливаем регистр MODE9 в 00
    GPIOB->CRH |= GPIO_CRH_MODE9_1;// Установка 1 бита в [1], второй остается 0 ->MODE9[10] = 2Mhz
    GPIOB->CRH &= ~GPIO_CRH_CNF9;  // 00: General purpose output push-pull 
    GPIOB->BSRR |= GPIO_BSRR_BR9; // Установка 0 для ST485EBD на прием
    
    //Port config for read from ST485EBD PB11
    GPIOB->CRH &= ~GPIO_CRH_CNF11;
    GPIOB->CRH |= GPIO_CRH_CNF11_0;// установка CNF 01
    GPIOB->CRH &= ~GPIO_CRH_MODE11;// режим на прием   
}

static void PortAInit(void){
    //PA1 DIR2_Tx/Rx config
    RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;// Тактирование порта А
    
    GPIOA->CRL &= ~GPIO_CRL_MODE1_0;
    GPIOA->CRL |= GPIO_CRL_MODE1_1; // Mode1[10] = 2Mhz
    GPIOA->CRL &= ~GPIO_CRL_CNF1; // 00: General purpose output push-pull
    GPIOA->BSRR|= GPIO_BSRR_BR1;// Установка 0 для ISO3082DW на прием
    
    //Port config for read from ISO3082DW PA3
    GPIOA->CRL &= ~GPIO_CRL_CNF3;
    GPIOA->CRL |= GPIO_CRL_CNF3_0;//установка CNF 01 
    GPIOA->CRL &= ~GPIO_CRL_MODE3;// режим на прием 
    
    //Port config for  transmit PA9/U1TX
    GPIOA->CRH &= ~GPIO_CRH_CNF9;
    GPIOA->CRH |=GPIO_CRH_CNF9_1;
    GPIOA->CRH |=GPIO_CRH_MODE9_0;
}

static void Usart3Init(void){
    RCC->APB1ENR |= RCC_APB1ENR_USART3EN;// Тактирование для USART3
    USART3->BRR = 0x1d4c; // 9600 бод для 72 Мгц
    USART3->CR1 |= USART_CR1_UE;//Usart on
    USART3->CR1 |= USART_CR1_RE;// read on
    USART3->CR1 |= USART_CR1_RXNEIE;// прерывание по приему
    
    NVIC_EnableIRQ(USART3_IRQn); // включаем прерывание по приему
    __enable_irq();
}

static void Usart2Init(void){
    RCC->APB1ENR |= RCC_APB1ENR_USART2EN;// // Тактирование для USART2
    USART2->BRR = 0x1d4c; // 9600 бод для 72 Мгц
    USART2->CR1 |= USART_CR1_UE;//Usart on
    USART2->CR1 |= USART_CR1_RE;// read on    
}

static void Usart1Init(void){
    RCC->APB2ENR |= RCC_APB2ENR_USART1EN;// Тактирование для USART1
    USART1->BRR = 0x1d4c;// 9600 бод для 72 Мгц
    USART1->CR1 |= USART_CR1_UE;//Usart on
    USART1->CR1 |= USART_CR1_TE;//Transmit on
}

void USART_Transmit(char data){
    if(data == 0) return;
    USART1->DR = data;
    while((USART1->SR & USART_SR_TC)==0){}
        USART1->SR = ~USART_SR_TC;// Очистить флаг окончания передачи
}
void USART3_IRQHandler(void){
    if((USART3->SR & USART_SR_RXNE)!= 0 ){ // Если прием завершен
       USART_Transmit(USART3->DR);       
    }
}

int main(){
    SystemInit();
    PortBInit();
    PortAInit();
    Usart3Init();
    Usart2Init();
    Usart1Init();
    while(1)
    {
        if((USART2->SR & USART_SR_RXNE)!= 0 ){ // Если прием завершен
            USART_Transmit(USART2->DR);    
        }
    }
}
