/*
 *  Example of the usage
 *  Pin Layout: 4port RW (7 pins, R/W)
 *  LCD1602     Arduino
 *  --------------------
 *      RS          12
 *      RW          11
 *      E           10
 *  D7 --> D4    7 --> 4
 */

#include <LCD1602.h>

LCD lcd(12, 11, 10, 7, 6, 5, 4);
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
