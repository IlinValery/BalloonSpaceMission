

#include "Configs.h" // all defined variables
#include "src/Utils.h"


/*
String convertIntToBeautyString(int integer, int signCount)
*/

//---Temperature DS18B20----
#include <OneWire.h>
OneWire ds(10);
double tempDS18B20 = 0;
byte addrTempSensor[8];
byte typeTempSensor;
unsigned long timeFromActivation = 0;
const int intervalTempSensor = 1000;
bool isTempSensorActive = false;
bool goodStatusTempSensor = false;

#include "src/TemperatureDS18B20.h"
/*
bool initDS18B20(byte* addressTempDevice, byte& typeOfSensor);
void updateDS18B20(double& temperature, bool& isActive, unsigned long& prevActivation, const int& interval, const byte* addressTempDevice, const byte& typeOfSensor);
*/

//----


//---Barometr---
#include <Wire.h>
#include <SPI.h>
#include <MS5611.h>
MS5611 ms5611;
double referencePressure = 0;
double tempBarometr = 0;
double pressureBarometr = 0;
double relativeAltitudeBarometr = 0;

#include "src/BarometerController.h"
/*
double initBarometr();
void updateBarometr(double& tempBarometr, double& pressureBarometr, double& relativeAltitudeBarometr, const double& referencePressure);
*/
//-----------------------


//---Timer---

int h = 0, m = 0, s = 0, ms = 0;
#include "src/TimeSeparator.h"
/*
  void updateTimeSeparator(int& h, int& m, int& s, int& ms);
*/

//-------------

//---RTC---
#include <ThreeWire.h>  
#include <RtcDS1302.h>
ThreeWire myWire(DATA_PIN,CLK_PIN,RST_PIN); // IO, SCLK, CE (DATA/CLK/RST) 
RtcDS1302<ThreeWire> Rtc(myWire); // start init RTC
bool RTCIsAvalable = false;
#include "src/RTCController.h"
/*
  bool initRTC();
*/
//---------

//---GPS---
#include <TinyGPS.h>
//long   lat,lon; // create variable for latitude and longitude object
float lat = 0,lon = 0;
TinyGPS gps; // create gps object

#include "src/GPSController.h"
/*
  void updateGPS(float& lat, float& lon);
*/
//----------


//---Battery---
float batteryStatus = 0.0;  
#include "src/BatteryChecker.h"
/*
  void updateBatteryStatus(float& battery);
*/

//-------------


//---SD---
#include <MsTimer2.h> //interrupt timer lib
#include <SD.h>

String logStringData = "";
String fileName = "data.csv";
const String headToFile = String("h") + String(":") + String("m") + String(":") + String("s") + String(".") + String("ms") + String(FILE_SEPARATOR) +
                        String("lat") + String(FILE_SEPARATOR) +
                        String("lon") + String(FILE_SEPARATOR) +
                        String("tempBarometr") + String(FILE_SEPARATOR) +
                        String("pressureBarometr") + String(FILE_SEPARATOR) +
                        String("altitudeBarometr") + String(FILE_SEPARATOR) +
                        String("tempDS18B20") + String(FILE_SEPARATOR) +
                        String("batteryStatus");


#include "src/SDController.h"
/*
    void writeDataToSD(String filename);
    void recorderHandler();
    void initRecorderSDCard();
    String getDateTimeForFile(const RtcDateTime& dt);
    int getFileCount( String dir );
    String initSDCard(const String& headToFile);
*/
//-----------------------

          
//---RetrievalMode----
#include "src/CustomDateTime.h"
#include <RtcDateTime.h>
#include <EEPROM.h>

bool retrievalStatus = false;
const int eeAddress = 0;

RtcDateTime savedRtcDateTime;
DateTime criticalDateTime = {
  CRITICAL_DATE,
  CRITICAL_TIME
};
RtcDateTime criticalRtcDateTime = RtcDateTime(criticalDateTime.date, criticalDateTime.time);

#include "src/RetrievalDateTime.h"
/*
  RtcDateTime initSavedDateTime();
  String getStringData(const RtcDateTime& dt);
  String getStringTime(const RtcDateTime& dt);
  DateTime getDateTimeForRecording(const DateTime& critical);
*/

