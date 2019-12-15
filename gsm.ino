#include <gprs.h>
#include <SoftwareSerial.h>
// licza pinów wejściowych
#define pins 2 // liczba aktywanych pinów wejsciowych

//piny zasięgu
#define PINZ0 8
#define PINZ1 9
#define PINZ2 10
#define PINZ3 11
//KONIEC - piny zasięgu

//zmienne gprs/gsm -stany
char APN[]="plus";
char APN_user[]="";
char APN_password[]="";
byte opcja_przylaczenia_do_gprs=0;

int licznik_stanu_GSM[3]={4000,4000,4000};// czasy oczekiwania miedzy przed kolejnym wyslaniem zapytania
long licznik_stanu_GSM_start=0;
int licznik_proba_logowania_gprs=10000;
long licznik_proba_logowania_gprs_start=licznik_proba_logowania_gprs;
int kolejka_zapytan_do_modulu=1;

byte czy_nadajnik_ma_dzialac_z_gprs=1;
byte stan_gprs=0; //!!!!!! // najwazniejsze stany
int stan_sieci=0; //!!!!!!

int licznik_stanu_sieci=30000; // co ile mają przychodzić testy kontroli zalogowania do sieci GSM - po tym czasie zgłasza problem
long licznik_stanu_sieci_start=licznik_stanu_sieci;
int licznik_stan_GPRS=30000; // co ile maja przychodzic testy kontrolne APn z modulu - po tym czasie zgłasza problem
long licznik_stan_GPRS_start=licznik_stan_GPRS;
//KONIEC - zmienne gprs/gsm -stany


//numery telefonów
#define nr_pryw 4 //liczba numerów tel prywatnych
#define nr_SMA 2 // liczba numerów SMA
char nr_prywatny_pam[nr_pryw][14]={"692478153","","",""}; // jezeli nie ma żanych numerów nie wykona się funkcja wysyłania
char roboczy_nr_prywatny[nr_pryw][14]={"","","",""};

char nr_SMA_pam[nr_SMA][14]={"",""};
char roboczy_nr_SMA[nr_SMA][14]={"",""};

//KONIEC - numery telefonów

//definicja komunikatów wysyłanych sms i gprs
String ID_nadajnika="1899";

String komunikat_test="7F";
long timer_test_SMA=600000; // 600 000 to 10 minut
long timer_test_SMA_start=timer_test_SMA; // licznik testów do SMA

char sms_pryw_pam_on[pins][20]={"Wlamanie","Rozbrojenie"};//niby dane z pamieci.. .taka symulacja 
char sms_pryw_pam_off[pins][20]={"Koniec wlamania","Uzbrojenie"};//niby dane z pamieci.. .taka symulacja 

char komunikat_SMS_pryw__pin_on[pins][20]={"",""};//w te miejsce beda zaczydane dane z pamieci
char komunikat_SMS_pryw__pin_off[pins][20]={"",""};

String sms_SMA_pam_on[pins]={"12","21"};//niby dane z pamieci.. .taka symulacja 
String sms_SMA_pam_off[pins]={"13","22"};//niby dane z pamieci.. .taka symulacja 

String komunikat_SMS_SMA__pin_on[pins]={"0","0"};//w te miejsce beda zaczydane dane z pamieci
String komunikat_SMS_SMA__pin_off[pins]={"0","0"};

//KONIEC - definicja komunikatów wysyłanych sms i gprs


//zmienne potrzebne do obsługi wejść alarmowych

int pin[3]={5,6}; // piny wejsciowe
int stan_pinow[pins]; // stan bierzący pinów - będzie tu przechowywany
int ostatni_stan_pinow[pins];
int typ_pinow[pins]={0,0}; // nc=1 no=0 pozniej będzie pobierane z pamięci


int licznik_wejsc[pins]={2000,2000}; //jak długo ma być zwarte wejscie
static long licznik_odniesienia[pins]={0,0}; //potrzebny do wyliczenia czasu zwarcia
//KONIEC - zmienne potrzebne do obsługi wejść alarmowych 






