void mainScreen() {
  if (!insideMenue) {
    display.clearDisplay();
   // display.setTextSize(3); //standard font is 5x8, so scale it by 3 ->15x24
   display.setFont(&FreeSans18pt7b);

  display.setCursor(25, 30);
  display.setTextColor(BLACK);
  display.println("Main Screen");
  display.refresh();
  }

}
