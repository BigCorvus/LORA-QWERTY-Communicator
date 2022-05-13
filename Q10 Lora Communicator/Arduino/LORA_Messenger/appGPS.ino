void appGPS() {
  //start sequence
  //display Stuff, initialize....
  digitalWrite(GPS_ON, LOW); //turn ON GPS
  display.clearDisplay();
 // display.setTextSize(3); //standard font is 5x8, so scale it by 3 ->15x24
 display.setFont(&FreeSans18pt7b);

  display.setCursor(25, 30);
  display.setTextColor(BLACK);
  display.println("GPS");
  display.refresh();


  while (insideSubMenue) { //the back button terminates the loop
    readKeyboard(); //navigate with "WASD"
    readWASD();

    while (Serial1.available() > 0)
      if (gps.encode(Serial1.read()))
        displayInfo();

    //loop of the APP
    delay(100); //get some sleep else the app hangs!
  }
  digitalWrite(GPS_ON, HIGH); //turn OFF GPS
  insideGPS = false;
  displayMenue(menueIndex);
}



void displayInfo()
{

  //display.setTextColor(BLACK, WHITE);
display.clearDisplay();

  if (gps.location.isValid())
  {
   // display.setTextSize(2); //standard font is 5x8, so scale it by 3 ->15x24
   display.setFont(&FreeSans12pt7b);

    display.setCursor(5, 50);
    display.print("LAT: ");
    display.print(gps.location.lat(), 6);
    display.print(", LON: ");
    display.print(gps.location.lng(), 6);
  }
  else
  {
    //display.setTextSize(2); //standard font is 5x8, so scale it by 3 ->15x24
    display.setCursor(5, 50);
    display.print("LOC INVALID");
  }


  if (gps.date.isValid())
  {
    //display.setTextSize(2); //standard font is 5x8, so scale it by 3 ->15x24
    display.setFont(&FreeSans12pt7b);

    display.setCursor(5, 70);
    display.print(gps.date.day());
    display.print(".");
    display.print(gps.date.month());
    display.print(".");
    display.print(gps.date.year());
    display.print("     "); //so the "invalid" line disappears
  }
  else
  {
    //display.setTextSize(2); //standard font is 5x8, so scale it by 3 ->15x24
    display.setFont(&FreeSans12pt7b);

    display.setCursor(5, 70);
    display.print("DATE INVALID");
  }


  if (gps.time.isValid())
  {
    //display.setTextSize(2); //standard font is 5x8, so scale it by 3 ->15x24
    display.setFont(&FreeSans12pt7b);

    display.setCursor(5, 90);
    if (gps.time.hour() < 10) display.print("0");
    display.print(gps.time.hour()+TIME_ZONE);
    display.print(":");
    if (gps.time.minute() < 10) display.print("0");
    display.print(gps.time.minute());
    display.print(":");
    if (gps.time.second() < 10) display.print("0");
    display.print(gps.time.second());
    display.print(".");
    if (gps.time.centisecond() < 10) display.print("0");
    display.print(gps.time.centisecond());
    display.print("  ");
  }
  else
  {
    //display.setTextSize(2); //standard font is 5x8, so scale it by 3 ->15x24
    display.setFont(&FreeSans12pt7b);

    display.setCursor(5, 90);
    display.print("TIME INVALID");
  }

  display.refresh();
}
