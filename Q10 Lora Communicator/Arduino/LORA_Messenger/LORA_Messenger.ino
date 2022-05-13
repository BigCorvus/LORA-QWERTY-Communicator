/*********************************************************************
  Code Base for the ultimate LORA QWERTY COMMUNICATOR project
  The Feather nRF52840 bootloader was installed (the device uses the same button an the same 2 LEDs)
  However, the variant.h and variant.cpp files of the feather board have been heavily modified.
  The pin numbering now just goes from 0 to 47 (P0.00 to P1.15) which is much easier to work with.

  Already working:
  -LS027B7DH01 400x240 memory LCD
  -Blackberry Q10 keyboard with backlight
  -BLE Stuff
  -BQ27441 Lithium Fuel Gauge
  -Buzzer
  -vibration motor
  -SX1262 TX and RX
  -BME280
  -DS3231M RTC
  -MPU9250 9-DOF IMU (hardware bug had to be corrected - switched I2C address to 0x69, else it collides with the DS3231 (0x68))
  -GD25Q16CE QSPI flash (works with mass storage TinyUSB test sketch)
  -GPS via tinyGPSPlus
  
  ToDo:
  -SD card - 
  
*********************************************************************/
#include <Arduino.h>
#include <Adafruit_TinyUSB.h> // for Serial
#include <Adafruit_GFX.h>
#include <Adafruit_SharpMem.h>
#include <SPI.h>
#include <Wire.h>
#include <RadioLib.h>
#include <SparkFunBQ27441.h>
#include <bluefruit.h>
#include "SparkFunBME280.h"
#include <DS3231M.h> //Written by Arnd <Zanshin_Github@sv-zanshin.com> / https://www.github.com/SV-Zanshin
//#include "MPU9250.h"
#include <MPU9250_WE.h>
#define MPU9250_ADDR 0x69 //changed it because the DS3231M also has 0x68!!
#include <TinyGPSPlus.h>

#include <Fonts/FreeSans9pt7b.h> 
#include <Fonts/FreeSans12pt7b.h>
#include <Fonts/FreeSans18pt7b.h>
#include <Fonts/FreeSans24pt7b.h>
//display.setFont(&FreeSans12pt7b);
#define BLACK 0
#define WHITE 1
#define TIME_ZONE 2 //what you need to add to UTC time

//#define SPI_MISO         15
//#define SPI_MOSI         13
//#define SPI_SCK          14
//Q10 keyboard defines
#define col1    16
#define col2    33
#define col3    17
#define col4    39
#define col5    24

#define row1    22
#define row2    35
#define row3    37
#define row4    20
#define row5    46
#define row6    44
#define row7    45
//PIN defines
#define IMU_INT       9
#define RTC_INT       40
#define PIN_BUZZER    31
#define GPS_ON        43 //LOW means on
#define MOT           25 //HIGH means ON
#define K_BLT         19 //HIGH means ON
#define DISP_CS       27
#define DISP_DISP     11
#define BTN           34 
#define SD_CS         32
#define SD_DET        10

#define DIO1 (28)
#define DIO2 (8)
#define TXEN (6)
#define RXEN (26)
#define L_SS (4)
#define BUSY (29)
#define L_RST (30)

//-------------------menue variables-------------------
//in order to add a new app you have to:
//add the name to appNames String array, take care of the order
//create a boolean "insideFuncXXX"
//add a "case" in the appropriate order to void ok_callback(void) ISR
//add an if statement with function call in loop()
//create the app function inside a new .ino file ("add new tab") ideally named appFuncXXX (easier to find)
//in the sketch folder with the following structure:
/*
  void funcXXX(){
  //start sequence
  //display Stuff, initialize....
  display.clearDisplay();

  display.setTextSize(3); //standard font is 5x8, so scale it by 3 ->15x24
  display.setCursor(5, 0);
  display.setTextColor(LCD_COLOR_WHITE);
  display.println("funcXXX");
  display.refresh();
  while (insideSubMenue) { //the back button terminates the loop
    //loop of the APP
    readKeyboard();
    readWASD();
    // ...
    delay(100); //get some sleep else the app hangs!
   }
   insideFuncXXX=false;
  displayMenue(menueIndex);
  }
*/
//note: since the project structure is based on freeRTOS and is callback-driven, not all app code is inside this app function
//for example the button callbacks and bluetooth functions have to be adapted
unsigned long prevStepCount = 0;
byte menueIndex = 0;
String appNames [] = {"LORA Chat","GPS","Sensor Data","BLE Sensors", "Settings"}; //enter your app names in correct order
#define NUMITEMS(arg) ((unsigned int) (sizeof (arg) / sizeof (arg [0]))) // number of items in an array
byte maxMenueIndex = NUMITEMS(appNames) - 1;  //the actual nuber of apps minus one
boolean insideLORAchat = false;
boolean insideGPS = false;
boolean insideSensorData = false;
boolean insideSettings = false;
boolean insideBLEsensors = false;
//------------------------------------------------------
boolean booting, insideMenue, insideSubMenue, centralConnected, peripheralConnected, charging, notificationHere = false;
boolean inMotion = false;
String notificationString = "";
//booleans for WASD navigation. They are changed in readKeyboard() and call the dn_callback etc.
boolean nav_up=0;
boolean nav_dn = 0;
boolean nav_right=0;
boolean nav_left=0;
boolean enter = 0;
boolean alt_for_wasd = 0;

