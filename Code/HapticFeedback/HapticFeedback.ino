/*  Joshua Liska
 *  Haptic Feedback System Version 2
 *  ------------------------------------------------------------------------------------------
 *  References:
 *  ------------------------------------------------------------------------------------------
 *  Use Case:
 *    The user will interact with the touch sensor.
 *    The touch sensor will record position and pressure and then relay this to an array.
 *    This data will then be fed into a mongodb server sitting on an AWS EC2 server.
 *    The data can then be grabbed by another device and recreate the system feedback.
 */
#include <WiFi.h>
#include <HTTPClient.h>

//Your wifi ssid and password
const char* ssid = "";
const char* password = "";

//Your Domain name with URL path or IP address with path
const char* serverName = "";

String response;

//Pin decleration
const int rows[9] = {13,12,14,27,26,25,33,32,35};
const int cols[6] = {15,2,4,16,17,5};

float pressureMatrix[9][6];
float calibrationMatrix[9][6];

//Timers and counters
unsigned long printMatrixTimer = millis();
const int delayTime = 1000;
const int sampleNum = 100;

/*  void setup()
 *    sets up the pins, initializes the webserver communication, and calibrates the calibration matrix
 */
void setup() {
  Serial.begin(115200);
  Serial.print("Starting program!");
  for(int i=0;i<9;i++) pinMode(rows[i], INPUT);
  for(int i=0;i<6;i++) pinMode(cols[i], OUTPUT);

  WiFi.begin(ssid, password);
  Serial.println("Connecting");
  while(WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to WiFi network with IP Address: ");
  Serial.println(WiFi.localIP());
  delay(500);

  //Calibrating the matrix
  for(int i=0;i<sampleNum;i++){
    readMatrix(true);
  }
  printMatrix(true);
  delay(100);
}

/*  void loop()
 *    Every 100 ms it runs the pressure matrix, returning a refresh rate of 10 "frames" per second.
 *    Also returns the array as a serial output and sends the information to the cloud server.
 */
void loop() {
  if(millis()-printMatrixTimer >= delayTime){
    readMatrix(false);
    printMatrix(false);
    Serial.println(toUrl());
    printMatrixTimer = millis();
  }
}

/*  void printMatrix()
 *    prints the matrix output to show the output values
 */
void printMatrix(bool startup){
  Serial.print("Url: ");Serial.println(toUrl());
  Serial.println("--------------------------------------------------------");
  for(int i=0;i<9;i++){
    for(int j=0;j<6;j++){
      Serial.print("[ ");
      if(!startup) Serial.print(pressureMatrix[i][j]);
      else Serial.print(calibrationMatrix[i][j]);
      Serial.print(" ]");
    }
    Serial.println();
  }
  Serial.println("--------------------------------------------------------");
}

/*  void readMatrix()
 *    reads from the pressure matrix, compares to the initial calibration matrix and then stores the difference
 */

int preI = 0, preJ = 0;

void readMatrix(bool startup){
  int analogIn;
  for(int i=0;i<9;i++){
    digitalWrite(rows[preI], LOW);
    digitalWrite(rows[i], HIGH);
    preI = i;
    for(int j=0;j<6;j++){
      digitalWrite(cols[preJ], LOW);
      digitalWrite(cols[j], HIGH);
      preJ = j;
      analogIn = analogRead(rows[i]);
      if(!startup){
        if((analogIn - calibrationMatrix[i][j])<=0) pressureMatrix[i][j] = 0.0;
        else pressureMatrix[i][j] = float(analogIn);
      }
      else calibrationMatrix[i][j] = calibrationMatrix[i][j] + (float(analogIn)/sampleNum);
    }
  }
}

/*  void httpGETRequest()
 *    sets up communication with the AWS server
 */
String httpGETRequest(const char* serverName) {
  HTTPClient http;
    
  // Your IP address with path or Domain name with URL path 
  http.begin(serverName);
  
  // Send HTTP POST request
  int httpResponseCode = http.GET();
  
  String payload = "{}"; 
  
  if (httpResponseCode>0) {
    Serial.print("HTTP Response code: ");
    Serial.println(httpResponseCode);
    payload = http.getString();
  }
  else {
    Serial.print("Error code: ");
    Serial.println(httpResponseCode);
  }
  // Free resources
  http.end();

  return payload;
}

/*  void toUrl()
 *    takes the array data taken from the pressure sensor and parses it into a readable format for the AWS website to handle
 */
String toUrl(){
  String url = serverName;
  for(int i=0;i<9;i++){
    for(int j=0;j<6;j++){
      if(j<5) url = url + pressureMatrix[i][j] + " ";
      else url = url + pressureMatrix[i][j];
    }
    if(i<8) url = url + ",";
  }
  return url;
}
