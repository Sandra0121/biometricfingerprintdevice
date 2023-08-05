/**********************************************************************************
          CONNECTION                    CONNECTION
  BIOMETRIC          ESP32       OLED DISPLAY       ESP32     TOUCH SENSOR     
   VCC               Vin             VCC            3.3V      D12    
   GND               GND             GND            GND       D13
   TX                TX2(17)         SDA            21
   RX                RX2(16)         SCL            22
 
************************************************************************************/

//*** Libraries **********
#include <WiFi.h>
#include "R30X_FPS.h"
#include <GyverOLED.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <Adafruit_Fingerprint.h>

//Definitions
#define mySerial Serial2
#define FPS_PASSWORD 0xFFFFFFFF
#define FPS_ADDRESS 0xFFFFFFFF

//Create Instances

HTTPClient http;
StaticJsonDocument<400> doc;
GyverOLED<SSH1106_128x64> oled;

//**** Create Biometric Instance ****

R30X_FPS fps = R30X_FPS (&Serial1, FPS_PASSWORD, FPS_ADDRESS);
Adafruit_Fingerprint finger = Adafruit_Fingerprint(&mySerial);

//**** Variables Declaration *****

uint16_t id;
// uint16_t id;
uint16_t fingerId;
uint16_t confidence;
char jsonOutput[128];
const byte led_ok = 25;
const byte led_fail = 33;
uint16_t fingerprint_device_id = 1;
char Lorem_ipsum[] = "....,....,.....";
uint16_t code, patient_id, fingerprint_no;



//**** Live URLS ****
String signInOut_url = "http://192.168.43.180:8081/api/authenticatemember";
String enroll_get_url = "http://192.168.43.180:8081/api/registerfingerprint";
String enroll_post_url = "http://192.168.43.180:8081/api/savefingerprint";


//*** Interrupt Stuffs ****
int a = 0;
int threshold = 25;

//*** Touch sensors functions ***
//CheckIn Touch Button Fx
 void gotTouch4() {
Serial.println("Check In");
  
    //Check In        
      a = 7;          
 }

//Enrolling Touch Button Fx
 void gotTouch5() {
   Serial.println("Enrolling");
    
   //Enroll            
   a = 6;          
  
 }

 void gotTouch6() {
  Serial.println("Check Out");
   
   // Check Out
   a = 5; 
 }

 void gotTouch7() {
 Serial.println("Cancelling");
   
   // Cancelling
   a = 4; 
 } 


//***** Initialization *****
void setup() {
  //Initialize serial communications with the PC
Serial.begin(57600);

//Initialize Led Pins
pinMode(led_ok, OUTPUT);
pinMode(led_fail, OUTPUT);


// set the data rate for the sensor serial port
finger.begin(57600); 
delay(1000); 

//Oled Display Initialization
oled.init();
oled.clear();

//Interrupt functions 
 touchAttachInterrupt(T4, gotTouch4, threshold);
 touchAttachInterrupt(T5, gotTouch5, threshold);
 touchAttachInterrupt(T6, gotTouch6, threshold);
 touchAttachInterrupt(T7, gotTouch7, threshold);

//Checking for Fingerprint Module 
Serial.print(F("Verifying password 0x"));
Serial.println(FPS_PASSWORD, HEX);

  if (finger.verifyPassword()) {
  Serial.println("Found fingerprint sensor!");
  indication(led_ok, 100, 2);
  display_text(2, 1, 2, "   NHIF");
  display_text(1, 5, 7, "Register  CheckIn/Out");
  
  Serial.println(F("Reading sensor parameters"));
  finger.getParameters();
  Serial.print(F("Capacity: ")); Serial.println(finger.capacity);
  Serial.print(F("Baud rate: ")); Serial.println(finger.baud_rate);
  Serial.print(F("Packet len: ")); Serial.println(finger.packet_len);
  Serial.print(F("Sys ID: 0x")); Serial.println(finger.system_id, HEX);
  Serial.print(F("Status: 0x")); Serial.println(finger.status_reg, HEX);
  Serial.print(F("Security level: ")); Serial.println(finger.security_level);
  Serial.print(F("Device address: ")); Serial.println(finger.device_addr, HEX); 
  } 

  else {
  Serial.println("Did not find fingerprint sensor :(");
  while (1) {
  delay(1);
  }
  indication(led_fail, 100, 2);
  }
  
//*** Justify Wifi Credentials ****
WiFi.begin("Sandy's", "stsy3182");


while (WiFi.status() != WL_CONNECTED) {
Serial.print(".");
display_text(1, 15, 5, "Initializing.....");
indication(led_ok, 50, 2);

}

//Check if  Wifi is connected
  if (WiFi.status() == WL_CONNECTED) {
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  indication(led_ok, 1000, 1);
  wifi(115, 3);
  }
 
//Interrupt functions
 touchAttachInterrupt(T4, gotTouch4, threshold);
 touchAttachInterrupt(T5, gotTouch5, threshold);
 touchAttachInterrupt(T6, gotTouch6, threshold);
 touchAttachInterrupt(T7, gotTouch7, threshold);

Serial.println();
}


