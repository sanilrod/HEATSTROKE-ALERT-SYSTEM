#include "dht.h"
#define dht_apin A0

#include <dht.h>

dht DHT;

const int lm35_pin = A1;      //declare of output  pin of temp

String apiKey = "QC88N8MC44NNNVNE";  //API key


void setup() {

 
 
  Serial.begin(9600);      // PC Arduino Serial Monitor
  Serial1.begin(115200);   // Arduino to ESP Communication
  connectWiFi();           // To connect to Wifi
  
 } 

void loop() { 
  DHT.read11(dht_apin);
    
    Serial.print("Current humidity = ");
    Serial.print(DHT.humidity);
    Serial.print("%  ");
    Serial.print("temperature = ");
    Serial.print(DHT.temperature); 
    Serial.println("C  ");
    
    delay(1000);

  
  int temp_adc_val;
  float temp_val;
  temp_adc_val = analogRead(lm35_pin); // Read Temperature 
  temp_val = (temp_adc_val * 4.88); // Convert adc value to equivalent voltage 
  temp_val = (temp_val/10); // LM35 gives output of 10mv/°C 
  Serial.print("Temperature = ");
  Serial.print(temp_val);
  Serial.print(" Degree Celsius\n");
  delay(1000);
  if (temp_val >= 20)
  {
      delay (9000);
      digitalWrite(9,HIGH);
      delay(15000);
      digitalWrite(9,LOW);
  }
 


  
 
  Serial1.println("AT+CIPMUX=0\r\n");      // To Set MUX = 0
  delay(2000);                             // Wait for 2 sec

  // TCP connection 
  String cmd = "AT+CIPSTART=\"TCP\",\"";   // TCP connection with thingspeak server
  cmd += "184.106.153.149";                // IP addr of api.thingspeak.com
  cmd += "\",80\r\n\r\n";                  // Port No. = 80

  Serial1.println(cmd);                    // Display above Command
  Serial.println(cmd);                     // Send above command to Rx1, Tx1

  delay(20000);                            // Wait for 20 Sec

  if(Serial1.find("ERROR"))                // If returns error in TCP connection
  { 
    Serial.println("AT+CIPSTART error");   // Display error msg
    //return; 
  }

  // prepare GET string 
  String getStr = "GET /update?api_key=";   
  getStr += apiKey;
  getStr +="&field1=";
  getStr += temp_val; 
  getStr +="&field2=";
  getStr += DHT.humidity;
  getStr +="&field3=";
  getStr += DHT.temperature;
  getStr += "\r\n\r\n"; 

  Serial.println(getStr);                 // Display GET String 

  cmd = "AT+CIPSEND=";                    // send data length 
  cmd += String(getStr.length());
  cmd+="\r\n";

  Serial.println(cmd);                   // Display Data length 
  Serial1.println(cmd);                  // Send Data length command to Tx1, Rx1
  
  delay(20000);                          // wait for 20sec

  if(Serial1.find(">"))                    // If prompt opens verify connection with cloud
  {
    Serial.println("connected to Cloud");  // Display confirmation msg 
    Serial1.print(getStr);                 // Send GET String to Rx1, Tx1
  }
  else
  { 
    Serial1.println("AT+CIPCLOSE\r\n");    // Send Close Connection command to Rx1, Tx1
    Serial.println("AT+CIPCLOSE");         // Display Connection closed 
  } 
  
 
  delay(16000);                            // wait for 16sec

 
}
boolean connectWiFi() {               // Connect to Wifi Function
 
  Serial1.println("AT+CWMODE=1\r\n"); // Setting Mode = 1 
  delay(100);                         // wait for 100 mSec

  String cmd = "AT+CWJAP=\"";         // Connect to WiFi
  cmd += "op";                     // ssid_name
  cmd += "\",\"";
  cmd += "12345678";                  // password
  cmd += "\"\r\n";              
  
  Serial.println(cmd);                // Display Connect Wifi 
  Serial1.println(cmd);               // send Connect WiFi command to Rx1, Tx1 
  
  delay(10000);                       // wait for 10 sec

  Serial1.println("AT+CWJAP?");       // Verify Connected WiFi

  if(Serial1.find("+CWJAP"))        
  {
    Serial.println("OK, Connected to WiFi.");         // Display Confirmation msg ......first time
    return true;
  }
  else
  {
    Serial.println("Can not connect to the WiFi.");   // Display Error msg
    return false;
  }
}
