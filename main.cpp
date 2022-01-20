/* Copyright (C) Vít Chvoj - All Rights Reserved
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 * Written by Vít Chvoj <vitchvoj@gmail.com>, January 2020
 */
//KNIHOVNY
#include <SPI.h>
#include <Arduino.h>
#include <Esp.h>
#include <DHT.h>
#include <math.h>
#include "WiFi.h"
#include <WiFiAP.h>
#include <WiFiMulti.h>
#include <Servo.h>
#include <ESPAsyncWebServer.h>
#include <AsyncTCP.h>
//moje knihovny
const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML><html>
<head>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <meta charset="UTF-8">
  <style>
  .tlacitko {
  background-color: #00b5d1;
  border: none;
  color: white;
  padding: 8px 16px;
  text-align: center;
  text-decoration: none;
  display: inline-block;
  font-size: 16px;
  margin: 4px 2px;
  cursor: pointer;
}
    body {
      background-color: #ffffff;
      color: #000000;
    }
    html {
     font-family: monospace;
     display: inline-block;
     margin: 0px auto;
     text-align: center;
    }
    p{ font-size: 2.0rem; }
    .jednotky { font-size: 1rem; }
    .hodnoty{
      font-size: 2.0 rem;
      vertical-align:middle;
      padding-bottom: 20px;
    }
    .tab{
	display:inline-block; width:ONE-TAB-WIDTH;
}
  </style>
</head>
<body>
  <title>Automatizovany sklenik - Chvoj</title>
  <div id="box">
  <u style="color:#00b5d1"><h1 style="font-size:48px;color:#00b5d1"> Přehled</h1></u>
  <p>
    <span class="hodnoty">🌡️<u>Teplota</u></span>
    <br>
    <span class="hodnoty">🏠Uvnitr</span>
    <span class="hodnoty">&nbsp&nbsp🌲Venku</span>
    <br>
    <span id="teplotauvnitr">&nbsp&nbsp&nbsp&nbsp&nbsp%TEPIN%</span>
    <sup class="jednotky">&deg;C</sup>
    <span id="teplotavenku">%TEPOUT%</span>
    <sup class="jednotky">&deg;C</sup>
  </p>
  <p>
    <span class="hodnoty">💦<u>Vlhkost</u></span>
    <br>
    <span class="hodnoty">🏠Uvnitr</span>
    <span class="hodnoty">&nbsp&nbsp🌲Venku</span>
    <br>
    <span id="vlhkouvnitr">&nbsp&nbsp&nbsp&nbsp&nbsp%VLHIN%‍‍</span>
    <sup class="jednotky">&percnt;</sup>
    <span id="vlhkovenku">%VLHOUT%</span>
    <sup class="jednotky">&percnt;</sup>
  </p>
    <p>
    <span class="hodnoty"><u>Vetrani</u></span>
    <br>
    <span id="vetrakos">%CHLAZENI%</span>
  </p>
     <p>
    <span class="hodnoty"><u>Topeni</u></span>
    <br>
    <span id="topenicko">%TOPENI%</span>
  </p>
  <u style="color:#00b5d1"><h3 style="font-size:36px;color:#00b5d1">Manuální ovládání</h3></u>
  <br>
  <table style="margin:auto">
  <tr>
<th>
  <span class="hodnoty">Větrání</span>
    <br>
  <button type="button" class="tlacitko" onclick="VyzvaVetrani()">Zmenit stav</button>
  <br>
  <span id="vetranickobt">%STAVVETBT%</span>
</th>
<th>
  <span class="hodnoty">Topení</span>
  <br>
  <button type="button" class="tlacitko" onclick="VyzvaTopeni()">Zmenit stav</button>
  <br>
  <span id="topenickobt">%STAVTOPBT%</span>
</th>
    </tr>
</table>
  </div>
