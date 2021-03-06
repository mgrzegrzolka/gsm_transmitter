#include <Arduino.h>
#include "dtmf.h"

long czas_po_nadaniu_numeru=czas_po_nadaniu_numeru_start_val;
long czas_po_odebraniu_cid=czas_po_odebraniu_cid_start_val;      
String cid="";
byte index_cid_char=0;

int rozpznanie_wej_DTMF(){
 if(digitalRead(STQ)==HIGH){       //When a DTMF tone is detected, STQ will read HIGH for the duration of the tone.

  if(state.raz==1){
    
    if((digitalRead(Q4)==LOW)&(digitalRead(Q3)==LOW)&(digitalRead(Q2)==LOW)&(digitalRead(Q1)==HIGH)){    //1  
          
          //Serial.println(1);
          state.raz=0; 
          return 1;
        }else if((digitalRead(Q4)==LOW)&(digitalRead(Q3)==LOW)&(digitalRead(Q2)==HIGH)&(digitalRead(Q1)==LOW)){     //2 
         
          //Serial.println(2);
          state.raz=0; 
          return 2;
        }else if((digitalRead(Q4)==LOW)&(digitalRead(Q3)==LOW)&(digitalRead(Q2)==HIGH)&(digitalRead(Q1)==HIGH)){  //3    
          
         // Serial.println(3);
          state.raz=0; 
          return 3;
        }else if((digitalRead(Q4)==LOW)&(digitalRead(Q3)==HIGH)&(digitalRead(Q2)==LOW)&(digitalRead(Q1)==LOW)){   //4   
          
          //Serial.println(4);
          state.raz=0; 
          return 4;
        }else if((digitalRead(Q4)==LOW)&(digitalRead(Q3)==HIGH)&(digitalRead(Q2)==LOW)&(digitalRead(Q1)==HIGH)){  //5    
          
          //Serial.println(5);
          state.raz=0; 
          return 5;
        }else if((digitalRead(Q4)==LOW)&(digitalRead(Q3)==HIGH)&(digitalRead(Q2)==HIGH)&(digitalRead(Q1)==LOW)){ //6     
          
          //Serial.println(6);
          state.raz=0; 
          return 6;
        }else if((digitalRead(Q4)==LOW)&(digitalRead(Q3)==HIGH)&(digitalRead(Q2)==HIGH)&(digitalRead(Q1)==HIGH)){  //7     
          
          //Serial.println(7);
          state.raz=0; 
          return 7;
        }else if((digitalRead(Q4)==HIGH)&(digitalRead(Q3)==LOW)&(digitalRead(Q2)==LOW)&(digitalRead(Q1)==LOW)){  //8    
          
          //Serial.println(8);
          state.raz=0; 
          return 8;
        }else if((digitalRead(Q4)==HIGH)&(digitalRead(Q3)==LOW)&(digitalRead(Q2)==LOW)&(digitalRead(Q1)==HIGH)){  //9   
          
          //Serial.println(9);
          state.raz=0; 
          return 9;
        }else if((digitalRead(Q4)==HIGH)&(digitalRead(Q3)==LOW)&(digitalRead(Q2)==HIGH)&(digitalRead(Q1)==LOW)){  //10    
         
          //Serial.println(0);
          state.raz=0; 
          return 10;
        }else if((digitalRead(Q4)==HIGH)&(digitalRead(Q3)==LOW)&(digitalRead(Q2)==HIGH)&(digitalRead(Q1)==HIGH)){ //11    
          
         // Serial.println(bit_wej_dtmf);
          state.raz=0; 
          return 11;
        }else if((digitalRead(Q4)==HIGH)&(digitalRead(Q3)==HIGH)&(digitalRead(Q2)==LOW)&(digitalRead(Q1)==LOW)){  //12    
          
         // Serial.println(bit_wej_dtmf);
          state.raz=0; 
          return 12;
        }else if((digitalRead(Q4)==HIGH)&(digitalRead(Q3)==HIGH)&(digitalRead(Q2)==LOW)&(digitalRead(Q1)==HIGH)){  //13    
          
        //  Serial.println(bit_wej_dtmf);
          state.raz=0; 
          return 13;
        }else if((digitalRead(Q4)==HIGH)&(digitalRead(Q3)==HIGH)&(digitalRead(Q2)==HIGH)&(digitalRead(Q1)==LOW)){ //14     
          
        //  Serial.println(bit_wej_dtmf);
          state.raz=0; 
          return 14;
        }else if((digitalRead(Q4)==HIGH)&(digitalRead(Q3)==HIGH)&(digitalRead(Q2)==HIGH)&(digitalRead(Q1)==HIGH)){  //15    
          
          //Serial.println(bit_wej_dtmf);
          state.raz=0; 
          return 15;
        }
   
      
       
      return -1;
      }
  return -1;    
  }else{
    state.raz=1;
  return -1;  
  }
  




}

