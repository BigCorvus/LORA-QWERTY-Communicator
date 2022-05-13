//LORA STUFF
void prepareTX(void) {
  digitalWrite(RXEN, LOW);
  digitalWrite(TXEN, HIGH);

}

void prepareRX(void) {
  digitalWrite(TXEN, LOW);
  digitalWrite(RXEN, HIGH);
}

// this function is called when a complete packet
// is received by the module
// IMPORTANT: this function MUST be 'void' type
//            and MUST NOT have any arguments!
void setFlag(void) {
  // check if the interrupt is enabled
  if (!enableInterrupt) {
    return;
  }
  // we got a packet, set the flag
  receivedFlag = true;
}

//ALARM function
void notificationAlarm(void){
      digitalWrite(K_BLT, HIGH);
    digitalWrite(MOT, HIGH);
    delay(200);
    digitalWrite(K_BLT, LOW);
    digitalWrite(MOT, LOW);

    tone(PIN_BUZZER, 2000, 70);
}

void testdrawchar(void) {
  display.setTextSize(0);
  display.setTextColor(BLACK);
  display.setCursor(0, 0);
  display.cp437(true);

  for (int i = 0; i < 256; i++) {
    if (i == '\n') continue;
    display.write(i);
  }
  display.refresh();
}

//Fuel Gauge


void printBatteryStats()
{
  // Read battery stats from the BQ27441-G1A
  unsigned int soc = lipo.soc();  // Read state-of-charge (%)
  unsigned int volts = lipo.voltage(); // Read battery voltage (mV)
  int current = lipo.current(AVG); // Read average current (mA)
  unsigned int fullCapacity = lipo.capacity(FULL); // Read full capacity (mAh)
  unsigned int capacity = lipo.capacity(REMAIN); // Read remaining capacity (mAh)
  int power = lipo.power(); // Read average power draw (mW)
  int health = lipo.soh(); // Read state-of-health (%)
  blebas.write(soc);
  // Now print out those values:
  String toPrint = String(soc) + "% | ";
  toPrint += String(volts) + " mV | ";
  toPrint += String(current) + " mA | ";
  toPrint += String(capacity) + " / ";
  toPrint += String(fullCapacity) + " mAh | ";
  toPrint += String(power) + " mW | ";
  toPrint += String(health) + "%";

  Serial.println(toPrint);
}