</body>
<script>
function VyzvaTopeni(){
    var presmerovaniX;
    if(confirm("Upozornění: Pokud je topení zapnuto díky regulátoru, akce se neprovede!\nNa základě bezpečnosti, bude větrání zastaveno během dalších 30 minut!\nPřejete si pokračovat?")){
        presmerovaniX = window.location.href="/topenibt";
    }
}
function VyzvaVetrani(){
    var presmerovaniY;
    if(confirm("Upozornění: Pokud je větrání zapnuto díky regulátoru, akce se neprovede!\nNa základě bezpečnosti, bude topení vypnuto během dalších 30 minut!\nPřejete si pokračovat?")){
        presmerovaniY = window.location.href="/vetranibt";
    }
}
$.ajax({
    type: "GET",
    url: "/vetranibt",
    success: function(btn_Vetrak){
        document.getElementById("vetranickobt").innerHTML = this.responseText;
        $.ajax({
    type: "GET",
    url: "/topenibt",
    success: function(btn_Topeni){
        document.getElementById("topenickobt").innerHTML = this.responseText;
        $.ajax({
    type: "GET",
    url: "/teplotauvnitr",
    success: function(mom_Teplota_Uvnitr){
        document.getElementById("teplotauvnitr").innerHTML = this.responseText;
        $.ajax({
    type: "GET",
    url: "/teplotavenku",
    success: function(mom_Teplota_Venku){
        document.getElementById("teplotavenku").innerHTML = this.responseText;
        $.ajax({
    type: "GET",
    url: "/topenicko",
    success: function(stav_topeni){
        document.getElementById("topenicko").innerHTML = this.responseText;
        $.ajax({
    type: "GET",
    url: "/vetrakos",
    success: function(stav_vetraku){
        document.getElementById("vetrakos").innerHTML = this.responseText;
        $.ajax({
    type: "GET",
    url: "/vlhkouvnitr",
    success: function(mom_Vlhkost_Uvnitr){
        document.getElementById("vlhkouvnitr").innerHTML = this.responseText;
        $.ajax({
    type: "GET",
    url: "/vlhkovenku",
    success: function(mom_Vlhkost_Venku){
        document.getElementById("vlhkovenku").innerHTML = this.responseText;
    },
    error: function(chyba){
        document.getElementById("vlhkovenku").innerHTML = "---";
    }
});
    },
    error: function(chyba){
        document.getElementById("vlhkouvnitr").innerHTML = "---";
    }
});
    },
    error: function(chyba){
        document.getElementById("vetrakos").innerHTML = "---";
    }
});
    },
    error: function(chyba){
        document.getElementById("topenicko").innerHTML = "---";
    }
});
    },
    error: function(chyba){
        document.getElementById("teplotavenku").innerHTML = "---";
    }
});
    },
    error: function(chyba){
        document.getElementById("teplotauvnitr").innerHTML = "---";
    }
});
    },
    error: function(chyba){
        document.getElementById("topenickobt").innerHTML = "---";
    }
});
    },
    error: function(chyba){
        document.getElementById("vetranickobt").innerHTML = "---";
    }
});
</script>
</html>)rawliteral";
//define
#define   chlaz_pin   26        //pin rele, ktere spousti vetrani
#define   bt_chlaz    25        //pin rele, ktere spousti vetrani pres webserver
#define   teptel_pin  27        //pin rele, ktere spousti topne teleso
#define   bt_tep      12        //pin rele, ktere spousti topne teleso pres webserver
#define   teplomer1   22        //pin DHT11 teplomeru/vlhkomeru vnitrni
#define   teplomer2   23        //pin DHT11 teplomeru/vlhkomeru venkovni
#define   H_M_A1      18        //H-mustek A1 pin
#define   H_M_A2      19        //H-mustek A2 pin
#define   RELE_SEP    0x0       //rele_sep = LOW - definovano jen pro omezeni chyb v programu
#define   RELE_ROZEP  0x1       //rele_rozep = HIGH - definovano jen pro omezeni chyb v programu
#define   A_VLH_PIN   35        //pin analagoveho vstupu vlhkomeru pudy
#define   D_VLH_PIN   16        //digitalni vstup vlkomeru pudy - prekroceni limitu trimmeru
#define   VCC_VLH_PIN 17        //napajeni vlkomeru pudy
#define   hodina      3600000    //definovani hodiny / estetika programu
#define   STAV_ON     "Zapnuto"
#define   STAV_OFF    "Vypnuto"
bool      manualni_ovladani;
//promenne
const char* ssid = "Sklenik-AP";
const char* password = "bobmarley";
//const char* ssid = "TP-LINK_D8F4";
//const char* password = "31071973";
AsyncWebServer server(80);
//pro regulator teploty
float momtep = 0;             //teplota z teplomeru
float opttep = 30.00;           //pozadovana hodnota
float tep_odch = 0;                 //regulacni odchylka - rozdil teplot
float tep_zesil = 0;            //akcni zasah
float P = 10;                   //konstanta P
float I = 0.6;                  //konstanta I
int PIp = 0;                    //akcni zasah P regulatoru
int PIi = 0;                    //akcni zasah I regulatoru

//casove promenne
unsigned long ubeh_cas, cas, min_cas;//ubehnuty cas, momentalni cas, predchozi cas
//teplomery
DHT vnitrni(teplomer1, DHT11);
DHT venkovni(teplomer2, DHT11);
float venktep;
//Serva
Servo okno1;
Servo okno2;

