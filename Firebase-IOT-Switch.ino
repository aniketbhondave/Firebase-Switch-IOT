#include "FirebaseESP8266.h"
#include <ESP8266WiFi.h>

#include <DNSServer.h>            //Local DNS Server used for redirecting all requests to the configuration portal
#include <ESP8266WebServer.h>     //Local WebServer used to serve the configuration portal
#include <WiFiManager.h>

#define FIREBASE_HOST "homelights-6c78c.firebaseio.com"  //Change to your Firebase RTDB project ID e.g. Your_Project_ID.firebaseio.com
#define FIREBASE_AUTH "ywt7thpWCpCSsp0m8QUsdoC2b1b0nnsEZEp2W3k3" //Change to your Firebase RTDB secret password
#define WIFI_SSID "Refrain"                                               //your WiFi SSID for which yout NodeMCU connects
#define WIFI_PASSWORD "lemontechnosys9762a" 

//Define Firebase Data objects
FirebaseData firebaseData1;
FirebaseData firebaseData2;




FirebaseJson updateData;
FirebaseJson json;

#define pin1  5
#define pin2  4
#define pin3  14
#define pin4  12


#define button1  0 //working
#define button2  3 //working
#define button3  13 //working
#define button4  10 

bool swState = false;
String path = "/Buttons";
String statusPath = "/Status/STATUS";
String childPath[4] = {"/Trigger1","/Trigger2","/Trigger3","/Trigger4"};
size_t childPathSize = 4;
String nodeID1 = "Trigger1";
String nodeID2 = "Trigger2"; 
String nodeID3 = "Trigger3"; 
String nodeID4 = "Trigger4"; 
int contact = 0;

int buttonState1 = 0;
int buttonState2 = 0;
int buttonState3 = 0;
int buttonState4 = 0;

String previousManualButton1State = "HIGH";
String previousManualButton2State = "HIGH";
String previousManualButton3State = "HIGH";
String previousManualButton4State = "HIGH";

void setup()
{
     Serial.begin(115200);
    //WifiManager Variables
    WiFiManager wifiManager;
    wifiManager.autoConnect("Switch Board Wifi");
    wifiManager.setAPCallback(configModeCallback);
    



    //pinMode(ledPin, OUTPUT);
    pinMode(pin1, OUTPUT);
    pinMode(pin2, OUTPUT);
    pinMode(pin3, OUTPUT);
    pinMode(pin4, OUTPUT);
    //pinMode(LED, OUTPUT);

    
    pinMode(button1, INPUT);
    pinMode(button2, INPUT);
    pinMode(button3, INPUT);
    pinMode(button4, INPUT);


    Serial.println();
    Serial.println();

   digitalWrite(pin1, HIGH);
   digitalWrite(pin2, HIGH);
   digitalWrite(pin3, HIGH);
   digitalWrite(pin4, HIGH);
   

    
//    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
//    Serial.print("Connecting to Wi-Fi");
//    while (WiFi.status() != WL_CONNECTED)
//    {
//        Serial.print(".");
//        delay(300);
//    }
//    Serial.println();
//    Serial.print("Connected with IP: ");
//    Serial.println(WiFi.localIP());
//    Serial.println();

    Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
    Firebase.reconnectWiFi(true);


    
    
//  if (!Firebase.beginStream(firebaseData1, path + "/" + nodeID1))
//    {
//        Serial.println("Could not begin stream 1");
//        Serial.println("REASON: " + firebaseData1.errorReason());
//        Serial.println();
//    }

Firebase.setMultiPathStreamCallback(firebaseData1, streamCallback, streamTimeoutCallback);

  if (!Firebase.beginMultiPathStream(firebaseData1, path, childPath, childPathSize))
  {
    Serial.println("------------------------------------");
    Serial.println("Can't begin stream connection...");
    Serial.println("REASON: " + firebaseData1.errorReason());
    Serial.println("------------------------------------");
    Serial.println();
  }

  

}

void configModeCallback (WiFiManager *myWiFiManager) {
  Serial.println("Entered config mode");
  Serial.println(WiFi.softAPIP());

  Serial.println(myWiFiManager->getConfigPortalSSID());
}