int minorHalfSize; // 1/2 of lesser of display width or height
const uint8_t  SPRINTF_BUFFER_SIZE {32};  ///< Buffer size for sprintf() (RTC stuff)
//unsigned int year, month, day, hour, minute,second;  // Variables to hold parsed date/time


// Set BATTERY_CAPACITY to the design capacity of your battery.
const unsigned int BATTERY_CAPACITY = 1100; //
unsigned int soc = 0;
// flag to indicate that a packet was received
volatile bool receivedFlag = false;
unsigned long bltTimeout = 0;
boolean incomingMsg = false;
boolean ledState=false;
// disable interrupt when it's not needed
volatile bool enableInterrupt = true;

//KEYBOARD STUFF
char buf[6][21];
short int pos = 0;
char oldchr = 0;
byte shiftlock = 0; // set/unset by pressing shift and letting it go rather than using it as a modifier for another key
byte shiftlockchanged = 0; // helper
unsigned long time = 0; // keypad repetition interval
byte displaychanged = 1;
byte curline = 0;

//create instances of the components
Adafruit_SharpMem display(&SPI, DISP_CS, 400, 240);
SX1262 lora = new Module(L_SS, DIO1, -1, BUSY);
BME280 bme280; //address is set to 0x76
DS3231M_Class DS3231M;
//MPU9250 mpu;
MPU9250_WE myMPU9250 = MPU9250_WE(MPU9250_ADDR);
// The TinyGPSPlus object
TinyGPSPlus gps; 

// BLE Service
BLEDfu  bledfu;  // OTA DFU service
BLEDis  bledis;  // device information
BLEUart bleuart; // uart over ble
BLEBas  blebas;  // battery

SoftwareTimer screenUpdateTimer;

// *******************************************************SETUP*****************************************************************
void setup(void)
{
  Serial.begin(9600);
  delay(3000);
  Serial.println("Hello!");
 Serial1.begin(9600); //hardware UART for GPS
  //----------------INIT all the I2C sensors--------------------------
  setupBQ27441(); //WTF, Serial and the fuel gauge has to be initialized prior to the pins....
  soc = lipo.soc();  // Read state-of-charge (%)
  delay(100);
  setupBME_RTC_IMU();
  
  //KEYBOARD STUFF 
  // initialize our 8 lines of text
  for (int i = 0; i < 8; i++) buf[i][0] = 0;

  setupPins();

  setupLORA();

  //----------------DISPLAY INIT--------------------------
  // start & clear the display
  display.begin();
  display.clearDisplay();
  // Several shapes are drawn centered on the screen.  Calculate 1/2 of
  // lesser of display width or height, this is used repeatedly later.
  minorHalfSize = min(display.width(), display.height()) / 2;

  //test display
  testdrawchar();
  display.refresh();
  delay(1000);
  display.clearDisplay();

  //----------------TESTS--------------------------
  /*
notificationAlarm();
  */
  
  //------------------------------SOFTWARE TIMERS-----------------------------------
    // Configure the timer with 1000 ms interval, with our callback
  screenUpdateTimer.begin(1000, screenUpdateTimer_callback);

  // Start the timer
  screenUpdateTimer.start();

  setupBLE();

  Serial.println("Position you MPU9250 flat and don't move it - calibrating...");
  delay(1000);
  myMPU9250.autoOffsets();
  Serial.println("Done!");
  myMPU9250.setSampleRateDivider(5);
  myMPU9250.setAccRange(MPU9250_ACC_RANGE_2G);
  myMPU9250.enableAccDLPF(true);
  myMPU9250.setAccDLPF(MPU9250_DLPF_6);  


}