int pos = 0;
//Promenne pro web
String btn_topeni(){
  if(teptel_pin == RELE_SEP){//nemuzu zapnout topne teleso, kdyz bezi
    Serial.println("topeni je jiz zapnuto!");
    return String();
  }
  else{
    digitalWrite(bt_tep, !digitalRead(bt_tep));
    return String(digitalRead(bt_tep));//vratim aktualni stav
  }
  return String();
}
String btn_vetrani(){
  if(chlaz_pin == RELE_SEP){//nemuzu zapnout vetrak, kdyz bezi
    Serial.println("chlazeni jiz probiha!");
    return String();
  }
  else  {
  digitalWrite(bt_chlaz, !digitalRead(bt_chlaz));
  return String(digitalRead(bt_chlaz));//vratim aktualni stav
  }
  return String();
}
String cteni_TeplotyIn(){
  float vnitrni_teplota = vnitrni.readTemperature();
  Serial.println(vnitrni_teplota);
  return String(vnitrni_teplota);
}
String cteni_VlhkostiIn(){
  float vnitrni_vlhkost = vnitrni.readHumidity();
  Serial.println(vnitrni_vlhkost);
  return String(vnitrni_vlhkost);
}
String cteni_TeplotyOut(){
  float venkovni_teplota = venkovni.readTemperature();
  Serial.println(venkovni_teplota);
  return String(venkovni_teplota);
}
String cteni_VlhkostiOut(){
  float venkovni_vlhkost = venkovni.readHumidity();
  Serial.println(venkovni_vlhkost);
  return String(venkovni_vlhkost);
}
String stav_vetraku(){
  int stav = digitalRead(chlaz_pin);
  int b_stav = digitalRead(bt_chlaz);
  char stav_z[] = STAV_ON;
  char stav_v[] = STAV_OFF;
  if(stav == HIGH && b_stav == HIGH){
    return F(stav_v);
    return String();
  } else if(stav == LOW || b_stav == LOW){
    return F(stav_z);
    return String();
  }
  return String();
}
String stav_topeni(){
   int stav = digitalRead(bt_tep);
  int b_stav = digitalRead(teptel_pin);
  char stav_z[] = STAV_ON;
  char stav_v[]= STAV_OFF;
  if(stav == HIGH && b_stav == HIGH){
    return F(stav_v);
    return String();
  }
  else if(stav == LOW || b_stav == LOW){
    return F(stav_z);
    return String();
  }
  return String();
}

String processor(const String& var){
  if(var == "TEPIN"){
    return cteni_TeplotyIn();
  }
  else if(var == "TEPOUT"){
    return cteni_TeplotyOut();
  }
  else if(var == "VLHIN"){
    return cteni_VlhkostiIn();
  }
  else if(var == "VLHOUT"){
    return cteni_VlhkostiOut();
  }
  else if(var == "CHLAZENI"){
    return stav_vetraku();
  }
  else if(var == "TOPENI"){
    return stav_topeni();
  }
  else if(var == "STAVVETBT"){
    return stav_vetraku();
  }
  else if(var == "STAVTOPBT"){
    return stav_topeni();
  }
}

void setup() {
  Serial.begin(115200);
  okno1.attach(33);
  okno2.attach(32);
  pinMode(A_VLH_PIN, INPUT);//pudni vlhkomer
  pinMode(D_VLH_PIN, INPUT);// -||-
  pinMode(VCC_VLH_PIN, OUTPUT);// -||-
  pinMode(teptel_pin, OUTPUT);//rele na topne telisko
  pinMode(bt_tep, OUTPUT);
  pinMode(chlaz_pin, OUTPUT);//rele na vetrak
  pinMode(bt_chlaz, OUTPUT);
  pinMode(H_M_A1, OUTPUT);//h mustek
  pinMode(H_M_A2, OUTPUT);//-||-
  vnitrni.begin();
  venkovni.begin();
  digitalWrite(chlaz_pin, RELE_ROZEP);
  digitalWrite(VCC_VLH_PIN, LOW);
  digitalWrite(bt_chlaz, RELE_ROZEP);
  digitalWrite(teptel_pin, RELE_ROZEP);
  digitalWrite(bt_tep, RELE_ROZEP);
WiFi.begin(ssid, password);
WiFi.enableAP(true);

  WiFi.softAP(ssid, password, 5);
  IPAddress IP = WiFi.softAPIP();
  WiFi.softAPConfig(IP, IP, 252);
  Serial.print("AP IP address: ");
  Serial.println(IP);

 /* while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi..");
  }*/

  // Print ESP32 Local IP Address
  ///Serial.println(WiFi.localIP());

  // Route for root / web page
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/html", index_html, processor);
  });
  server.on("/teplotauvnitr", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/plain", cteni_TeplotyIn().c_str());
  });
  server.on("/teplotavenku", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/plain", cteni_TeplotyOut().c_str());
  });
  server.on("/vlhkostin", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/plain", cteni_VlhkostiIn().c_str());
  });
  server.on("/vlhkostout", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/plain", cteni_VlhkostiOut().c_str());
  });
   server.on("/vetrakos", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/plain", stav_vetraku().c_str());
  });
  server.on("/topenicko", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/plain", stav_topeni().c_str());
  });
   server.on("/vetranibt", HTTP_GET, [](AsyncWebServerRequest *request){
    request->redirect("/");
    request->send(200, "text/plain", btn_vetrani().c_str());
  });
  server.on("/topenibt", HTTP_GET, [](AsyncWebServerRequest *request){
    request->redirect("/");
    request->send(200, "text/html", btn_topeni().c_str());
  });

  // Start server
  server.begin();
}