//tworzenie obiektu GPRS/GSM - obiekt typu GPRS z biblioteki gprs streuje funkcjami modułu SIM800
GPRS gprs;
//KONIEC - tworzenie obiektu GPRS/GSM - obiekt typu GPRS z biblioteki gprs streuje funkcjami modułu SIM800


void setup() { // SETUPSETUPSETUPSETUPSETUPSETUPSETUPSETUPSETUPSETUPSETUPSETUPSETUP

//tworzenie połączenia serial
Serial.begin(9600);
//KONIEC tworzenie połączenia serial

//definiowanie pinów dla wejść alarmowych
  pinMode(pin[0],INPUT);
  pinMode(pin[1],INPUT);
//KONIEC - definiowanie pinów dla wejść alarmowych  

//definiowanie pinow zasięgu - poprzez funkcję init_LED (funckja znajduje się gdzieś w pod loopem)
init_LED();
//KONIEC - definiowanie pinow zasięgu - poprzez funkcję init_LED (funckja znajduje się gdzieś w pod loopem)

//metody inicjalizacji wszelkich funkcji związanych z obsługą pinów - np. pobieranie danych z pamięci (w chwili obecnej ze zmiennych tablicowych)

//czesc kodu ktora pobiera dane z PAMIĘCI - start
  for(int i=0;i<pins;i++) // pobieranie tresci smsow z pamieci
  {
  strcpy(komunikat_SMS_pryw__pin_on[i],sms_pryw_pam_on[i]); //symulacja przypisania danych odczytanych z pamieci 
  strcpy(komunikat_SMS_pryw__pin_off[i],sms_pryw_pam_off[i]);//symulacja przypisania danych odczytanych z pamieci
  
  komunikat_SMS_SMA__pin_on[i]=ID_nadajnika+sms_SMA_pam_on[i]; //symulacja przypisania danych odczytanych z pamieci SMA - komunikaty
  komunikat_SMS_SMA__pin_off[i]=ID_nadajnika+sms_SMA_pam_off[i];//symulacja przypisania danych odczytanych z pamieci SMA - komunikaty

  }

  for(int i=0;i<nr_pryw;i++){
    strcpy(roboczy_nr_prywatny[i],nr_prywatny_pam[i]); // kopiowanie numerów z tabeli która symyuluje pamięć
  }
  for(int i=0;i<nr_SMA;i++){  // kopiowanie numerów do SMA z tabeli która symuluje pamięć
    strcpy(roboczy_nr_SMA[i],nr_SMA_pam[i]);
  }

  
//KONIEC czesc kodu ktora pobiera dane z PAMIĘCI - koniec
  for(int i=0;i<pins;i++)
  {
  ostatni_stan_pinow[i]=typ_pinow[i]; // kopiowanie domyslnego stanu pinów do ostatniego stanu na werjsciach - okrelsa logike NC NO dla wejść
  }
//KONIEC - metody inicjalizacji wszelkich funkcji związanych z obsługą pinów - np. pobieranie danych z pamięci (w chwili obecnej ze zmiennych tablicowych)

//metody inicjalizacji wszelkich funkcji związanych ze stanami GPRS/GSM - sprawdzanie łączności z modłem sim800, sprawdzanie pinu(póki co bez pinu), sprawdzenie zalogowania do sieci GSM
 byte proba_logowania_do_sieci=0;
 
 
 while(0 != gprs.init()) {
     delay(1000);
     Serial.println("init error ");
     
     
  }
  
  while((0 !=gprs.networkCheck())&&(proba_logowania_do_sieci<3)){ // jeżeli brak łączności z modułem GSM nie uruchomi się nadajnik - petla dziala do chwili zalogowania lub
    delay(1000);  // funkcja networkCheck sprawdza stan sieci i opcje przyłączenia do odmeny wymiany pakietów w GPRS - ta opcja musi byc wlaczona zeby działał gprs
    proba_logowania_do_sieci++;
    Serial.print("network error "); // !!!!!!!!!!! trzeba sie zastanowic czy tych dówch opcji nie sprawdzać osobno kontola sieci i opcji gprs !!!!! ważne!!!!!!!!!!!!!
    Serial.println(proba_logowania_do_sieci);
    
  }
  
    if(proba_logowania_do_sieci==3){
       stan_sieci=0; // jeżeli nie uzyskamy połączenia po 3 próbach ustawiamy stan sieci na 0. W loopie będziemy kontrolowali stan sieci.
      }else{
       stan_sieci=1; // jeżeli licznik jest 
      }

}