// Indication functio
void indication(byte led, int delay_time, int times) {
  for (int i = 0; i < times; i++) {
  digitalWrite(led, HIGH);
  delay(delay_time);
  digitalWrite(led, LOW);
  delay(delay_time);
  }
}

//***** Main Loop *****
void loop() {
  id == 0;

while (a == 0) {

Serial.println("Select Action......");
display_text(1, 15, 5, "Select Action......");
delay(10000);
}



while (a == 4) {
Serial.println("Cancelling Successfuly");
display_text(1, 15, 5, "Cancelling..");
indication(led_fail, 500, 1);
delay(1000);
a = 0;
}

while (a == 5) {
Serial.println();
Serial.println("Place u'r Finger to CheckOut...");
display_text(1, 15, 5, "Ready to CheckOut....");
fingerId, confidence = getFingerprintID(5);


if (fingerId != 0 and confidence > 50) {
Serial.println("Ok..Processing.....");
display_text(1, 15, 5, "Ok..Processing.....");
int httpResponse = post_data(confidence, fingerId, "OUT", signInOut_url);

if (httpResponse == 200) { 
Serial.println("CheckOut..Ok.....");

Serial.println("CheckOut..Ok.....");  
display_text(1, 15, 5, "CheckOut..Ok.....");
indication(led_ok, 500, 1);
delay(1000);
}

else if (httpResponse == 204) {
indication(led_fail, 500, 1);
Serial.println("Check In First.....");
display_text(1, 15, 5, "Check In First!......");
delay(1000);
}

else { 
indication(led_fail, 50, 4);
Serial.println("Failed! Try");
display_text(1, 15, 5, "Failed!.Try.");
delay(2000);
}

}

else {
Serial.println("FIngerPrint Id does not Exist");
display_text(1, 15, 5, "Failed!.Try Again.");
display_text(1, 15, 5, "Failed!.Try Again.");
}

delay(1000);

a = 0;
}


// Starting of Enrolling Function
while (a == 6) {
Serial.println("Waiting to enroll a fingerprint!");
fingerprint_no = 0;


while (fingerprint_no == 0 and a == 6) {
indication(led_fail, 20, 4);
Serial.println("Get ID From Server...");
display_text(1, 15, 5, "Waiting 4rm Server.");
code, fingerprint_no = get_data();
Serial.println("Unpacked Response");
Serial.println("code");
Serial.println(code);
Serial.println("patient_id");
Serial.println(patient_id);
Serial.println("fingerprint_no");
Serial.println(fingerprint_no);

delay(900);

}
display_text(1, 15, 5, "Received, Id = " + String(fingerprint_no) + "....");
indication(led_ok, 500, 1);
id = fingerprint_no;
Serial.print("Requested Number From Server: ");
Serial.print(id);

if (fingerprint_no == 0 ) {// ID #0 not allowed, try again!
return;
}
Serial.println();
Serial.print("Enrolling ID #");
Serial.println(id);

while (! getFingerprintEnroll(fingerprint_no) and a == 6);

Serial.println(" Registering Successfuly");
display_text(1, 15, 5, "Registration..Ok");
indication(led_ok, 500, 1);
delay(500);

if (true) {

indication(led_ok, 500, 1);
Serial.println("Registering Successfuly");
Serial.println("Post Data Response");
Serial.println(patient_id);
Serial.println(fingerprint_no);

//Serial.println(enroll_post_url);
uint16_t httpResponse = post_enroll_data(patient_id, fingerprint_no, enroll_post_url);
if (httpResponse == 200) {
Serial.println("Success Enrollment");
display_text(1, 15, 5, "Registration..Success");
indication(led_ok, 500, 1);

}
else {
// Serial.println(httpResponse);
Serial.println("Something went wrong in Enrollment");
display_text(1, 15, 5, "Registration..Failed");
indication(led_fail, 50, 4);
}

}
else {
// Serial.println(httpResponse);  
Serial.println("Ooops! Something went wrong in Enrollment");
display_text(1, 15, 5, "Registration..Failed");
indication(led_fail, 50, 4);
}

delay(500);
a = 0;
}

///////////////////////----Check In Function---------////////////////

while (a == 7) {
//Serial.println();
Serial.println("Place u'r Finger to CheckIn...");
display_text(1, 15, 5, "Ready to CheckIn...");
fingerId, confidence = getFingerprintID(7);


if (fingerId != 0 and confidence > 50) {

// Sending Data to server
Serial.println("Ok..Processing........");
display_text(1, 15, 5, "Ok..Processing....");
uint16_t httpResponse = post_data(confidence, fingerId, "IN", signInOut_url);

if (httpResponse == 200) {

Serial.println("SignIn Success");
indication(led_ok, 500, 1);
Serial.println("CheckIn...Ok!......");
display_text(1, 15, 5, "CheckIn...Ok!....");
delay(1000);
}

else if (httpResponse == 204) {

Serial.println("Check out First!......");

indication(led_fail, 500, 1);
Serial.println("Check out First!......");
display_text(1, 15, 5, "Check out First!......");
delay(1000);
}

else {

Serial.println(httpResponse);
indication(led_fail, 500, 1);
Serial.println("Failed!.Try Again.");
display_text(1, 15, 5, "Failed!.Try Again.");
delay(2000);
}


}

else {
Serial.println("FIngerPrint Id does not Exist");
display_text(1, 15, 5, "Failed!.Try.");
}


delay(1000);
a = 0;
}

}

