void appSensorData() {
  //start sequence
  //display Stuff, initialize....
  display.clearDisplay();
  //display.setTextSize(3); //standard font is 5x8, so scale it by 3 ->15x24
  display.setFont(&FreeSans18pt7b);
  display.setCursor(25, 30);
  display.setTextColor(BLACK);
  display.println("Sensor Data");
  display.refresh();
  while (insideSubMenue) { //the back button terminates the loop
    readKeyboard(); //navigate with "WASD"
    readWASD();
    //loop of the APP
display.clearDisplay();
    //----------------DRAWING AND MEASURING STUFF --------------------------
    unsigned int soc = lipo.soc();
    display.setTextColor(BLACK);
    //display.setTextSize(1);
    display.setFont(&FreeSans9pt7b);

    display.setCursor(330, 20);
    //display.print("    ");
    //display.refresh();
    display.setCursor(330, 20);
    display.print(soc);
    display.print("% ");
    display.setCursor(330, 40);
    int current = lipo.current(AVG); // Read average current (mA)
    //display.print("     ");
    //display.refresh();
    display.setCursor(330, 40);
    display.print(current);
    display.print("mA  ");
    display.refresh();
    delay(5);
    unsigned int hum =  bme280.readFloatHumidity();
    display.setCursor(330, 60);
    display.print(hum);
    display.print("%  ");
    display.refresh();
    delay(5);
    DateTime       now = DS3231M.now();  // get the current time from device
    // Use sprintf() to pretty print the date/time with leading zeros
    char output_buffer[SPRINTF_BUFFER_SIZE];  ///< Temporary buffer for sprintf()
    sprintf(output_buffer, "%04d-%02d-%02d %02d:%02d:%02d", now.year(), now.month(), now.day(),
            now.hour(), now.minute(), now.second());
    display.setCursor(10, 40);
    display.print(output_buffer);
    display.refresh();
    delay(5);

    xyzFloat angles = myMPU9250.getAngles();
    //mpu.update();
    display.setCursor(10, 60);
    // float roll = mpu.getRoll();
    display.print(angles.x);
    display.print(" ");
    //float pitch = mpu.getPitch();
    display.print(angles.y);
    display.print(" ");
    // float yaw = mpu.getYaw();
    display.print(angles.z);
    display.print("     ");
    display.refresh();

    delay(400); //get some sleep else the app hangs!
  }
  insideSensorData = false;
  displayMenue(menueIndex);
}
