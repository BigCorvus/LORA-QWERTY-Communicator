void appSettings(){
  //start sequence
  //display Stuff, initialize....
  display.clearDisplay();
  //display.setTextSize(3); //standard font is 5x8, so scale it by 3 ->15x24
  display.setFont(&FreeSans18pt7b);

  display.setCursor(25, 30);
  display.setTextColor(BLACK);
  display.println("Settings");
  display.refresh();
  while (insideSubMenue) { //the back button terminates the loop
    readKeyboard(); //navigate with "WASD"
    readWASD();
    //loop of the APP
    delay(100); //get some sleep else the app hangs!
   }
   insideSettings=false;
  displayMenue(menueIndex);
}