//Enroll function 
uint8_t getFingerprintEnroll(uint16_t id) {

uint16_t p = -1;
  Serial.println("Place u'r Finger...");
  display_text(1, 15, 5, "Place u'r Finger...");
  Serial.print("Waiting for finger ID: "); Serial.println(id);
  
  while (p != FINGERPRINT_OK and a == 6) {
    p = finger.getImage();
    switch (p) {
      case FINGERPRINT_OK:
        Serial.println("Image taken");
        Serial.println("Remove u'r Finger....");
        display_text(1, 15, 5, "Remove u'r Finger....");
        indication(led_ok, 300, 2);
        break;
      case FINGERPRINT_NOFINGER:
        Serial.println(".");

        break;
      case FINGERPRINT_PACKETRECIEVEERR:
        Serial.println("Communication error");
        display_text(1, 15, 5, "Failed.......");
        indication(led_fail, 100, 3);
        break;
      case FINGERPRINT_IMAGEFAIL:
        Serial.println("Imaging error");
        display_text(1, 15, 5, "Failed......");
        indication(led_fail, 100, 3);
        break;
      default:
        Serial.println("Unknown error");
        display_text(1, 15, 5, "Failed.....");
        indication(led_fail, 100, 3);
        break;
    }
  }

  // OK success!

  p = finger.image2Tz(1);
  switch (p) {
    case FINGERPRINT_OK:
      //Serial.println("Image converted");
      indication(led_ok, 300, 2);
      break;
    case FINGERPRINT_IMAGEMESS:
      //Serial.println("Image too messy");
      display_text(1, 15, 5, "Failed.");
      indication(led_fail, 50, 4);
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      //Serial.println("Communication error");
      display_text(1, 15, 5, "Failed....");
      indication(led_fail, 50, 4);
      return p;
    case FINGERPRINT_FEATUREFAIL:
      //Serial.println("Could not find fingerprint features");
      display_text(1, 15, 5, "Failed.....");
      indication(led_fail, 50, 4);
      return p;
    case FINGERPRINT_INVALIDIMAGE:
      //Serial.println("Could not find fingerprint features");
      display_text(1, 15, 5, "Failed.....");
      indication(led_fail, 50, 4);
      return p;
    default:
      //Serial.println("Unknown error");
      display_text(1, 15, 5, "Failed....");
      indication(led_fail, 50, 4);
      return p;
  }

  //Serial.println("Remove finger");
  delay(1000);
  p = 0;
  while (p != FINGERPRINT_NOFINGER and a == 6) {
    p = finger.getImage();
  }
  //Serial.print("ID "); //Serial.println(id);
  p = -1;
  Serial.println("Place same finger again");
  display_text(1, 15, 5, "Place u'r Finger.....");
  while (p != FINGERPRINT_OK and a == 6) {
    p = finger.getImage();
    switch (p) {
      case FINGERPRINT_OK:
        Serial.println("Image taken");
        Serial.println("Remove u'r Finger....");
        display_text(1, 15, 5, "Remove u'r Finger....");
        indication(led_ok, 300, 2);
        break;
      case FINGERPRINT_NOFINGER:
        //Serial.print(".");
        //      digitalWrite(led_ok, HIGH);
        break;
      case FINGERPRINT_PACKETRECIEVEERR:
        Serial.println("Communication error");
        display_text(1, 15, 5, "Failed....");
        indication(led_fail, 50, 3);
        break;
      case FINGERPRINT_IMAGEFAIL:
        //Serial.println("Imaging error");
        display_text(1, 15, 5, "Failed....");
        indication(led_fail, 50, 3);
        break;
      default:
        //Serial.println("Unknown error");
        display_text(1, 15, 5, "Failed....");
        indication(led_fail, 50, 3);
        break;
    }
  }

  // OK success!

  p = finger.image2Tz(2);
  switch (p) {
    case FINGERPRINT_OK:
      //Serial.println("Image converted");
      indication(led_ok, 300, 2);
      break;
    case FINGERPRINT_IMAGEMESS:
      //Serial.println("Image too messy");
      display_text(1, 15, 5, "Failed.....");
      indication(led_fail, 50, 4);
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      //Serial.println("Communication error");
      display_text(1, 15, 5, "Failed......");
      indication(led_fail, 50, 4);
      return p;
    case FINGERPRINT_FEATUREFAIL:
      //Serial.println("Could not find fingerprint features");
      display_text(1, 15, 5, "Failed.....");
      indication(led_fail, 50, 4);
      return p;
    case FINGERPRINT_INVALIDIMAGE:
      //Serial.println("Could not find fingerprint features");
      display_text(1, 15, 5, "Failed.....");
      indication(led_fail, 50, 4);
      return p;
    default:
      //Serial.println("Unknown error");
      display_text(1, 15, 5, "Failed......");
      indication(led_fail, 50, 4);
      return p;
  }

  // OK converted!

  p = finger.createModel();
  if (p == FINGERPRINT_OK) {
    Serial.println("Prints matched!");
    display_text(1, 15, 5, "Prints Matched....");
    indication(led_ok, 300, 2);
  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
    //Serial.println("Communication error");
    display_text(1, 15, 5, "Failed.");
    indication(led_fail, 1000, 1);
    return p;
  } else if (p == FINGERPRINT_ENROLLMISMATCH) {
    //Serial.println("Fingerprints did not match");
    display_text(1, 15, 5, "Prints didn't Match...");
    indication(led_fail, 1000, 1);
    return p;
  } else {
    //Serial.println("Unknown error");
    display_text(1, 15, 5, "Failed.....");
    indication(led_fail, 1000, 1);
    return p;
  }

  Serial.println("ID "); Serial.println(id);
  p = finger.storeModel(id);
  if (p == FINGERPRINT_OK) {
    //Serial.println("Stored!");
    display_text(1, 15, 5, "Prints Stored....");
    indication(led_ok, 2000, 2);
  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
    //Serial.println("Communication error");
    display_text(1, 15, 5, "Failed.....");
    indication(led_ok, 500, 2);
    return p;
  } else if (p == FINGERPRINT_BADLOCATION) {
    //Serial.println("Could not store in that location");
    display_text(1, 15, 5, "Failed.");
    indication(led_ok, 1000, 2);
    return p;
  } else if (p == FINGERPRINT_FLASHERR) {
    //Serial.println("Error writing to flash");
    display_text(1, 15, 5, "Failed.....");
    indication(led_ok, 1000, 2);
    return p;
  } else {
    //Serial.println("Unknown error");
    display_text(1, 15, 5, "Failed......");
    indication(led_ok, 1000, 2);
    return p;
  }

return true;
}
//**** End of Enroll Function *****

