#include <miosix.h>
#include <stdio.h>
#include <bitset>
#include <sstream>
#include <iostream>
#include <sys/_default_fcntl.h>
#include <algorithm>
#include <pthread.h>
using namespace miosix;
using namespace std;
typedef Gpio<GPIOD_BASE,15> blueLed;    //b
typedef Gpio<GPIOD_BASE,14> redLed;     //r
typedef Gpio<GPIOD_BASE,12> greenLed;   //g
typedef Gpio<GPIOD_BASE,13> orangeLed;  //o
void initTempADC();
void initBluetoothModule();
void printBits(size_t const size, void  * ptr);
string mySplit(string str);
#define BUFSIZE 20
int fd;
string START="FF#";
string END= "#FF";
string LOGTAG="#SmartHome-Log: ";
pthread_t t1;

void* bluetoothRead(void*) {
    printf(LOGTAG.append("Bluetooth read is on\n").c_str());
    char buf2[10];
    for(;;) {
        read(fd,buf2,10);
        string str=mySplit(string(buf2));
        
        if(str.substr(0,1)=="b"){
            if(str.substr(1,1)=="0")
                blueLed::low();
            else
                blueLed::high();
        }
        else if(str.substr(0,1)=="r"){
            if(str.substr(1,1)=="0")
                redLed::low();
            else
                redLed::high();
        }
        else if(str.substr(0,1)=="g"){
            if(str.substr(1,1)=="0")
                greenLed::low();
            else
                greenLed::high();
        }
        else if(str.substr(0,1)=="o"){
            if(str.substr(1,1)=="0")
                orangeLed::low();
            else
                orangeLed::high();
        }
        else if(str.substr(0,1)=="t"){
            double Vsense=(ADC1->DR*3.0)/4095.0;
            double temp=((Vsense-0.76)/2.5)+25;
            std::stringstream ss;
            string str;
            double answer = static_cast<double>(static_cast<int>(temp * 100.)) / 100.;
            ss << START <<answer <<END<< "\r\n";
            str = ss.str();
            int a=write(fd, str.c_str(), str.length());
        }
    }
}

int main()
{
    blueLed::mode(Mode::OUTPUT);
    redLed::mode(Mode::OUTPUT);
    orangeLed::mode(Mode::OUTPUT);
    greenLed::mode(Mode::OUTPUT);
    initTempADC();
    initBluetoothModule();
    pthread_join(t1,NULL);
    printf("joined");
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
   
   fd = open("/dev/bluetooth", O_RDWR | O_NONBLOCK);
   
   //Testing the FileDescriptor
   if(fd == -1){ 
       printf(LOGTAG.append("Couldn't Open FileDescriptor\n").c_str());
   }
   else{
       pthread_create(&t1, NULL, &bluetoothRead, NULL);
    }
         
}

string mySplit(string str){
    string first="FF#";
    string end="#FF";
    std::string::size_type s=str.find(first);
    std::string::size_type e=str.find(end);
    string temp=str.substr(s+first.length(),e-(s+first.length()));
    return temp;
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