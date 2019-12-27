#ifndef DTMF_H_
#define DTMF_H_
#include <Arduino.h>
#define STQ 12
#define Q4  11
#define Q3  10
#define Q2  9
#define Q1  8    

#define pin_tone 5 // PIN NADAJ¥CY DO CENTRALI
#define czas_po_nadaniu_numeru_start_val 1000
#define czas_po_odebraniu_cid_start_val  1500

extern long czas_po_nadaniu_numeru;
extern long czas_po_odebraniu_cid;

extern String cid;
extern byte index_cid_char;

extern struct //definicja zmiennych operacyjnych dla czasu
{
	char sczas_1 :1;
	char sczas_2 :1;
}start_czas;

extern struct { // definicja etapów interpretacji sygna³ów
	char e1  :1;
	char e2  :1;
	char e3  :1;
	char raz :1;
	char etap:4; // ETAPY KONTAKTU Z CENTRALA DTMF
}state;

int rozpznanie_wej_DTMF();
void dtmf_init();
void cykl();

#endif