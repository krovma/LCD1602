// 
// 
// 
#include <Arduino.h>
#include <string.h>
#include "LCD1602.h"

LCD::LCD(LCD &src)
	: _RS(src._RS), _RW(src._RW), _EN(src._EN),
	_D7(src._D7), _D5(src._D5), _D4(src._D4)
	,_rw_writeonly(src._rw_writeonly) {
}

LCD::LCD(byte RS, byte RW, byte EN, byte D7, byte D6, byte D5, byte D4, byte D3, byte D2, byte D1, byte D0)
	: _RS(RS), _RW(RW), _EN(EN),
	_D7(D7), _D6(D6), _D5(D5), _D4(D4),
	_D3(D3), _D2(D2), _D1(D1), _D0(D0) {
	_4port = false;
	if (_RW == LCD_Not_Use_Port_RW) {
		_rw_writeonly = true;
	}
	else {
		_rw_writeonly = false;
	}
}

LCD::LCD(byte RS, byte RW, byte EN, byte D7, byte D6, byte D5, byte D4)
	: _RS(RS), _RW(RW), _EN(EN),
	_D7(D7), _D6(D6), _D5(D5), _D4(D4) {
	_4port = true;
	if (_RW == LCD_Not_Use_Port_RW) {
		_rw_writeonly = true;
	} else {
		_rw_writeonly = false;
	}
}

inline void LCD::_enable() {
	digitalWrite(_EN, HIGH);
	delayMicroseconds(10);
	digitalWrite(_EN, LOW); // Fall to enable
}

inline bool LCD::isBusy() {
	static int count = 0;
	if (_rw_writeonly) {
		delayMicroseconds(5000); // Up to 5ms to finish internal operation (Based on Experience)
		return false;
	}
	pinMode(_D7, INPUT);
	digitalWrite(_RS, LCD_Instruction);
	digitalWrite(_RW, LCD_Read);	// BUSY/AD READ CT
	digitalWrite(_EN, HIGH);	// Enable reading operation
	delayMicroseconds(500);		// Data output delay time (TD) up to 360ns?
	bool bf = digitalRead(_D7);
	pinMode(_D7, OUTPUT);
	digitalWrite(_EN, LOW);
	return bf;
}

inline void LCD::writeInstruction(byte cmd)
{
	digitalWrite(_RS, LCD_Instruction);
	if (!_rw_writeonly) {
		digitalWrite(_RW, LCD_Write);
	}
	digitalWrite(_D7, cmd & 0x80);
	digitalWrite(_D6, cmd & 0x40);
	digitalWrite(_D5, cmd & 0x20);
	digitalWrite(_D4, cmd & 0x10);
	if (_4port) {
		_enable();
		digitalWrite(_D7, cmd & 0x08);
		digitalWrite(_D6, cmd & 0x04);
		digitalWrite(_D5, cmd & 0x02);
		digitalWrite(_D4, cmd & 0x01);
		_enable();
	} else {
		digitalWrite(_D3, cmd & 0x08);
		digitalWrite(_D2, cmd & 0x04);
		digitalWrite(_D1, cmd & 0x02);
		digitalWrite(_D0, cmd & 0x01);
		_enable();
	}
}