// *******************************************************LOOP*****************************************************************
void loop(void)
{
  //printBatteryStats();
  //keep display backlight on for some seconds after incoming message
  if ((millis() - bltTimeout) > 5000 && incomingMsg == true) {
    digitalWrite(K_BLT, LOW);
    incomingMsg = false;
  }

  //----------------send data from serial and BLE via LORA--------------------------
  // Forward data from HW Serial to BLEUART
  while (Serial.available())
  {
    // Delay to wait for enough input, since we have a limited transmission buffer
    delay(2);

    uint8_t buf[64];
    int count = Serial.readBytes(buf, sizeof(buf));
    bleuart.write( buf, count );
  }
  if (bleuart.available() > 0) {
    int BLEbytes = 0;
    uint8_t BLEbuf[64];
    // Forward from BLEUART to HW Serial
    while ( bleuart.available() )
    {
      uint8_t ch;
      ch = (uint8_t) bleuart.read();
      Serial.write(ch);
      BLEbuf[BLEbytes] = ch;
      BLEbytes++;
    }

    //now transmit via LORA
    prepareTX();
    Serial.print(F("[SX1262] Transmitting packet ... "));

    // you can transmit C-string or Arduino string up to
    // 256 characters long
    // NOTE: transmit() is a blocking method!
    //       See example SX126x_Transmit_Interrupt for details
    //       on non-blocking transmission method.
    //int state = lora.transmit("Hello World!");

    // you can also transmit byte array up to 256 bytes long
    int state = lora.transmit(BLEbuf, BLEbytes + 1);

    if (state == ERR_NONE) {
      // the packet was successfully transmitted
      Serial.println(F("success!"));

      // print measured data rate
      Serial.print(F("[SX1262] Datarate:\t"));
      Serial.print(lora.getDataRate());
      Serial.println(F(" bps"));
      delay(200);
      prepareRX();
    } else if (state == ERR_PACKET_TOO_LONG) {
      // the supplied packet was longer than 256 bytes
      Serial.println(F("too long!"));
    } else if (state == ERR_TX_TIMEOUT) {
      // timeout occured while transmitting packet
      Serial.println(F("timeout!"));
    } else {
      // some other error occurred
      Serial.print(F("failed, code "));
      Serial.println(state);
    }
  }
  //----------------LORA RECEIVE ROUTINE--------------------------
  // check if the flag is set
  if (receivedFlag) {
    // disable the interrupt service routine while
    // processing the data
    enableInterrupt = false;
    // reset flag
    receivedFlag = false;
    // you can read received data as an Arduino String
    String str;
    int state = lora.readData(str);
    // you can also read received data as byte array
    /*
      byte byteArr[8];
      int state = lora.readData(byteArr, 8);
    */
    if (state == ERR_NONE) {
      // packet was successfully received
      Serial.println(F("[SX1262] Received packet!"));

      // print data of the packet
      Serial.print(F("[SX1262] Data:\t\t"));
      Serial.println(str);
      bleuart.print(str); //send via BLE
      display.clearDisplay();
     // digitalWrite(K_BLT, HIGH);
      notificationAlarm();
      display.setFont(&FreeSans9pt7b);
display.clearDisplay();
       display.setTextColor(BLACK);
      //display.setTextSize(1);
      display.setCursor(10, 30);
      display.print(str);
      display.println("     ");
      
      display.print("RSSI: ");
      display.print(lora.getRSSI());
      display.println(" dBm  ");
      display.print("SNR: ");
      display.print(lora.getSNR());
      display.println(" dB  ");
      display.refresh();
      bltTimeout = millis();
      incomingMsg = true;

      // print RSSI (Received Signal Strength Indicator)
      Serial.print(F("[SX1262] RSSI:\t\t"));
      Serial.print(lora.getRSSI());
      Serial.println(F(" dBm"));
      // print SNR (Signal-to-Noise Ratio)
      Serial.print(F("[SX1262] SNR:\t\t"));
      Serial.print(lora.getSNR());
      Serial.println(F(" dB"));
      
    } else if (state == ERR_CRC_MISMATCH) {
      // packet was received, but is malformed
      Serial.println(F("CRC error!"));
    } else {
      // some other error occurred
      Serial.print(F("failed, code "));
      Serial.println(state);
    }
    // put module back to listen mode
    lora.startReceive();
    // we're ready to receive more packets,
    // enable interrupt service routine
    enableInterrupt = true;
  }

 
  
  
//----------------housekeeping --------------------------
  readKeyboard();
  readWASD();
  /*
  display.setCursor(10, 200);
  display.setTextColor(BLACK, WHITE);
  display.setTextSize(2);
  for (int i = 0; i < 6; i++)
    display.print(buf[i]);
  display.refresh();
*/
//----------------MENU STUFF ----------------

    

    if (insideLORAchat) {
    appLORAchat();
  }

  if (insideGPS) {
    appGPS();
  }

  if (insideSensorData) {
    appSensorData();
  }

  if (insideSettings) {
    appSettings();
  }

  if (insideBLEsensors) {
    appBLEsensors();
  }

  //refresh dislay approx. every minute. Not perfect, I know.
//  if (second() == 0)
 //   if (!insideMenue) mainScreen();

  
  delay(100); //remember: delay=sleep in the nrf52 RTOS-based core
}