#include "src/RetrievalSensors.h"
/*
  void initRetrievalSensors();
  void updateRetrievalSensors(const bool& retrievalStatus);
  void updateRetrievalStatus(const RtcDateTime& critical, const RtcDateTime& loaded, bool& retrievalStatus);
*/


//---------------------

//---Button---
#include <GyverButton.h>
GButton controllerButton(BTN_PIN);
bool buttonWasActivated = false;


#include "src/RetrievalButton.h"
/*
  void initSavingButton();
*/
void updateSavingButton(bool& buttonStatus){
  controllerButton.tick();
  
  // TODO
  digitalWrite(LED_PIN_GREEN, buttonStatus);
  digitalWrite(LED_PIN_RED, !buttonStatus);

  // Serial.print(getDateTimeForRecording(criticalDateTime).date);
  // Serial.print(" ");
  // Serial.println(getDateTimeForRecording(criticalDateTime).time);

  if (!buttonStatus){
    if (controllerButton.isTriple()){
      buttonStatus = true;
      tone(PIEZO_PIN, PIEZO_SIGNAL, PIEZO_DURATION);
      EEPROM.put(eeAddress, getDateTimeForRecording(criticalDateTime));
      Serial.println("New data saved to EEPROM");
      savedRtcDateTime = initSavedDateTime();
      retrievalStatus = false;
      initAllSensors();
    }
  }

}
//-----------------

//----ProblemChecker----------
  // indexes:
byte temperatureProblem = 1; // 1 - 001
byte barometerPrbolem = 2; // 2 - 010 
byte batteryProblem = 3; // 3 - 011
byte GPSProblem = 4; // 4 - 100 
byte SDCardProblem = 5; // 5 - 101
bool problemsOnSD = true;
byte allWorks = 7; // 7 - 111

byte delayIndex = 0;

bool problemMatrix[8];
byte pinIndexes[3] = {LED_STATUS1, LED_STATUS2, LED_STATUS3};

void checkTemperature(bool* matrix, const byte& sensor){
  if (tempDS18B20 == 0.0){
    matrix[sensor] = true;
  } else {
    matrix[sensor] = false;
  }
}

void checkBarometer(bool* matrix, const byte& sensor){
  if (pressureBarometr == 0){
    matrix[sensor] = true;
  } else {
    matrix[sensor] = false;
  }
}

void checkBattery(bool* matrix, const byte& sensor){
  if (batteryStatus == 0.0){
    matrix[sensor] = true;
  } else {
    matrix[sensor] = false;
  }
}

void checkGPS(bool* matrix, const byte& sensor){
  if ((lat == 0.0) || (lon == 0.0)){
    matrix[sensor] = true;
  } else {
    matrix[sensor] = false;
  }
}



//-------------------------------

void initAllSensors(){
  savedRtcDateTime = initSavedDateTime(); delay(100);

  RTCIsAvalable = initRTC(); delay(100);

  initSavingButton();

  referencePressure = initBarometr();  delay(100);

  goodStatusTempSensor = initDS18B20(addrTempSensor, typeTempSensor); delay(100);

  fileName = initSDCard(headToFile); delay(100);

  initRecorderSDCard(); delay(100);

  initRetrievalSensors();
}

void updateNormalModeSensors(){
    updateTimeSeparator(h, m, s, ms);

    updateGPS(lat, lon);

    updateBarometr(tempBarometr, pressureBarometr, relativeAltitudeBarometr, referencePressure);

    updateDS18B20(tempDS18B20, isTempSensorActive, timeFromActivation, intervalTempSensor, addrTempSensor, typeTempSensor);

    updateBatteryStatus(batteryStatus);

}




void sendToNano(const bool& retrievalStatus){
  char sequence = 'N';
  if (retrievalStatus) {
    sequence = 'R';
	NANO_SERIAL.print(sequence);
  } 
}



void updateRetrievalModeSensors(){
  updateSavingButton(buttonWasActivated);

  updateRetrievalStatus(criticalRtcDateTime, savedRtcDateTime, retrievalStatus);

  updateRetrievalSensors(retrievalStatus);

  sendToNano(retrievalStatus);
}



void setup() {


  Serial.begin(9600);
  
  GPS_SERIAL.begin(9600);

  NANO_SERIAL.begin(9600);

  initAllSensors();

}

void loop() {

  updateRetrievalModeSensors();
  if (!retrievalStatus){
    updateNormalModeSensors();
  } else {

  }

}
