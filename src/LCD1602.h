/*
 *
 *
 */
#ifndef _LCD1602_h
#define _LCD1602_h

///////////////////////////////////////////////
/// Marcos
///////////////////////////////////////////////
#define LCD_Not_Use_Port_RW 0xff
#define LCD_Instruction LOW
#define LCD_Data HIGH
#define LCD_Write LOW
#define LCD_Read HIGH
///////////////////////////////////////////////
/// LCD1602 Instructions
///////////////////////////////////////////////
#define LCD_Clear 0x01

#define LCD_CursorReturn 0x02

#define LCD_InputSet 0x04
#define LCD_Input_Increment 0x02
#define LCD_Input_Decrement 0x00
#define LCD_Input_Shift 0x01
#define LCD_Input_Noshift 0x00

#define LCD_DisplaySwitch 0x08
#define LCD_Display_DisplayOn 0x04
#define LCD_Display_CursorOn 0x02
#define LCD_Display_BlinkOn 0x01
#define LCD_Display_DisplayOff 0x00
#define LCD_Display_CursorOff 0x00
#define LCD_Display_BlinkOff 0x00

#define LCD_Shift 0x10
#define LCD_Shift_DisplayShift 0x08
#define LCD_Shift_CursorShift 0x00
#define LCD_Shift_Right 0x04
#define LCD_Shift_Left 0x00
#define LCD_LEFT false
#define LCD_RIGHT true
#define LCD_CURSOR false
#define LCD_DISPLAY true

#define LCD_FunctionSet 0x20
#define LCD_Function_8Port 0x10
#define LCD_Function_4Port 0x00
#define LCD_Function_2Line 0x08
#define LCD_Funciton_1Line 0x00
#define LCD_Function_char_510 0x04  // Generally unusable
#define LCD_Function_char_57 0x00

#define LCD_CGRAMAddSet 0x40

#define LCD_DDRAMAddSet 0x80
#define LCD_Line1Start 0x00
#define LCD_Line2Start 0x40
///////////////////////////////////////////////
/// Begin of class LCD defination
///////////////////////////////////////////////
class LCD {
private:
	byte _RS;
	byte _RW;
	byte _EN;
	byte _D7, _D6, _D5, _D4;
	byte _D3, _D2, _D1, _D0;
	bool _4port;
	bool _rw_writeonly;
	LCD() = delete;

	inline void _enable();
	/// enable pulse on port EN
public:
	//////////////////////////////////////////////////////
	/// Constructors also for setting up the Arduino
	//////////////////////////////////////////////////////
	explicit LCD(LCD &src);
	LCD(byte RS, byte RW, byte EN, byte D7, byte D6, byte D5, byte D4, byte D3, byte D2, byte D1, byte D0);
	LCD(byte RS, byte RW, byte EN, byte D7, byte D6, byte D5, byte D4);
	/// Desinate the Arduino I/O port
	///
	/// RS, EN, D7, D6, D5, D4, [D3, D2, D1, D0]:
	///		Arduino pin# connected to each port of LCD1602.
	/// RW: 
	///		Arduino pin# connected to R/W port of LCD1602.
	///     If R/W port is connectd to GND (to saving precious Arduino I/O port), set RW to LCD_Not_Use_Port_RW,
	///     then all reading operation is invalid, and for reliability wherever to check LCD1602's busy flag will
	///     be replaced by delayMicroseconds(5000).

	/////////////////////////////////////////////////////
	/// Utilities
	/////////////////////////////////////////////////////
	inline bool isBusy();
	/// Check the busy flag (D7) and returns
	/// Always return false if R/W is unused
	inline void writeInstruction(byte cmd);
	/// Directly call instruction
	/// as a low-level interaction method, this method will NOT check busy flag
	void init();
	///	Initialize LCD1602 in 4-Port mode, 2-line display and 5*7 dots characters
	///
	///	Initialize sequence is from the module specification.
	/// Typical LCD1602 module can't use 5*10 dots characters. If you are using HD44780

	/////////////////////////////////////////////////////
	/// LCD1602 instruction encapsulation
	///	works as the name
	/////////////////////////////////////////////////////
	void clear();
	void cursorReturn();
	void inputSet(bool increment, bool shift);
	void displaySwitch(bool display, bool cursor, bool blink);
	void shift(bool target, bool direction);
	void setDataAddr(byte add);
	/////////////////////////////////////////////////////
	/// LCD1602 Data Writing
	/////////////////////////////////////////////////////
	void writeData(byte data);
	void writeData(byte data[][16]);
	/// Write a standard 16*2 display
	/// display will be cleared
	void writeData(byte data[], int n);
	/// n: length of data
	/// n can be greater than 16
	/// The cursor WILL NOT RESET
	void writeData(char data[]);
	/// A convenient substitute for writeData(byte*, int)
};
#endif