void LCD::init()
{
	pinMode(_RS, OUTPUT);
	pinMode(_RW, OUTPUT);
	pinMode(_D7, OUTPUT);
	pinMode(_D6, OUTPUT);
	pinMode(_D5, OUTPUT);
	pinMode(_D4, OUTPUT);
	pinMode(_EN, OUTPUT);
	digitalWrite(_EN, LOW);
	if (!_4port) {
		pinMode(_D3, OUTPUT);
		pinMode(_D2, OUTPUT);
		pinMode(_D1, OUTPUT);
		pinMode(_D0, OUTPUT);
	}
	if (!_4port) {
		// software reset
		delay(15);	// << Wait time > 15ms after VDD > 4.5V >>
		writeInstruction(LCD_FunctionSet | LCD_Function_8Port | LCD_Function_2Line | LCD_Function_char_57);
		delay(5);	// << Wait time > 4.1ms >>
		writeInstruction(LCD_FunctionSet | LCD_Function_8Port | LCD_Function_2Line | LCD_Function_char_57);
		delayMicroseconds(150);	// << Wait time > 100us >>
		writeInstruction(LCD_FunctionSet | LCD_Function_8Port | LCD_Function_2Line | LCD_Function_char_57);
		// software reset done and LCD1602 in 8-Port mode
	} else {
		digitalWrite(_RS, LCD_Instruction);
		if (!_rw_writeonly) {
			digitalWrite(_RW, LCD_Write);
		}
		// Software reset, only high 4 bits
		delay(15);	// << Wait time > 15ms after VDD > 4.5V >>
		digitalWrite(_D7, LOW); digitalWrite(_D6, LOW); digitalWrite(_D5, HIGH); digitalWrite(_D4, HIGH);
		_enable();
		delay(5);	// << Wait time > 4.1ms >>
		digitalWrite(_D7, LOW); digitalWrite(_D6, LOW); digitalWrite(_D5, HIGH); digitalWrite(_D4, HIGH);
		_enable();
		delayMicroseconds(150);	// << Wait time > 100us >>
		digitalWrite(_D7, LOW); digitalWrite(_D6, LOW); digitalWrite(_D5, HIGH); digitalWrite(_D4, HIGH);
		_enable();
		// software reset done and LCD1602 in 8-Port mode
		// ----- Set to 4-port mode -----
		// For 4-port mode, write the high 4 bits of function set instruction first
		digitalWrite(_D7, LOW); digitalWrite(_D6, LOW); digitalWrite(_D5, HIGH); digitalWrite(_D4, LOW);
		// High 4 bits of 0b001010** (FUNCTION SET: 4PORT, 2LINES, 5*7 CHARACTER)
		_enable();
		delay(10);
		// Set LCD1602 to 4-Port mode, setting lines and character, high 4 bits should be send again
		//while (isBusy());
		writeInstruction(LCD_FunctionSet | LCD_Function_4Port | LCD_Function_2Line | LCD_Function_char_57);
		// LCD1602 in 4-port mode now
	}
	clear();
}

void LCD::clear() {
	while (isBusy());
	writeInstruction(LCD_Clear);
	delayMicroseconds(5000);
}

void LCD::cursorReturn() {
	while (isBusy());
	writeInstruction(LCD_CursorReturn);
}

void LCD::inputSet(bool increment, bool shift) {
	while(isBusy());
	byte cmd = LCD_InputSet
				| (increment ? LCD_Input_Increment : LCD_Input_Decrement)
				| (shift ? LCD_Input_Shift : LCD_Input_Noshift);
	writeInstruction(cmd);
}

void LCD::displaySwitch(bool display, bool cursor, bool blink) {
	while (isBusy());
	byte cmd = LCD_DisplaySwitch
				| (display ? LCD_Display_DisplayOn : LCD_Display_DisplayOff)
				| (cursor ? LCD_Display_CursorOn : LCD_Display_CursorOff)
				| (blink ? LCD_Display_BlinkOn : LCD_Display_BlinkOff);
	writeInstruction(cmd);
}

void LCD::shift(bool target, bool direction) {
	while (isBusy());
	byte cmd = LCD_Shift
				| (target ? LCD_Shift_DisplayShift : LCD_Shift_CursorShift)
				| (direction ? LCD_Shift_Right : LCD_Shift_Left);
	writeInstruction(cmd);
}

void LCD::setDataAddr(byte add) {
	while (isBusy());
	writeInstruction(LCD_DDRAMAddSet | add);
}

void LCD::writeData(byte data) {
	while (isBusy());
	digitalWrite(_RS, LCD_Data);
	if (!_rw_writeonly) {
		digitalWrite(_RW, LCD_Write);
	}
	digitalWrite(_D7, data & 0x80);
	digitalWrite(_D6, data & 0x40);
	digitalWrite(_D5, data & 0x20);
	digitalWrite(_D4, data & 0x10);
	if (_4port) {
		_enable();
		digitalWrite(_D7, data & 0x08);
		digitalWrite(_D6, data & 0x04);
		digitalWrite(_D5, data & 0x02);
		digitalWrite(_D4, data & 0x01);
		_enable();
	} else {
		digitalWrite(_D3, data & 0x08);
		digitalWrite(_D2, data & 0x04);
		digitalWrite(_D1, data & 0x02);
		digitalWrite(_D0, data & 0x01);
		_enable();
	}
}

void LCD::writeData(byte data[], int n) {
	for (auto i = 0; i < n; ++i) {
		writeData(data[i]);
	}
}

void LCD::writeData(char data[]) {
	writeData(reinterpret_cast<byte *>(data), strlen(data));
}

void LCD::writeData(byte data[][16]) {
	clear();
	writeData(data[0], 16);
	setDataAddr(LCD_Line2Start);
	writeData(data[1], 16);
}