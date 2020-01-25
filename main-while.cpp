//KNIHOVNY
#include <Arduino.h>
#include <DHT.h>
#include <math.h>
#include "WiFi.h"
//define
#define   chlaz_pin   26        //pin rele, ktere spousti vetrani
#define   teptel_pin  27        //pin rele, ktere spousti topne teleso
#define   teplomer1   13        //pin DHT11 teplomeru/vlhkomeru
#define   H_M_A1      12        //H-mustek A1 pin
#define   H_M_A2      14        //H-mustek A2 pin
#define   H_M_B1      16        //H-mustek B1 pin
#define   H_M_B2      17        //H-mustek B2 pin
#define   RELE_SEP    0x0       //rele_sep = LOW - definovano jen pro omezeni chyb v programu
#define   RELE_ROZEP  0x1       //rele_rozep = HIGH - definovano jen pro omezeni chyb v programu
#define   A_VLH_PIN   15       //pin analagoveho vstupu vlhkomeru pudy
#define   D_VLH_PIN   13       //digitalni vstup vlkomeru pudy - prekroceni limitu trimmeru
#define   VCC_VLH_PIN 11        //napajeni vlkomeru pudy
#define   hodina      600000    //definovani hodiny / estetika programu
//promenne
//pro regulator teploty
float momtep = 0.0;//teplota z teplomeru
float opttep = 30.00;//pozadovana hodnota
float tep_odch;//regulacni odchylka - rozdil teplot
float tep_predchozi_odch;//predchozi odchylka
float tep_zesil = 0;//akcni zasah
float Pt=2;/*P konstanta*/ float St=1;/*S konstanta*/ float Dt=2.33;/*D konstanta*/
int PSD_pt = 0;/*velikost P*/ int PSD_st= 0; /*velikost S*/ int PSD_dt = 0; /*velikost D*/ 
//casove promenne
unsigned long ubeh_cas, cas, min_cas;//ubehnuty cas, momentalni cas, predchozi cas 
//teplomer
DHT vnitrni(teplomer1, DHT11);

//pudni vlhkomer


void setup() {
  Serial.begin(115200);
  pinMode(A_VLH_PIN, INPUT);
  pinMode(D_VLH_PIN, INPUT);
  pinMode(VCC_VLH_PIN, OUTPUT);
  pinMode(teptel_pin, OUTPUT);
  pinMode(chlaz_pin, OUTPUT);
  pinMode(H_M_A1, OUTPUT);
  pinMode(H_M_A2, OUTPUT);
  vnitrni.begin();
  digitalWrite(VCC_VLH_PIN, LOW);
}

void regulator_teloty(){
  momtep = vnitrni.readTemperature();
  tep_odch = opttep - momtep;
  PSD_pt = Pt * tep_odch;
  min_cas = cas;
  if( -5 <tep_odch && tep_odch > 5){
    PSD_st = PSD_st + (St * tep_odch);
  }

  cas = millis();
  ubeh_cas = (cas - min_cas) /1000;
  PSD_dt = Dt*((tep_odch - tep_predchozi_odch)/ubeh_cas);
  tep_zesil = PSD_pt + PSD_st + PSD_pt;
  tep_zesil = constrain(tep_zesil, -255, 255);
  Serial.print("Akcni zasah -> ");
  Serial.println(tep_zesil);
  Serial.print("Momentalni odchylka -> ");
  Serial.println(tep_odch);
}

void regulace_teploty(){
  while (tep_zesil > 5){
    digitalWrite(teptel_pin, RELE_SEP);
    break;
  }
  while(-2 > tep_zesil && tep_zesil >2){
    digitalWrite(teptel_pin, RELE_ROZEP);
    digitalWrite(chlaz_pin, RELE_ROZEP);
    break;
  }
  while (tep_zesil <= -5){
    digitalWrite(teptel_pin, RELE_ROZEP);
    digitalWrite(chlaz_pin, RELE_SEP);
    break;
  }
}

void zalevani(){
  if(millis() - cas > hodina){
    digitalWrite(VCC_VLH_PIN, HIGH);
    delay(100);
    int ANALOG_HODNOTA  = analogRead(A_VLH_PIN);
    bool LIMIT          = digitalRead(D_VLH_PIN);
    Serial.print("Analogovy vstup vlhkomeru -> ");
    Serial.println(ANALOG_HODNOTA);
      if(LIMIT == HIGH)
      {
        digitalWrite(H_M_A1, LOW);
        digitalWrite(H_M_A2 , LOW);
        Serial.println("Puda je zalita!");
      }else{
        digitalWrite(H_M_A1, HIGH);
        digitalWrite(H_M_A2 , LOW);
        Serial.println("Zalevam");
        delay(15000);
        digitalWrite(H_M_A1, LOW);
        digitalWrite(H_M_A2 , LOW);
        Serial.println("Zalito!");
    }
    digitalWrite(VCC_VLH_PIN, LOW);
    cas = millis(); 
  }
}

void loop(void) {
  regulator_teloty();
  regulace_teploty();
  delay(993);
}