void regulator_teloty(){
  momtep = vnitrni.readTemperature();//ctu z DHT
  if(millis() - cas > 500){
  if (isnan(momtep))
  {
  Serial.println("Chyba cteni!");
  cas = millis();
  }else{
  tep_odch = opttep - momtep;//odectu momentalni teplotu (z DHT) s pozadovanou
  PIp = P * tep_odch;//vypocitam P
  PIi = PIi + (I * tep_odch);//vypocitam I
  tep_zesil = PIp + PIi;//sectu P, I a dostanu akcni zasah
  tep_zesil = constrain(tep_zesil, -255, 255);//oseknu hodnoty akcniho zasahu na min. -255 a max. 255
  Serial.print("Akcni zasah -> ");
  Serial.println(tep_zesil);//vypisu
  Serial.print("Momentalni odchylka -> ");
  Serial.println(tep_odch);//vypisu (jen z duvodu testovani)
  cas = millis();
  }
}
}

void regulace_teploty(){
  digitalWrite(teptel_pin, 255 - tep_zesil); //pokud akcni zasah nabude 255 rele se sepne a zacne topit
  digitalWrite(chlaz_pin, 255 + tep_zesil); //pokud akcni zasah nabude -255 rele se sepne a zacne chladit

}

void zalevani(){
  if(millis() - min_cas > 5000){//kontrola vlhkosti pudy jednou za hodinu (jeste netestovano)
    digitalWrite(VCC_VLH_PIN, HIGH);//po hodine se zapne
    delay(100);
    int ANALOG_HODNOTA  = analogRead(A_VLH_PIN); //vycte analogovou hodnotu
    bool LIMIT          = digitalRead(D_VLH_PIN);//digitalni limit nastaveny na trimmeru
    Serial.print("Analogovy vstup vlhkomeru -> ");
    Serial.println(ANALOG_HODNOTA);//vypisu
      if(LIMIT == HIGH)
      {
        digitalWrite(H_M_A1, HIGH);//budu zalejvat
        digitalWrite(H_M_A2 , LOW);
        Serial.println("Zalevam");
        delay(15000);//po dobu 15 vterin
        digitalWrite(H_M_A1, LOW);//vypnu
        digitalWrite(H_M_A2 , LOW);
        Serial.println("Zalito!");//vypisu

      }else{
        digitalWrite(H_M_A1, LOW);//H mustek nic nedela
        digitalWrite(H_M_A2 , LOW);
        Serial.println("Puda je zalita!");
        }
    digitalWrite(VCC_VLH_PIN, LOW);//vypnu vlhkomer
    min_cas = millis();
}
}
void otevirani_oken(){
  venktep = venkovni.readTemperature();
    if(tep_zesil > 0 && venktep > momtep){
        int pozice1 = tep_zesil; int pozice2 = tep_zesil;
        pozice1 = map(pozice1, 0, 255, 0, 180); //(akcni zasah 255 = 180° na servu)serva jsou zrcadlove proti sobe
        pozice2 = map(pozice2, -255, 255, 180, 0); //(akcni zasah 255 = 0° na servu)
        okno1.write(pozice1);
        okno2.write(pozice2);
      delay(20);
    }
     if(tep_zesil < 0 && venktep < momtep){
        int pozice1 = tep_zesil; int pozice2 = tep_zesil;
        pozice1 = map(pozice1, 0, -255, 0, 180); //(akcni zasah 255 = 180° na servu)serva jsou zrcadlove proti sobe
        pozice2 = map(pozice2, 0, -255, 180, 0); //(akcni zasah 255 = 0° na servu)
        okno1.write(pozice1);
        okno2.write(pozice2);
      delay(20);
}
}

void loop() {
  regulator_teloty();
  regulace_teploty();
  zalevani();
  otevirani_oken();
}