//Start Capturing FingerTemplates
uint8_t getFingerprintID(uint16_t aa) 
{
uint16_t p = -1;
while (p != FINGERPRINT_OK and a == aa) {
p = finger.getImage();
switch (p) {
case FINGERPRINT_OK:

break;
case FINGERPRINT_NOFINGER:

break;
case FINGERPRINT_PACKETRECIEVEERR:

break;
case FINGERPRINT_IMAGEFAIL:

break;
default:

break;
}

}

// OK success!

p = finger.image2Tz();
switch (p) {
case FINGERPRINT_OK:
//Serial.println("Image converted");

break;
case FINGERPRINT_IMAGEMESS:
//Serial.println("Image too messy");

return p;
case FINGERPRINT_PACKETRECIEVEERR:
//Serial.println("Communication error");

return p;
case FINGERPRINT_FEATUREFAIL:
//Serial.println("Could not find fingerprint features");

return p;
case FINGERPRINT_INVALIDIMAGE:
//Serial.println("Could not find fingerprint features");

return p;
default:
//Serial.println("Unknown error");

return p;
}

// OK converted!
p = finger.fingerSearch();
if (p == FINGERPRINT_OK) {
//Serial.println("Found a print match!");//Serial.print(p);//Serial.println("");
indication(led_ok, 1000, 1);
} else if (p == FINGERPRINT_PACKETRECIEVEERR) {
//Serial.println("Communication error");
indication(led_fail, 50, 4);
return p, p;
} else if (p == FINGERPRINT_NOTFOUND) {
//Serial.println("Did not find a match");
indication(led_fail, 50, 4);
return p, p;
} else {
//Serial.println("Unknown error");
indication(led_fail, 50, 4);
return p, p;

}

fingerId = finger.fingerID;
return fingerId, finger.confidence;
}
/////////////////////////////-----------End Capturing FingerTemplates-------/////////////

