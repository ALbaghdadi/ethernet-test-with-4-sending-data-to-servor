//25/5/2022 Hussain Aziz
// ZMPT101b connect to pin 0
// SEN32 REV1.1 AC and DC voltage sensor 0-50v connect to pin 1 

// DC current sensor connect to pin 2 0-50mv 0-50A

//AC current sensor at pin 3 sct013 current sensor

#include "EmonLib.h"             // Include Emon Library

#include <SPI.h>
#include <Ethernet.h>
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED }; //Setting MAC Address



int IDlocation=1;


float voltageAC;
float currentAC;
float voltageDC;
float currentDC;

IPAddress server(172,23,174,101);
//char server[] = "192.168.0.100";
IPAddress ip(10,5,52,2); 

EthernetClient client; 


#define VOLT_CAL 592

EnergyMonitor emon1;             // Create an instance

int DC_current_pin=2;
void setup()
{  
  Serial.begin(9600);
  emon1.voltage(0, VOLT_CAL, 1.7);  // Voltage: input pin, calibration, phase_shift
    emon1.current(3, 111.1);             // Current: input pin, calibration.

 Serial.println("HI"); 
  Serial.println("Initialize Ethernet with DHCP:");
  if (Ethernet.begin(mac) == 0) {
    Serial.println("Failed to configure Ethernet using DHCP");
    // Check for Ethernet hardware present
    if (Ethernet.hardwareStatus() == EthernetNoHardware) {
      Serial.println("Ethernet shield was not found.  Sorry, can't run without hardware. :(");
      while (true) {
        delay(1); // do nothing, no point running without Ethernet hardware
      }
    }
    if (Ethernet.linkStatus() == LinkOFF) {
      Serial.println("Ethernet cable is not connected.");
    }
    // try to congifure using IP address instead of DHCP:
    Ethernet.begin(mac, ip);
  } else {
    Serial.print("  DHCP assigned IP ");
    Serial.println(Ethernet.localIP());
  }
    Serial.println(Ethernet.localIP());
  delay(1000);
  
}

void loop()
{
  emon1.calcVI(25,1000);         // Calculate all. No.of half wavelengths (crossings), time-out
  
  float supplyVoltage   = emon1.Vrms;             //extract Vrms into Variable
  
 Serial.print("VoltageAC : ");  
 Serial.print(supplyVoltage);

 // DC voltage measure
 float DCvoltage=analogRead(1)*50.0/1023-0.54;
 Serial.print("   VoltageDC : ");  
 Serial.print(DCvoltage);
 
// DC current measure

float DCcurrent=analogRead(DC_current_pin)*5000.0/1023;
 Serial.print("   CurrentDC : ");  
 Serial.print(DCcurrent);

 //AC current
   double Irms = emon1.calcIrms(1480);  // Calculate Irms only
  
  Serial.print("    currentAC=");        // Apparent power
  Serial.print(" ");
  Serial.println(Irms);          // Irms


  voltageAC = supplyVoltage;
  currentAC = Irms; 
  voltageDC=DCvoltage; 
  currentDC=DCcurrent; 
  Sending_To_phpmyadmindatabase(); 
  delay(6000); // interval
  Serial.println("done");
  


 }




  void Sending_To_phpmyadmindatabase()   //CONNECTING WITH MYSQL
 {
   if (client.connect(server, 80)) {
    Serial.println("connected");
    // Make a HTTP request:
    Serial.print("GET /testcode/dht4.php?ID_location=");
    client.print("GET /testcode/dht4.php?ID_location=");     //YOUR URL
    Serial.println(IDlocation);
    client.print(IDlocation);

    client.print("&voltageAC=");
    Serial.println("&voltageAC=");
    client.print(voltageAC);
    Serial.println(voltageAC);
    
    client.print("&currentAC=");
    Serial.println("&currentAC=");
    client.print(currentAC);
    Serial.println(currentAC);
    
    client.print("&voltageDC=");
    Serial.println("&voltageDC=");
    client.print(voltageDC);
    Serial.println(voltageDC);
    client.print("&currentDC=");
    Serial.println("&currentDC=");
    client.print(currentDC);
    Serial.println(currentDC);
    client.print(" ");      //SPACE BEFORE HTTP/1.1
    client.print("HTTP/1.1");
    client.println();
    client.println("Host: 172.23.174.101");
    client.println("Connection: close");
    client.println();
  } else {
    // if you didn't get a connection to the server:
    Serial.println("connection failed");
  }
 }
