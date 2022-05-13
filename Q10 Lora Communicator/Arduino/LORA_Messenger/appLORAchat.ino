void appLORAchat() {
  //start sequence
  //display Stuff, initialize....
  char loraBuf[126];

  display.clearDisplay();
  // display.setTextSize(3); //standard font is 5x8, so scale it by 3 ->15x24
  display.setFont(&FreeSans18pt7b);

  display.setCursor(25, 30);
  display.setTextColor(BLACK);
  display.println("LORA Chat");
  display.refresh();
  while (insideSubMenue) { //the back button terminates the loop
    readKeyboard(); //navigate with "WASD"
    readWASD();

    display.setCursor(10, 200);
    display.setTextColor(BLACK);
    //display.setTextSize(2);
    display.setFont(&FreeSans12pt7b);

    for (int i = 0; i < 6; i++)
      display.print(buf[i]);
    display.refresh();

    //now transmit via LORA
    prepareTX();
    Serial.print(F("[SX1262] Transmitting packet ... "));

    // you can transmit C-string or Arduino string up to
    // 256 characters long
    // NOTE: transmit() is a blocking method!
    //       See example SX126x_Transmit_Interrupt for details
    //       on non-blocking transmission method.
    //int state = lora.transmit("Hello World!");

    if (enter) {
      uint8_t idx = 0;
      for (int i = 0; i < 6; i++) {
        for (int j = 0; j < 21; j++) {
          idx = (i + 1) * j;
          loraBuf[idx] = buf[i][j];
        }
      }
      // you can also transmit byte array up to 256 bytes long
      int state = lora.transmit(loraBuf, sizeof(loraBuf));

      enter = 0;
      memset(loraBuf, 0, sizeof(loraBuf)); //clear the array
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
        // bleuart.print(str); //send via BLE
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

    //loop of the APP
    delay(100); //get some sleep else the app hangs!
  }
  insideLORAchat = false;
  displayMenue(menueIndex);
}
