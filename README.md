# LCD1602
A full library of LCD1602 on (Ard|Gen)uino
LCD1602 is a series of HD44780 compatible LCD module.

# To-do list
1. Add support for CGRAM
2. Add a map for HD44780 non-ascii characters
3. Add some common display effects

# Example
```C++
/*
 *  Example of the usage
 *  Pin Layout: Minimal (6 pins, write only)
 *  LCD1602     Arduino
 *  --------------------
 *      RS          12
 *      RW          GND
 *      E           10
 *  D7 --> D4    7 --> 4
 */

#include <LCD1602.h>

LCD lcd(12,LCD_Not_Use_Port_RW,10,7,6,5,4);
void setup(){
    lcd.init();
    lcd.displaySwitch(true, true, false);
    lcd.inputSet(true, false);
    lcd.clear();
}

void loop() {
    lcd.writeData("lib LCD1602");
    lcd.setDataAddr(LCD_Line2Start);
    lcd.writeData("I Love Arduino");
    delay(1000);
    lcd.clear();
}
```
