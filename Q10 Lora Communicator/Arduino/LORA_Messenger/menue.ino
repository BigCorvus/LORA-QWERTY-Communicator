void displayMenue(int itemIndex) {
  display.clearDisplay();
  display.setFont(&FreeSans18pt7b);

  //display.setTextSize(3);
  display.setCursor(25, 30);
  display.setTextColor(BLACK);
  display.println("Menu");
  //display.setTextSize(2);
  display.setFont(&FreeSans12pt7b);

  for (int i = 0; i <= maxMenueIndex; i++) {
    readKeyboard(); //navigate with "WASD"

display.print(appNames[i]);

    if (i == itemIndex) {
      //highlight the selcted item (only possible with builtin basic font)
      //display.setTextColor(WHITE, BLACK);
      display.print("<--"); //used as an arrow indicating the chosen item
    } else {
      //display.setTextColor(BLACK, WHITE);
      display.print("   ");
    }
    display.println();
  }

  display.refresh();
}