void loop() {  // LOOP LOOP LOOP LOOP LOOP LOOP LOOP LOOP LOOP LOOP LOOP LOOP 
 kontrola_stanow_GSM();
 pobieranie_stanow_GSM();
 logowanie_do_gprs();
  weryfikacja_stanow_wejsc();
 wylisj_TEST_do_SMA();
 //Serial.print("dziala");
//Serial.print(strlen(nr_prywatny_pam[0]));
}













//funkcje obslugi stanu GSM/GPRS
void logowanie_do_gprs(){
  
   if((stan_sieci==1)&(stan_gprs==0)&(czy_nadajnik_ma_dzialac_z_gprs==1)){ // jezeli stan sieci ok, stna gprs 0 czyli niezalogowoany do APN i oczywiscie zogda na polaczenie
    if(millis()>licznik_proba_logowania_gprs_start)
    {
      licznik_proba_logowania_gprs_start=licznik_proba_logowania_gprs+millis();
      if(gprs.join(APN,APN_user,APN_password))
      {
        stan_gprs=1;
      }
    }
   }
}
void kontrola_stanow_GSM(){ //tutaj wysylamy zapytania o stanie systemu

    if(millis()>licznik_stanu_GSM_start)
    {
    
      if(kolejka_zapytan_do_modulu==1)  // rózne czasy oczekiwania na wyslanie kolejnego komunikatu w zaleznosci od tego ile sie czeka na wczesniejsza odpowiedz
      {
        licznik_stanu_GSM_start=licznik_stanu_GSM[0]+millis(); 
      }
      if(kolejka_zapytan_do_modulu==2)
      {
        licznik_stanu_GSM_start=licznik_stanu_GSM[1]+millis();// po zapytaniu o zasieg na odpowiedz czeka się dłużej. dlatego wydłużam czas do następnego wysłania zapytania
      }
      if(kolejka_zapytan_do_modulu==3)
      {
        licznik_stanu_GSM_start=licznik_stanu_GSM[2]+millis();// po zapytaniu o stan gprs na odpowiedz czeka się dłużej. dlatego wydłużam czas do następnego wysłania zapytania
      }

      if(kolejka_zapytan_do_modulu==1){ // pierwsze jest zapytanie o zalogowanie do sieci
      gprs.sendCmd2("AT+CGREG?");
      }
      if(kolejka_zapytan_do_modulu==2){ // nastepnie zasieg
          if(stan_sieci==1) // jezeli stan sieci ok to jest generowane zapytanie
          {
        
             gprs.sendCmd2("AT+CSQ");
          }else  // jezeli nie wraca do zapytnia o stan sieci
          {
             kolejka_zapytan_do_modulu=1;  
          }
      }
      if(kolejka_zapytan_do_modulu==3){ // sprawdzanie zalogowania do APN - czyli stan pracy GPRS
        if(stan_sieci==1) // jezeli stan sieci ok to jest generowane zapytanie
          {
            gprs.sendCmd2("AT+CSTT?");
            
          }else  // jezeli nie wraca do zapytnia o stan sieci
          {
             kolejka_zapytan_do_modulu=1;  
          }
       
      }
      
    }
 
}