void streamCallback(MultiPathStreamData stream)
{
  Serial.println();
  Serial.println("Stream Callback");

      if(firebaseData1.dataPath() == "/Trigger1")
      {
        checkStatus(firebaseData1.dataPath(),pin1);
      }
      else if(firebaseData1.dataPath() == "/Trigger2")
      {
        checkStatus(firebaseData1.dataPath(),pin2);
      }
      else if(firebaseData1.dataPath() == "/Trigger3")
      {
        checkStatus(firebaseData1.dataPath(),pin3);
      }
      else if(firebaseData1.dataPath() == "/Trigger4")
      {
        checkStatus(firebaseData1.dataPath(),pin4);
      }

  size_t numChild = sizeof(childPath)/sizeof(childPath[0]);

  for(size_t i = 0;i< numChild;i++)
  {
    if (stream.get(childPath[i]))
    {
      Serial.println("path: " + stream.dataPath + ", type: " + stream.type + ", value: " + stream.value);
    }
  }
}



void streamTimeoutCallback(bool timeout)
{
  if (timeout)
  {
    
    Serial.println();
    Serial.println("Stream timeout, resume streaming...");
    //setTrigger("T1");
    Serial.println();
  }
}

void checkStatus(String triggerName, int pinNumber)
{
  String  triggerPath = triggerName;
  contact = digitalRead(pinNumber);
  
  Serial.println(contact);
  Serial.println("Check Pin status");
  delay(1000);
   
  if(firebaseData1.stringData() == "ON")
          {
            //1-OFF
            //0-ON
            if (contact == 1) //Check first if switch is ON or OFF - if OFF then only try to ON
            {
              digitalWrite(pinNumber, LOW) ;
              //changeTempPinState(pinNumber,"LOW");
              delay(2000);
              if (contact == 0) //check the status after 1 sec and if not set retry to set it low
              {
                Serial.println("Retrying to turn it ON"); 
                Serial.println(contact);
                digitalWrite(pinNumber, LOW) ;
               // changeTempPinState(pinNumber,"LOW");
                delay(1000);
              }
            }
            else
            {}      
          }
          else
          {
            if (contact == 0)//if ON then
            {
              digitalWrite(pinNumber, HIGH);
              //changeTempPinState(pinNumber,"HIGH");
               delay(2000);
               if (contact == 1) //check the status after 1 sec and if not set retry to set it HIGH
               {
                Serial.println("Retrying to turn it OFF"); 
                Serial.println(contact);
                digitalWrite(pinNumber, HIGH) ;
              //  changeTempPinState(pinNumber,"HIGH");
                delay(1000);
                
               }else
               {}
              }
               
          }

  
  }

  void changeTempPinState(int pinNo,String state) //State is - LOW OR HIGH
  {
    switch (pinNo) 
               {
                   case pin1:
                   previousManualButton1State = state;
                   break;
                   
                   case pin2:
                   previousManualButton2State = state;
                   break;
                   
                   case pin3:
                   previousManualButton3State = state;
                   break;
                   
                   case pin4:
                   previousManualButton4State = state;
                   break;
                    
                   default:
                    // statements
                   break;
               }
  }

  void manualButtonTrigger(int setPin,String triggerPath,String state)
  {

    if (state == "OFF")
      {
      Serial.println("STATUS OFF");
      digitalWrite(setPin, HIGH);
      }else if (state == "ON")
      {
       Serial.println("STATUS ON");
       digitalWrite(setPin, LOW); 
      }
        
   updateData.set(triggerPath,state);
    if (Firebase.updateNode(firebaseData1,"/Buttons", updateData))
    {
      Serial.println();
      if (state == "OFF")
      {
      Serial.println("STATUS OFF");
      //digitalWrite(setPin, HIGH);
      }else if (state == "ON")
      {
       Serial.println("STATUS ON");
       //digitalWrite(setPin, LOW); 
      }
    }
    else
    {
      Serial.println("FAILED to set Trigger");
      Serial.println("REASON: " + firebaseData2.errorReason());
      Serial.println("------------------------------------");
      Serial.println();
    }
  }


