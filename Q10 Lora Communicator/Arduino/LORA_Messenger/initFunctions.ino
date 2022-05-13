void setupPins(void){
   // basically we are setting columns to High-Z outputs which we will enable one by one in our keyboard scan routine
  pinMode(col1, INPUT); // High-Z
  pinMode(col2, INPUT); // High-Z
  pinMode(col3, INPUT); // High-Z
  pinMode(col4, INPUT); // High-Z
  pinMode(col5, INPUT); // High-Z

  // Rows are inputs with pullups
  pinMode(row1, INPUT_PULLUP);
  pinMode(row2, INPUT_PULLUP);
  pinMode(row3, INPUT_PULLUP);
  pinMode(row4, INPUT_PULLUP);
  pinMode(row5, INPUT_PULLUP);
  pinMode(row6, INPUT_PULLUP);
  pinMode(row7, INPUT_PULLUP);

  pinMode(L_RST, OUTPUT);
  pinMode(TXEN, OUTPUT);
  pinMode(RXEN, OUTPUT);

  pinMode(BTN, INPUT_PULLUP);
  pinMode(RTC_INT, INPUT);
  pinMode(IMU_INT, INPUT);
  pinMode(DISP_DISP, OUTPUT);
  pinMode(K_BLT, OUTPUT);
  pinMode(MOT, OUTPUT);
  pinMode(GPS_ON, OUTPUT);

  attachInterrupt(BTN, btn_callback, ISR_DEFERRED | FALLING);

  digitalWrite(DISP_DISP, HIGH);
  digitalWrite(MOT, LOW);
  digitalWrite(K_BLT, LOW);
  digitalWrite(GPS_ON, HIGH); //HIGH means ON here
}


void setupBQ27441(void)
{
  // Use lipo.begin() to initialize the BQ27441-G1A and confirm that it's
  // connected and communicating.
  if (!lipo.begin()) // begin() will return true if communication is successful
  {
    // If communication fails, print an error message and loop forever.
    Serial.println("Error: Unable to communicate with BQ27441.");
    Serial.println("  Check wiring and try again.");
    Serial.println("  (Battery must be plugged into Battery Babysitter!)");
    //while (1) ;
  }
  Serial.println("Connected to BQ27441!");

  // Uset lipo.setCapacity(BATTERY_CAPACITY) to set the design capacity
  // of your battery.
  lipo.setCapacity(BATTERY_CAPACITY);
}

void setupLORA(void) {

  //----------------LORA INIT--------------------------
  digitalWrite(L_RST, LOW);
  delay(100);
  digitalWrite(L_RST, HIGH);
  delay(100);
  //Wire.begin();
  prepareRX(); //switch RXEN and TXEN
  Serial.println("LORA BLE Relay and QWERTY communicator based on nRF52840 and SX1262\n");
  Serial.print(F("[SX1262] Initializing ... "));
  // initialize SX1262
  // carrier frequency:           868.0 MHz
  // bandwidth:                   125.0 kHz
  // spreading factor:            7
  // coding rate:                 5
  // sync word:                   0x1424 (private network)
  // output power:                22 dBm
  // current limit:               60 mA
  // preamble length:             8 symbols
  // CRC:                         enabled
  //  int16_t begin(float freq = 434.0, float bw = 125.0, uint8_t sf = 9, uint8_t cr = 7,
  // uint8_t syncWord = SX126X_SYNC_WORD_PRIVATE, int8_t power = 14, float currentLimit = 60.0,
  // uint16_t preambleLength = 8, float tcxoVoltage = 1.6, bool useRegulatorLDO = false);
  int state = lora.begin(868.0, 125.0, 7, 5, 0x1424, 22, 100, 8, 2.4, 0);

  if (state == ERR_NONE) {
    Serial.println(F("lora init success!"));
  } else {
    Serial.print(F("lora init failed, code "));
    Serial.println(state);
    //while (true);
  }

  // eByte E22-900M22S uses DIO3 to supply the external TCXO
  if (lora.setTCXO(2.4) == ERR_INVALID_TCXO_VOLTAGE)
  {
    Serial.println(F("Selected TCXO voltage is invalid for this module!"));
  }
  // set the function that will be called
  // when new packet is received
  lora.setDio1Action(setFlag);

  // start listening for LoRa packets
  Serial.print(F("[SX1262] Starting to listen ... "));
  state = lora.startReceive();
  if (state == ERR_NONE) {
    Serial.println(F("success!"));
  } else {
    Serial.print(F("failed, code "));
    Serial.println(state);
    while (true);
  }
}

void setupBME_RTC_IMU(void) {
  if (bme280.beginI2C() == false) //Begin communication over I2C
  {
    Serial.println("The sensor did not respond. Please check wiring.");
    while (1); //Freeze
  }
  delay(100);
  while (!DS3231M.begin())  // Initialize RTC communications
  {
    Serial.println(F("Unable to find DS3231MM. Dammit."));
    while (1); //Freeze
  }
  //DS3231M.pinSquareWave();  // Make INT/SQW pin toggle at 1Hz
  //DS3231M.adjust(DateTime(year, month, day, hour, minute, second));  // Set to library compile Date/Time

  // following line sets the RTC to the date & time this sketch was compiled
  DS3231M.adjust(DateTime(__DATE__, __TIME__));

  delay(1000);
  if (!myMPU9250.init()) {
    Serial.println("MPU9250 does not respond");
  }
  else {
    Serial.println("MPU9250 is connected");
  }

  delay(100);
}