void dtmf_init() {

  start_czas={0,0};
  state={1,1,1,1,1};// ustawiwnie domyślne dla state

  Serial.begin(9600);
  pinMode(STQ, INPUT);
  pinMode(Q4, INPUT);
  pinMode(Q3, INPUT);
  pinMode(Q2, INPUT);
  pinMode(Q1, INPUT);
}

/*=========================================================================================================
loop() : Arduino will interpret the DTMF module output and position the Servo accordingly
========================================================================================================== */
void cykl() {
  
  
  if(state.etap==1){// ODBIERNIAE NR TELEFONU ... PO SEKUNDOWEJ PRZERWIE (MINILANIE DLUŻSZA) PODAJEM SYGNAŁY Z ETAPU 2 HANDSHAKE
     if(state.e1==1){
      state.e1=0;
    Serial.println("etap1");
    tone(pin_tone,300);
    }
    if(rozpznanie_wej_DTMF()!=(-1)){
      
      start_czas.sczas_1=1;
      czas_po_nadaniu_numeru=1200+millis();
    }
 
  }
  if(start_czas.sczas_1==1){ // JEZELI MINEŁA SEKUNDA (1.2S) OD OSTATNIEGO KOMUNIKATU .. BITU WTEDY PRZECHODZIMY DO ETAPU 2 ... GDZIE POTWIERDZAMY ODEBRANIE NUMERU HANDSHAKE
    if(millis()>czas_po_nadaniu_numeru){
      state.etap=2;
      start_czas.sczas_1=0;
      state.e2=1;
    }
  }
  
  
  if(state.etap==2){ // HANDSHAKE
    if(state.e2==1){
      state.e2=0;
    Serial.println("etap2");
    }
    tone(pin_tone,1400);
    delay(100);
    noTone(pin_tone);
    tone(pin_tone,2300);
    delay(100);
    noTone(pin_tone);
    
    state.etap=3;
    state.e3=1;
  }
  
  if(state.etap==3){ // ETTAP 3 TO ODCZYTYWANIE CIDA
    if(state.e3==1){
      state.e3=0;
    Serial.println("etap3");
    }
   int cid1=rozpznanie_wej_DTMF();
   
   if(cid1!=(-1)){
    start_czas.sczas_2=1;
    czas_po_odebraniu_cid=13000+millis();
    
      switch( cid1 )
      {
        case 1:
          cid.concat("1");
        break;
        case 2:
          cid.concat("2");
        break;
        case 3:
         cid.concat("3");
        break;
        case 4:
          cid.concat("4");
        break;
        case 5:
          cid.concat("5");
        break;
        case 6:
          cid.concat("6");
        break;
        case 7:
          cid.concat("7");
        break;
        case 8:
          cid.concat("8");
        break;
        case 9:
          cid.concat("9");
        break;
        case 10:
          cid.concat("0");
        break;
        case 11:
          cid.concat("B");
        break;
        case 12:
          cid.concat("C");
        break;
        case 13:
          cid.concat("D");
        break;
        case 14:
          cid.concat("E");
        break;
        case 15:
          cid.concat("F");
        break;
        
      }
        
      
      
      index_cid_char++;
   }
   if(index_cid_char==16){
      index_cid_char=0;
      Serial.print("Nadajemy: ");
      Serial.println(cid);
      cid="";
      delay(300);
      tone(5,1400);
      delay(10000);
      noTone(5);
    }
 
  }
  if(start_czas.sczas_2==1){ // jezeli minal czas po nadaniu ostatniego cida ... wracamy do punktu 1
    //Serial.println("kuoa");
    if(millis()>czas_po_odebraniu_cid){
      state.etap=1;
      start_czas.sczas_2=0;
      state.e1=1;
    }
  }
  
    
}