void loop()
{

   buttonState1 = digitalRead(button1); 
   buttonState2 = digitalRead(button2);
   buttonState3 = digitalRead(button3);
   buttonState4 = digitalRead(button4);
   
//Serial.println("Button 1");
//Serial.println("Digital Read 1:" + buttonState1);
//Serial.println(previousManualButton1State);
//Serial.println("Button 2");
//Serial.println("Digital Read 2:" + buttonState2);
//Serial.println(previousManualButton2State);
//Serial.println("Button 3");
//Serial.println("Digital Read 3:" + buttonState3);
//Serial.println(previousManualButton3State);
//Serial.println("Button 4");
//Serial.println("Digital Read 4:" + buttonState4);
//Serial.println(previousManualButton4State);
   

if(buttonState1 == HIGH)
{
  if (previousManualButton1State == "LOW")
  {
   previousManualButton1State = "HIGH";
   manualButtonTrigger(pin1,"/Trigger1","OFF");
   delay(500); 
  }
}else if (buttonState1 == LOW)
{
  if (previousManualButton1State == "HIGH")
  {
   previousManualButton1State = "LOW";
   manualButtonTrigger(pin1,"/Trigger1","ON");
   delay(500); 
  }
}


if(buttonState2 == HIGH)
{
  if (previousManualButton1State == "LOW")
  {
   previousManualButton2State = "HIGH";
   manualButtonTrigger(pin2,"/Trigger2","OFF");
   delay(500); 
  }
}else if (buttonState2 == LOW)
{
  if (previousManualButton2State == "HIGH")
  {
   previousManualButton2State = "LOW";
   manualButtonTrigger(pin2,"/Trigger2","ON");
   delay(500); 
  }
}



if(buttonState3 == HIGH)
{
  if (previousManualButton3State == "LOW")
  {
   previousManualButton3State = "HIGH";
   manualButtonTrigger(pin3,"/Trigger3","OFF");
   
   delay(500); 
  }
}else if (buttonState3 == LOW)
{
  if (previousManualButton3State == "HIGH")
  {
   
   previousManualButton3State = "LOW";
   manualButtonTrigger(pin3,"/Trigger3","ON");
   delay(500); 
  }
}



if(buttonState4 == HIGH)
{
  if (previousManualButton4State == "LOW")
  {
   previousManualButton4State = "HIGH";
   manualButtonTrigger(pin4,"/Trigger4","OFF");
   delay(500); 
  }
}else if (buttonState4 == LOW)
{
  if (previousManualButton4State == "HIGH")
  {
   previousManualButton4State = "LOW";
   manualButtonTrigger(pin4,"/Trigger4","ON");
   delay(500); 
  }
}
  delay(1000);
  
    if (!Firebase.readStream(firebaseData1))
    {
        Serial.println();
        Serial.println("Can't read stream data");
        Serial.println("REASON: " + firebaseData1.errorReason());
        //setTrigger("REASON: " + firebaseData1.errorReason());
        Serial.println();
    }

    if (firebaseData1.streamTimeout())
    {
        Serial.println();
       
        Serial.println("Stream timeout, resume streaming...");
        Serial.println();
    }

    if (firebaseData1.streamAvailable())
    {
      Serial.println("Currently Connected");

    if (firebaseData1.dataType() == "string")
    {
      Serial.println(firebaseData1.dataPath());

      if(firebaseData1.dataPath() == "/Trigger1")
      {
        checkStatus(firebaseData1.dataPath(),pin1);
      }
      else if(firebaseData1.dataPath() == "/Trigger2")
      {
        checkStatus(firebaseData1.dataPath(),pin2);
      }
      else if(firebaseData1.dataPath() == "/Trigger3")
      {
        checkStatus(firebaseData1.dataPath(),pin3);
      }
      else if(firebaseData1.dataPath() == "/Trigger4")
      {
        checkStatus(firebaseData1.dataPath(),pin4);
      }    
    }

        
    }
}