// *******************************************************FUNCTIONS*****************************************************************



void btn_callback(void)
{
  ledState=!ledState;
  digitalWrite(LED_BUILTIN, ledState);

  if (!insideMenue && !insideSubMenue) {
    insideMenue = true;
    menueIndex = 0;
    displayMenue(menueIndex);
  }
}

void screenUpdateTimer_callback(TimerHandle_t xTimerID)
{
  // freeRTOS timer ID, ignored if not used
  (void) xTimerID;

}

void ok_callback(void)
{

  //digitalToggle(BLT);
  if (insideMenue && !insideSubMenue) {
    insideSubMenue = true;
    switch (menueIndex) {
      case 0:
        //enter BLE scan function
        insideLORAchat = true;
        break;

      case 1:
        //Show data of connected BLE UART sensors
        insideGPS = true;
        break;

      case 2:
        insideSensorData = true;
        break;


      case 3:
        insideBLEsensors = true;
        break;

        
      case 4:
        insideSettings = true;
        break;

      case 5:
      /*
        clearBlack();
        display.setTextSize(2);
        display.setCursor(2, 70);
        display.setTextColor(WHITE);
        display.println("Shutdown...");
        display.setTextColor(WHITE);
        display.println("press OK --->");
        display.println("to turn on");
        display.refresh();
        delay(3000); //some delay to make sure the device does not reboot again immediately
        clearBlack();
        display.refresh();
        shutdownSystem();
        */
        break;

    }
  } else if (!insideMenue && !insideSubMenue) {
    insideMenue = true;
    menueIndex = 0;
    displayMenue(menueIndex);
  }

}

void bck_callback(void)
{

  //turnOff = true;
  //shutdownSystem();
  if (insideSubMenue) {
    insideSubMenue = false;
    displayMenue(menueIndex);
  } else if (insideMenue && !insideSubMenue) {
    insideMenue = false;
    mainScreen();
  } else {
    //what to do while in mainScreen
  }

}

void dn_callback(void)
{
  //enterOTADfu();
  if (insideMenue && !insideSubMenue) {
    if (menueIndex >= maxMenueIndex) menueIndex = maxMenueIndex; else menueIndex++;
    displayMenue(menueIndex);
  } else if (insideSubMenue) {
    //it depends
    //if (insideGestRecFunc) {
      //this decides whether we record or recognize gestures
    //  if (!recordGest) recordGest = true; else recordGest = false;

   // }
  } else {
   // digitalToggle(BLT);
  }

}
void up_callback(void)
{
  if (insideMenue && !insideSubMenue) {
    if (menueIndex <= 0) menueIndex = 0; else menueIndex--;
    displayMenue(menueIndex);
  } else if (insideSubMenue) {
    //it depends

  } else {
   // notificationHere = false; //delete the notification window on the main screen.
   // notificationString = "";
    mainScreen();
  }
}

void readWASD(){
  
    if (nav_up) up_callback();
    if (nav_dn) dn_callback();
    if (nav_right) ok_callback();
    if (nav_left) bck_callback();
}