// API Get data Function
int get_data()
{
code = 0;
patient_id = 0;
fingerprint_no = 0;
if ((WiFi.status() == WL_CONNECTED)) {
http.begin(enroll_get_url);
http.addHeader("Content-Type", "application/json");

uint16_t httpCode = http.POST("");
String payload = http.getString();
deserializeJson(doc, payload);

//Justify the variable obtained
code = doc["code"];
patient_id = doc["patient_id"];
fingerprint_no = doc["fingerprint_no"];

http.end();
}
return code, patient_id, fingerprint_no;
}

//Function to Post Data
int post_data(uint16_t confidence, uint16_t fingerId, String fingerprint_data, String url) {

uint16_t httpCode;

  if ((WiFi.status() == WL_CONNECTED)) {
  http.begin(url);
  http.addHeader("Content-Type", "application/json");
  JsonObject object = doc.to<JsonObject>();
  Serial.println("Sending data to Server");
  
  object["fingerprint_no"] = fingerId; 
  object["request_flag"] = fingerprint_data;
  serializeJson(doc, jsonOutput);
  Serial.println(fingerprint_no);
  Serial.println(fingerId); 
  Serial.println(String(jsonOutput));
  
  httpCode = http.POST(String(jsonOutput));
  Serial.println(httpCode);
  
  http.end();
  }

return httpCode;
}


//Function to Post enrollment data
int post_enroll_data(uint16_t patient_id, uint16_t fingerprint_no, String url) {

uint16_t httpCode;
  if ((WiFi.status() == WL_CONNECTED)) {   
  http.begin(url);
  http.addHeader("Content-Type", "application/json"); 
  JsonObject object = doc.to<JsonObject>();
  Serial.println("Sending enroll Post data to Server");

  //Justify the data to be sent
  object["patient_id"] = patient_id;
  object["fingerprint_no"] = fingerprint_no;
  
  serializeJson(doc, jsonOutput);
  
  Serial.println(fingerprint_no);
  
  String data_to_post = String(jsonOutput);
  Serial.println(data_to_post);
  Serial.println(data_to_post);
  Serial.println("String(jsonOutput)");    
  httpCode = http.POST(data_to_post);   
  Serial.println(httpCode);
  http.end();
}

return httpCode;
}

//Wifi Function
void wifi(int x, int y) {
oled.setCursorXY(x, y);
oled.drawByte(0b00111100);

oled.setCursorXY(x + 2, y - 1);
oled.drawByte(0b00111100);
oled.setCursorXY(x + 2, y);
oled.drawByte(0b00111100);

oled.setCursorXY(x + 4, y - 2);
oled.drawByte(0b00111100);
oled.setCursorXY(x + 4, y - 1);
oled.drawByte(0b00111100);
oled.setCursorXY(x + 4, y);
oled.drawByte(0b00111100);

oled.setCursorXY(x + 6, y - 3);
oled.drawByte(0b00111100);
oled.setCursorXY(x + 6, y - 2);
oled.drawByte(0b00111100);
oled.setCursorXY(x + 6, y - 1);
oled.drawByte(0b00111100);
oled.setCursorXY(x + 6, y);
oled.drawByte(0b00111100);
oled.update();
}

//Oled Display Function
void display_text(int scale, int x, int y, String text) {
oled.setScale(scale);
oled.setCursor(x, y);
oled.print(text);
oled.update();
}

//Oled Display Function
void display_txt(int scale, int x, int y, String text) {
oled.setScale(scale);
oled.setCursor(x, y);
oled.print(text);

}

void scrollText() {
oled.clear();
uint32_t tmr = millis();
oled.autoPrintln(false);
int val = 125;

for (;;) {
oled.clear();   
oled.setScale(1);
oled.setCursor(val, 0);
oled.print(Lorem_ipsum);
oled.setCursor(val + 2, 0);
oled.print(Lorem_ipsum);
oled.update();
val--;
if (millis() - tmr > 1500) return;
}
}
