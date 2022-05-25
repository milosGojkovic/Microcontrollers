#define MY_FREQ  1000 // in Hz
#define MY_PERIOD  ((1000000/MY_FREQ) - 1) // in µsec
#include <STM32ADC.h>
#include <HardwareTimer.h>
STM32ADC myADC(ADC1); 

uint8 pin = PA7; 

volatile static bool dma1_ch1_Active; //flag for interrupt

float usPeriod,timer_freq,ary[320];
uint16_t timPeriod_c;

#define maxSamples 4000
uint16_t buffer[maxSamples];
uint64_t perr=10;

void setup() {
 


    // Setup LED Timer

    // Setup Counting Timers
    Timer2.setMode(TIMER_CH2, TIMER_OUTPUTCOMPARE);
    Timer2.setMode(TIMER_CH3, TIMER_OUTPUTCOMPARE);
    Timer2.pause();
    Timer2.setPrescaleFactor(1440);//1440
    Timer2.setCount(0);
    Timer2.setOverflow(1000);
    Timer2.setCompare(TIMER_CH2, 100);   // 100
    Timer2.setCompare(TIMER_CH3, 1000);//1000
    Timer2.attachInterrupt(TIMER_CH2, handler1);
    Timer2.attachInterrupt(TIMER_CH3, handler1);
    Timer2.resume();
    



  Serial.begin(115200); 
  pinMode(pin, INPUT_ANALOG);//AD pin.
  pinMode(PB3,OUTPUT);//C3
  pinMode(PB4,OUTPUT);//C4
  pinMode(PB5,OUTPUT);//C5
  pinMode(PB6,OUTPUT);//C6
  pinMode(PB7,OUTPUT);//C7
  pinMode(PB8,OUTPUT);//C8
  pinMode(PB12,OUTPUT);//L2
  pinMode(PB13,OUTPUT);//L3
  pinMode(PB9,OUTPUT);//Osciloskop
  pinMode(PB14,OUTPUT);//Generator
  pinMode(PB15,OUTPUT);//Veza PC
  pinMode(PA15,OUTPUT);//L1,C1,C2
    
  
  
  timer_freq = 1000000/perr;
  timPeriod_c = 72000000/timer_freq;
  pinMode(PA1,OUTPUT);
  pinMode(PC13,OUTPUT);
  digitalWrite(PC13,HIGH);
  digitalWrite(PB14,HIGH);
}

bool up=false;
char zz[15];
String ff;
int temp;
void loop() 
{  
     if(Serial)
     digitalWrite(PB15,HIGH);
     else
     digitalWrite(PB15,LOW);
  
     if(Serial.available())
     {
      
     
     digitalWrite(PC13,HIGH);
      Timer2.pause();
      Timer2.setCount(0);
      
     ff=Serial.readStringUntil('x');
     if(ff=="-0" || ff=="-1")
     {
     if(ff=="-1")
     
     digitalWrite(PB9,HIGH);
     else
     digitalWrite(PB9, LOW);
     }
     
     
     else
     {
     Timer2.setCompare(TIMER_CH2, ff.toInt()*10);
     }
     ff=Serial.readStringUntil('x');//dovde cita duty
     Timer2.setPrescaleFactor(72000/ff.toInt());
     ff=Serial.readStringUntil('y');//dovde cita stanja
     if(ff[0]=='0') digitalWrite(PB6,HIGH); else digitalWrite(PB6,LOW);//C3
     if(ff[1]=='0') digitalWrite(PB5,HIGH); else digitalWrite(PB5,LOW);//C4
     if(ff[2]=='0') digitalWrite(PB4,HIGH); else digitalWrite(PB4,LOW);//C5
     if(ff[3]=='0') digitalWrite(PB3,HIGH); else digitalWrite(PB3,LOW);//C6
     if(ff[4]=='0') digitalWrite(PB7,HIGH); else digitalWrite(PB7,LOW);//C7
     if(ff[5]=='0') digitalWrite(PB8,HIGH); else digitalWrite(PB8,LOW);//C8
     if(ff[6]=='1') digitalWrite(PA15,HIGH); else digitalWrite(PA15,LOW);//C1,C2,L1
     if(ff[8]=='0') digitalWrite(PB12,HIGH); else digitalWrite(PB12,LOW);//L2
     if(ff[9]=='0') digitalWrite(PB13,HIGH); else digitalWrite(PB13,LOW);//L3
     digitalWrite(PB9,HIGH);
     delay(50);
     
         Timer2.resume();
         digitalWrite(PA1,HIGH);
        
         up= true;

      DMA();
      if(digitalRead(PB9)==LOW)
      goto preskoci;

   
      if(dma1_ch1_Active == 0)
      {
      
        for(int x=0; x<maxSamples; x++){
         
        
         sprintf(zz,"%d,",buffer[x]);
          Serial.print(zz);
          delayMicroseconds(20);
              }
              Serial.println(";");
          }
           digitalWrite(PC13,LOW);
            preskoci:
            ff=Serial.read();
     }
    
      
    }


void DMA(){
  
        Timer3.setPrescaleFactor(1);
        Timer3.setOverflow(timPeriod_c);
        

  Timer3.setMasterModeTrGo(TIMER_CR2_MMS_UPDATE);
  myADC.setSampleRate(ADC_SMPR_1_5); //default - myADC.setSampleRate(ADC_SMPR_1_5); 
  myADC.setPins(&pin, 1);
  dma1_ch1_Active = 1;
  myADC.setTrigger(ADC_EXT_EV_TIM3_TRGO);
  myADC.setDMA(buffer, maxSamples, (DMA_MINC_MODE | DMA_TRNS_CMPLT), DMA1_CH1_Event);  

    while (dma1_ch1_Active >= 1){}; //wait for DMA to complete.  
    dma_disable(DMA1, DMA_CH1); //End of trasfer, disable DMA and Continuous mode.
        
  }

 void handler1(void) {
    up^=1;
    digitalWrite(PA1,up);
} 
static void DMA1_CH1_Event() {
  dma1_ch1_Active = 0;
}
