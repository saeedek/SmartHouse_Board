#include <miosix.h>
#include <stdio.h>
#include <bitset>
#include <iostream>
#include <sys/_default_fcntl.h>
using namespace miosix;
using namespace std;
void initTempADC();
void printBits(size_t const size, void  * ptr);

int main()
{
    initTempADC();
}
void initTempADC(){
    
     {
        FastInterruptDisableLock dLock;
        //<- The constructor of dLock has disabled interrupts

        RCC->APB2ENR |= RCC_APB2ENR_ADC1EN; //Enable ADC1

    }   //<- The destructor of dLock enables back interrupts
    
   //Select the Channel
    ADC1->SQR3 &= ~(1<<0);
    ADC1->SQR3 &= ~(1<<1);
    ADC1->SQR3 &= ~(1<<2);
    ADC1->SQR3 &= ~(1<<3);
    ADC1->SQR3 |=  (1<<4);

    //Select Sampling Rate 3 cycles
    ADC1->SMPR1 &= ~(1<<18);
    ADC1->SMPR1 &= ~(1<<19);
    ADC1->SMPR1 &= ~(1<<20);
    
    //Enabling the Temp sensor
    ADC->CCR |= (1<<23);
    ADC1->CR2 |= (1<<0);
    ADC1->CR2 |= (1<<1);
    ADC1->CR2 &= ~(1<<11); //Right alignment of the DR
    ADC1->CR2 |= (1<<30);
   
    
}