void pobieranie_stanow_GSM(){
String serial_raport;
int zas=0;
    if(gprs.checkReadable())
    {
       
        if(serial_raport=gprs.read_serial())
        {
            //Serial.println(serial_raport);
            if(sprawdzanie_otrzymanego_raportu("CGREG: 0,1",serial_raport))//sprawdzanie stanu sieci
            { 
              Serial.println("zalogowany w sieci - reset licznika");
              licznik_stanu_sieci_start=licznik_stanu_sieci+millis(); // przedłużamy timer kontroli stanu sieci
              stan_sieci=1;
              kolejka_zapytan_do_modulu=2;
          
            } 
            if(stan_sieci==1) // jezeli GSM w zalogowany w sieci to sprawdzamy zasieg
            {
              zas=sprawdzanie_zasiegu("CSQ:",serial_raport);
              if(zas>0)
              {
                kolejka_zapytan_do_modulu=3;
                stan_LED_zasieg(zas);
              }
              
              if(sprawdzanie_stanu_GPRS_po_akt_APNie("CSTT:",serial_raport)==1)
              {
                kolejka_zapytan_do_modulu=1;
                stan_gprs=1;
                Serial.println("jest gprs-reset licznika");
                licznik_stan_GPRS_start=licznik_stan_GPRS+millis();
              }
              
            }
          
        }
   
    }    
        
    if(millis()>licznik_stanu_sieci_start) // jezeli po wyznaczonym czasie nie otrzymaliśmy żadnego potwierdzenia z sieci zmieniamy stan sieci na 0
    {
          licznik_stanu_sieci_start=licznik_stanu_sieci+millis();
          stan_sieci=0;
          Serial.println("nie log do sieci");
          stan_LED_zasieg(0);
          
    }
    if(millis()>licznik_stan_GPRS_start) // jezeli po wyznaczonym czasie nie otrzymaliśmy żadnego potwierdzenia z sieci zmieniamy stan sieci na 0
    {
          licznik_stan_GPRS_start=licznik_stan_GPRS+millis();
          stan_gprs=0;
          Serial.println("nie wykryto APN");
         
    }
  
}
int sprawdzanie_stanu_GPRS_po_akt_APNie(String rap, String raport_serial){
int index_pozycji=0; 
int dl_apn;
int kontrola_porownania=0;
        if(raport_serial.indexOf(rap)>0)
          {
              if(index_pozycji=raport_serial.indexOf(rap))
              {
                 index_pozycji=index_pozycji+7;
                 
                 dl_apn=strlen(APN);
                 
                 for(int i=0;i<dl_apn;i++)
                 {
                 
                   if(raport_serial[index_pozycji+i]==APN[i])
                   {
                    kontrola_porownania++; 
                    
                   }
                   
                 }
                 
                 if(kontrola_porownania==dl_apn){
                  return 1;
                 }
                
              }
          }
return 0;
}
int sprawdzanie_zasiegu(String rap,String raport_serial){
 
 int index_pozycji=0;  
 String zasieg; 
 int zasieg_int=0;
 
          if(raport_serial.indexOf(rap)>0)
          {
              if(index_pozycji=raport_serial.indexOf(rap)){
                  index_pozycji=index_pozycji+5;

                 zasieg=raport_serial[index_pozycji];
                 index_pozycji++;
                 if(raport_serial[index_pozycji]==",") 
                 {
                  
                 }else
                 {
                   zasieg+=raport_serial[index_pozycji];
                 }
                 
                 //zasieg_int=zasieg.toInt();

                 Serial.println(zasieg.toInt());
                 return zasieg.toInt();
                 
              }
          }
      
   return 0;   
}
int sprawdzanie_otrzymanego_raportu(String rap,String raport_serial){
    
          if(raport_serial.indexOf(rap)>0){
             
             return 1;
              
          }

   return 0;   
}



