void readKeyboard(){
  //https://forum.arduino.cc/t/interfacing-blackberry-q10-keypad-to-arduino-and-the-oled-typewriter/342989
byte alt = 0; // flag for alt modifer key, unimplemented.
  byte sym = 0; // flag for symbol modifer key
  byte shift = 0; // flag for shift modifier key
  byte shl = 0; // left shift for shift lock detection
  byte shr = 0; // right shift for shift lock detection
  char chr = 0; // current character
  char symb = 0; // current symbol
nav_up=0;
nav_dn = 0;
nav_right=0;
nav_left=0;
enter=0;

  // figure out the current scan code, if any, and any modifier keys, if any
  pinMode(col1, OUTPUT); 
  digitalWrite(col1, LOW);
  delay(1);
  if (digitalRead(row1)==0) {chr = 'Q'; symb='#';}
  if (digitalRead(row2)==0) {chr = 'W'; symb='1'; nav_up=1; } //if (insideMenue) up_callback();
  if (digitalRead(row3)==0) {sym = 1;}             // symbol modifier key
  if (digitalRead(row4)==0) {chr = 'A'; symb='*'; nav_left=1; } //if (insideMenue) bck_callback();
  if (digitalRead(row5)==0) {alt = 1; alt_for_wasd = !alt_for_wasd;}             // alt modifier key - it enables different usage scenarios of the keyboard, f.e. wasd navigation
  if (digitalRead(row6)==0) {chr = ' '; symb=' ';}
  if (digitalRead(row7)==0) {chr = '~'; symb='0';}
  
  pinMode(col1, INPUT); 
  pinMode(col2, OUTPUT); 
  digitalWrite(col2, LOW);
  delay(1); //the nrf52840 needs those delays
  if (digitalRead(row1)==0) {chr = 'E'; symb='2';}
  if (digitalRead(row2)==0) {chr = 'S'; symb='4'; nav_dn=1;} //if (insideMenue) dn_callback();
  if (digitalRead(row3)==0) {chr = 'D'; symb='5'; nav_right=1;} //if (insideMenue) ok_callback();
  if (digitalRead(row4)==0) {chr = 'P'; symb='@';}
  if (digitalRead(row5)==0) {chr = 'X'; symb='8';}
  if (digitalRead(row6)==0) {chr = 'Z'; symb='7';}
  if (digitalRead(row7)==0) {shift = 1; shl = 1;}  // shift modifier key
  
  pinMode(col2, INPUT); 
  pinMode(col3, OUTPUT); 
  digitalWrite(col3, LOW);
 delay(1);
  if (digitalRead(row1)==0) {chr = 'R'; symb='3';}
  if (digitalRead(row2)==0) {chr = 'G'; symb='/';}
  if (digitalRead(row3)==0) {chr = 'T'; symb='(';}
  if (digitalRead(row4)==0) {shift = 1; shr = 1;}  // shift modifier key
  if (digitalRead(row5)==0) {chr = 'V'; symb='?';}
  if (digitalRead(row6)==0) {chr = 'C'; symb='9';}
  if (digitalRead(row7)==0) {chr = 'F'; symb='6';}
  
  pinMode(col3, INPUT); 
  pinMode(col4, OUTPUT); 
  digitalWrite(col4, LOW);
 delay(1);
  if (digitalRead(row1)==0) {chr = 'U'; symb='_';}
  if (digitalRead(row2)==0) {chr = 'H'; symb=':';}
  if (digitalRead(row3)==0) {chr = 'Y'; symb=')';}
  if (digitalRead(row4)==0) {chr = '|'; symb='|'; enter=1;} // this should be a CR but I am substituting a pipe for CR in this implementation
  if (digitalRead(row5)==0) {chr = 'B'; symb='!';}
  if (digitalRead(row6)==0) {chr = 'N'; symb=',';}
  if (digitalRead(row7)==0) {chr = 'J'; symb=';';}
  
  pinMode(col4, INPUT); 
  pinMode(col5, OUTPUT); 
  digitalWrite(col5, LOW);
  delay(1);
  if (digitalRead(row1)==0) {chr = 'O'; symb='+';}
  if (digitalRead(row2)==0) {chr = 'L'; symb='"';}
  if (digitalRead(row3)==0) {chr = 'I'; symb='-';}
  if (digitalRead(row4)==0) {chr = 8;}            // backspace
  if (digitalRead(row5)==0) {chr = '$'; symb='`';}
  if (digitalRead(row6)==0) {chr = 'M'; symb='.';}
  if (digitalRead(row7)==0) {chr = 'K'; symb='\'';}
  
  pinMode(col5, INPUT); 
if(alt_for_wasd==false){
  if (chr != oldchr)
    if (chr==8)    // Deal with backspace  
    {
      if ((pos > 0) || (curline > 0)) // don't underflow our buffer
      {
          if (pos==0) {pos=20; curline--;}
          buf[curline][--pos] = 0;
          time = millis();
          displaychanged = 1;
      }
    }
    else if (chr !=0)
    {
      if (curline < 6) // don't overflow our buffer      
      {
        if (sym==1) // if the symbol key is pressed, put it in the buff as a symbol
          buf[curline][pos] = symb;
        // enter raw/upper case character if shift is selected or it is not a shiftable character
        else if (shift==1 || shiftlock==1 || chr=='$' || chr==' ' || chr=='~' || chr == 13)
          buf[curline][pos] = chr;
        // otherwise enter as a lower case character
        else
          buf[curline][pos] = (chr+32);    
          
        // advance end of buffer
        buf[curline][++pos] = 0;
        if (pos>19) {curline++; pos=0;}
        time = millis();
        displaychanged = 1;
      }
    }
  
  // Pressing both shift keys together is taken as a shift lock operation
  
  if (shl == 1 && shr == 1 && shiftlockchanged == 0)
  {
    shiftlock = (shiftlock==0)?1:0;
    shiftlockchanged = 1;  // prevent multiple shift lock activations
  }

  // release shift lock multi-activation production when either shift key is released.
  if ((shl == 0) || (shr == 0))
    shiftlockchanged = 0;
    
  oldchr = chr; // remember old character so we don't have a high keyboard repeat rate
}
  // however, allow repeat if there has been no change in keypress in 200ms by clearing old character
  
  if ((millis()-time)>200)
  {
    oldchr = 0;
    time = millis();
  }

  
}
