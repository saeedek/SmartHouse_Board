#include <miosix.h>
#include <stdio.h>
#include <bitset>
#include <iostream>
#include <sys/_default_fcntl.h>
using namespace miosix;
using namespace std;
void initTempADC();
void initBluetoothModule();
void printBits(size_t const size, void  * ptr);

int main()
{
    initTempADC();
	initBluetoothModule();
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
void initBluetoothModule(){
    {
        FastInterruptDisableLock dLock;
        //<- The constructor of dLock has disabled interrupts

        RCC->APB2ENR |= RCC_APB1ENR_USART2EN; //Enable USART1

    }   //<- The destructor of dLock enables back interrupts
    
    //Enable USART
    USART2->CR1 |= (1<<13);
    int fd=open("/dev/bluetooth",O_RDWR);
    
    for(;;){
        Thread::sleep(500);
        int a=write(fd, "This will be output to testfile.txt\n", 36);
        fflush(fd);
        printf("Printed to %d , %d Bytes\n",fd,a);
    }
   
}
void printBits(size_t const size, void * ptr)
{
    unsigned char *b = (unsigned char*) ptr;
    unsigned char byte;
    int i, j;

    for (i=size-1;i>=0;i--)
    {
        for (j=7;j>=0;j--)
        {
            byte = (b[i] >> j) & 1;
            printf("%u", byte);
        }
    }
    puts("");
}