void stan_LED_zasieg(int zasieg){

      

    if((zasieg>2)&(zasieg<10)){
      digitalWrite(PINZ0,HIGH);
      digitalWrite(PINZ1,LOW);
      digitalWrite(PINZ2,LOW);
      digitalWrite(PINZ3,LOW);
    }else if((zasieg>=10)&(zasieg<16)){
      digitalWrite(PINZ0,HIGH);
      digitalWrite(PINZ1,HIGH);
      digitalWrite(PINZ2,LOW);
      digitalWrite(PINZ3,LOW);
    }else if((zasieg>=16)&(zasieg<20)){
      digitalWrite(PINZ0,HIGH);
      digitalWrite(PINZ1,HIGH);
      digitalWrite(PINZ2,HIGH);
      digitalWrite(PINZ3,LOW);
    }else if((zasieg>20)){
      digitalWrite(PINZ0,HIGH);
      digitalWrite(PINZ1,HIGH);
      digitalWrite(PINZ2,HIGH);
      digitalWrite(PINZ3,HIGH);
    }
    
  
}
void init_LED(){
  pinMode(PINZ0,OUTPUT);
  pinMode(PINZ1,OUTPUT);
  pinMode(PINZ2,OUTPUT);
  pinMode(PINZ3,OUTPUT);
  digitalWrite(PINZ0,LOW);
      digitalWrite(PINZ1,LOW);
      digitalWrite(PINZ2,LOW);
      digitalWrite(PINZ3,LOW);
}
//KONIEC - funkcje obslugi stanu GSM/GPRS


//funkcje obsługi wejść alarmowych
void odczyt_stanow_pin(){
  
  for(int i=0;i<pins;i++)
  {
     stan_pinow[i]=digitalRead(pin[i]);//czytanie stanów na wszystkich pinach  
  }
  
}
void weryfikacja_stanow_wejsc(){
  odczyt_stanow_pin();
  for(int i=0;i<pins;i++)
  {
    if(stan_pinow[i]!=ostatni_stan_pinow[i])
    {
       
       if(licznik_odniesienia[i]==0){
         licznik_odniesienia[i]=millis(); //jezeli jest zmiana stanu, zaczynamy liczyc czas. punkt od ktorego liczymy zapisujemy w tej zmiennej tablicowej dla danego pinu i-pin
       }
       if((millis()-licznik_odniesienia[i])>licznik_wejsc[i]){ //jezeli uplynal wyznaczony czas zwarcia
        
          komunikat_SMS_pryw(typ_pinow[i],stan_pinow[i],i); // typ pinu to no albo nc definowane wcześniej
          komunikat_SMS_SMA(typ_pinow[i],stan_pinow[i],i);
          ostatni_stan_pinow[i]=stan_pinow[i];// po odczytaniu zmiany stanów, zmieniamy ostatni zapamietany stan
       }
    
    }
    else
    {
      licznik_odniesienia[i]=0;
    }
  }
  
  
  
}
void komunikat_SMS_pryw(int typ_pin,int val_pin,int nr_pin){

      if(val_pin==1)// czyli on zwarcie 
      {
        if(typ_pinow[nr_pin]==0){ // logika normalna NO
          if(stan_sieci==1)
          {
           wyslij_sms_pryw(komunikat_SMS_pryw__pin_on[nr_pin]);
          }else Serial.println("funkcja bufor dla kom pryw"); // tu powinna znajdowac się funkcja która w sytuacji braku sieci GSM zapisuje zmiane stanu do bufora
        }else if(typ_pinow[nr_pin]==1) // logika NC
        {
          if(stan_sieci==1)
          {
          wyslij_sms_pryw(komunikat_SMS_pryw__pin_off[nr_pin]);
          }else Serial.println("funkcja bufor dla kom pryw"); // tu powinna znajdowac się funkcja która w sytuacji braku sieci GSM zapisuje zmiane stanu do bufora

        }
      }
      else if(val_pin==0) //czyli on rozwarcie
      {
        if(typ_pinow[nr_pin]==0){ // logika normalna NO
          if(stan_sieci==1)
          {
           wyslij_sms_pryw(komunikat_SMS_pryw__pin_off[nr_pin]);
          }else Serial.println("funkcja bufor dla kom pryw"); // tu powinna znajdowac się funkcja która w sytuacji braku sieci GSM zapisuje zmiane stanu do bufora

        }else if(typ_pinow[nr_pin]==1) // logika NC
        {
          if(stan_sieci==1)
          {
          wyslij_sms_pryw(komunikat_SMS_pryw__pin_on[nr_pin]);
          }else Serial.println("funkcja bufor dla kom pryw"); // tu powinna znajdowac się funkcja która w sytuacji braku sieci GSM zapisuje zmiane stanu do bufora

        }
      }
   
}
void komunikat_SMS_SMA(int typ_pin,int val_pin,int nr_pin){

    if(val_pin==1)// czyli on zwarcie 
    {
      if(typ_pinow[nr_pin]==0){ // logika normalna NO
        
          if(stan_sieci==1)
          {
              wyslij_sms_SMA(komunikat_SMS_SMA__pin_on[nr_pin]);
          }else Serial.println("zapis do bufora kom SMA"); // tu powinna znajdowac się funkcja która w sytuacji braku sieci GSM zapisuje zmiane stanu do bufora
   
      }else if(typ_pinow[nr_pin]==1) // logika NC
      {
          Serial.println("sms");
         if(stan_sieci==1)
          {
         wyslij_sms_SMA(komunikat_SMS_SMA__pin_off[nr_pin]);
          }else Serial.println("zapis do bufora kom SMA"); // tu powinna znajdowac się funkcja która w sytuacji braku sieci GSM zapisuje zmiane stanu do bufora
   
      }
    }
    else if(val_pin==0) //czyli off rozwarcie
    {
      if(typ_pinow[nr_pin]==0){ // logika normalna NO
        Serial.println("sms");
         if(stan_sieci==1)
          {
          wyslij_sms_SMA(komunikat_SMS_SMA__pin_off[nr_pin]);
          }else Serial.println("zapis do bufora kom SMA"); // tu powinna znajdowac się funkcja która w sytuacji braku sieci GSM zapisuje zmiane stanu do bufora
   
      }else if(typ_pinow[nr_pin]==1) // logika NC
      {
        Serial.println("sms");
          if(stan_sieci==1)
          {
          wyslij_sms_SMA(komunikat_SMS_SMA__pin_on[nr_pin]);
          }else Serial.println("zapis do bufora kom SMA"); // tu powinna znajdowac się funkcja która w sytuacji braku sieci GSM zapisuje zmiane stanu do bufora
   
      }
    }
   
}
void wylisj_TEST_do_SMA(){
    if(millis()>timer_test_SMA_start)
    {
      timer_test_SMA_start=timer_test_SMA+millis();
      if(wyslij_sms_SMA(komunikat_test)){
        
      }else Serial.println("E:BladSMStest"); // test powinnien wyladaowac w buforze SMA
    }
}
//KONIEC - funkcje obsługi wejść alarmowych


