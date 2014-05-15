/*
 * lcd.h
 *
 * Definitionen und Funktionen für die Ansteuerung des LC-Displays mit HD44780-kompatiblem Controller
 * im 4-bit-Modus
 *
 */

#ifndef LCD_H_
#define LCD_H_

#define		LCD_BIT_MODUS	4

// Anzupassende Definitionen
#define		ZEILEN			4
#define		SPALTEN			20

// Anschlüsse
#define		RS_PORT			D
#define		RS_NUM			4
#define		RW_PORT			C
#define		RW_NUM			5
#define		EN_PORT			C
#define		EN_NUM			4
#define		DB4_PORT		C
#define		DB4_NUM			3
#define		DB5_PORT		C
#define		DB5_NUM			2
#define		DB6_PORT		C
#define		DB6_NUM			1
#define		DB7_PORT		C
#define		DB7_NUM			0

#if(LCD_BIT_MODUS == 8)
#define		DB4_PORT		D
#define		DB4_NUM			3
#define		DB5_PORT		D
#define		DB5_NUM			4
#define		DB6_PORT		D
#define		DB6_NUM			5
#define		DB7_PORT		D
#define		DB7_NUM			6
#endif

// Ab hier nichts mehr anpassen
#define		RSPORT			PORT(RS_PORT)
#define		RSDDR			DDR(RS_PORT)
#define		RS				RS_NUM

#define		RWPORT			PORT(RW_PORT)
#define		RWDDR			DDR(RW_PORT)
#define		RW				RW_NUM

#define		ENPORT			PORT(EN_PORT)
#define		ENDDR			DDR(EN_PORT)
#define		EN				EN_NUM

#define		DB4PORT			PORT(DB4_PORT)
#define		DB4DDR			DDR(DB4_PORT)
#define		DB4PIN			PIN(DB4_PORT)
#define 	DB4				DB4_NUM

#define		DB5PORT			PORT(DB5_PORT)
#define		DB5DDR			DDR(DB5_PORT)
#define		DB5PIN			PIN(DB5_PORT)
#define 	DB5				DB5_NUM

#define		DB6PORT			PORT(DB6_PORT)
#define		DB6DDR			DDR(DB6_PORT)
#define		DB6PIN			PIN(DB6_PORT)
#define 	DB6				DB6_NUM

#define		DB7PORT			PORT(DB7_PORT)
#define		DB7DDR			DDR(DB7_PORT)
#define		DB7PIN			PIN(DB7_PORT)
#define 	DB7				DB7_NUM

#if(LCD_BIT_MODUS == 8)
#define		DB0PORT			PORT(DB0_PORT)
#define		DB0DDR			DDR(DB0_PORT)
#define		DB0PIN			PIN(DB0_PORT)
#define 	DB0				DB0_NUM

#define		DB1PORT			PORT(DB1_PORT)
#define		DB1DDR			DDR(DB1_PORT)
#define		DB1PIN			PIN(DB1_PORT)
#define 	DB1				DB1_NUM

#define		DB2PORT			PORT(DB2_PORT)
#define		DB2DDR			DDR(DB2_PORT)
#define		DB2PIN			PIN(DB2_PORT)
#define 	DB2				DB2_NUM

#define		DB3PORT			PORT(DB3_PORT)
#define		DB3DDR			DDR(DB3_PORT)
#define		DB3PIN			PIN(DB3_PORT)
#define 	DB3				DB3_NUM
#endif

// Makros
#define		BEFEHLSMODUS	RSPORT	&= ~(1<<RS)
#define		DATENMODUS		RSPORT 	|= 	(1<<RS)

#define		SCHREIBEN		RWPORT	&= ~(1<<RW)
#define		LESEN			RWPORT 	|=	(1<<RW)

#define		E_LOW			ENPORT 	&= ~(1<<EN)
#define		E_HIGH			ENPORT 	|=	(1<<EN)



// Funktionen
void lcd_enable(void);
void lcd_busycheck(void);
void lcd_clear_all_bits(void);
void lcd_set_all_bits(void);
void lcd_transfer (uint8_t data);
void lcd_cgwrite(uint8_t data);
void lcd_send(uint8_t data, uint8_t dat);
void lcd_puts(char *strin);
void lcd_clear(void);
void lcd_cursorhome(void);
void lcd_arrize(int32_t zahl, char *feld, uint8_t digits, int8_t vorzeichen);
uint8_t lcd_getaddr(void);
uint8_t lcd_cursorread(void);
void lcd_cursorset(uint8_t zeile, uint8_t spalte);
void lcd_init(void);





#endif