//funkcje wystlania komunikatów
void wyslij_sms_pryw(char* tresc){

  
   for(int i=0;i<nr_pryw;i++)
   {

      if(strlen(roboczy_nr_prywatny[i])>4)
      {
        
        if(0==gprs.sendSMS(roboczy_nr_prywatny[i],tresc)){
          Serial.print("Kom pryw: ");
          Serial.print(roboczy_nr_prywatny[i]);
          Serial.print(":");
          Serial.println(tresc);
        }else Serial.println("E:SMSprywNieWyslany"); // powinno isc do buforu  jeżeli użytkownik zaznaczył taką opcje
      }
      
   }
  
}
int wyslij_sms_SMA(String tresc){

  
   for(int i=0;i<nr_SMA;i++)
   {

      if(strlen(roboczy_nr_SMA[i])>4)
      {
        char tresc2[10];
        tresc.toCharArray(tresc2,10);
        
        if(0==gprs.sendSMS(roboczy_nr_SMA[i],tresc2))
        {
            Serial.print("Kom SMA: ");
            Serial.print(roboczy_nr_SMA[i]);
            Serial.print(":");
            Serial.println(tresc2);
            return 1;
        }else Serial.println("E:SMSsmaNIEwyslany"); // powinno isc do buforu    
      }
      
   }
  
}
//KONIEC funkcje wystlania komunikatów
