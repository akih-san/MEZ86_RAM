/*
 *  This source is for PIC18F47Q43 UART, I2C, SPI and TIMER0
 *
 * Base source code is maked by @hanyazou
 *  https://twitter.com/hanyazou
 *
 * Redesigned by Akihito Honda(Aki.h @akih_san)
 *  https://twitter.com/akih_san
 *  https://github.com/akih-san
 *
 *  Target: MEZ86__RAM
 *  Date. 2026.7.1
*/

#define BOARD_DEPENDENT_SOURCE

#include "../mez86.h"

//#define clk5m
//#define clk8m
#define clk9_10m

// console input buffers
#define U3B_SIZE 128

unsigned char rx_buf[U3B_SIZE];	//UART Rx ring buffer
unsigned int rx_wp, rx_rp, rx_cnt;

TPB tim_pb;			// TIME device parameter block

// RTC DS1307 format
// rtc[0] : seconds (BCD) 00-59
// rtc[1] : minuts  (BCD) 00-59
// rtc[2] : hours   (BCD) 00-23 (or 1-12 when bit6=1. bit5: AM(0)/PM(1) )
// rtc[3] : day     (BCD) week day 01-07
// rtc[4] : date    (BCD) 01-31
// rtc[5] : month   (BCD) 01-12
// rtc[6] : year    (BCD) 00-99 : range : (19)80-(19)99, (20)00-(20)79
uint8_t rtc[7];

static uint8_t cnt_sec;		// sec timer (1000ms = 10ms * 100)

void enable_int(void) {
	GIE = 1;             // Global interrupt enable
	TMR0IF = 0;			// Clear timer0 interrupt flag
	TMR0IE = 1;			// Enable timer0 interrupt
}

void enable_clc4int(void) {
	CLC4IF = 0;			// Clear CLC4 interrupt flag
	CLC4IE = 1;			// Enable CLC4 interrupt
}

void disable_clc4int(void) {
	CLC4IF = 0;			// Clear CLC4 interrupt flag
	CLC4IE = 0;			// disable CLC4 interrupt
}

//initialize TIMER0 & TIM device parameter block
void timer0_init(void) {
	
	uint16_t year, month, date;

	cnt_sec = 0;	// set initial adjust timer counter
	tim_pb.TIM_DAYS = TIM20250601;		//set 2025.06.01
	tim_pb.TIM_MINS = 0;
	tim_pb.TIM_HRS = 0;
	tim_pb.TIM_SECS = 0;
	tim_pb.TIM_HSEC = 0;

	// convert number of days to year, month and date
	cnv_ymd(tim_pb.TIM_DAYS, &year, &month, &date );
	// convert bin to BCD
	rtc[0] = cnv_bcd(tim_pb.TIM_SECS);
	rtc[1] = cnv_bcd(tim_pb.TIM_MINS);
	rtc[2] = cnv_bcd(tim_pb.TIM_HRS);
	rtc[4] = cnv_bcd((uint8_t)date);
	rtc[5] = cnv_bcd((uint8_t)month);
	rtc[6] = cnv_bcd((uint8_t)year);
}

void cvt_bcd_bin(void) {
	uint16_t year, month, date;
	// convert BCD to bin
	rtc[0] &=0x7f;		// mask bit 7(CH: clock disable bit)

	TMR0IE = 0;			// disable timer0 interrupt
	tim_pb.TIM_SECS = cnv_byte(rtc[0]);
	tim_pb.TIM_MINS = cnv_byte(rtc[1]);
	tim_pb.TIM_HRS  = cnv_byte(rtc[2]);
	date  = (uint16_t)cnv_byte(rtc[4]);
	month = (uint16_t)cnv_byte(rtc[5]);
	year  = (uint16_t)cnv_byte(rtc[6]);
	if (year >= 80) year += 1900;
	else year += 2000;

	// convert year, month and date to number of days from 1980
	tim_pb.TIM_DAYS = days_from_1980(year, month, date);
	TMR0IE = 1;			// Enable timer0 interrupt
}

int cnv_rtc_tim(void) {
	if ( read_I2C(DS1307, 0, 7, &rtc[0]) == 0xFF) return 1;
	cvt_bcd_bin();
	return 0;
}

void datcnv_tim_rtc(void) {
	uint16_t year, month, date;
	
	cnv_ymd(tim_pb.TIM_DAYS, &year, &month, &date );
	// convert bin to BCD
	rtc[0] = cnv_bcd(tim_pb.TIM_SECS);
	rtc[1] = cnv_bcd(tim_pb.TIM_MINS);
	rtc[2] = cnv_bcd(tim_pb.TIM_HRS);
	rtc[4] = cnv_bcd((uint8_t)date);
	rtc[5] = cnv_bcd((uint8_t)month);
	rtc[6] = cnv_bcd((uint8_t)year);
}

static void base_pin_definition()
{
    // System initialize
    OSCFRQ = 0x08;      // 64MHz internal OSC

	// Disable analog function
    ANSELA = 0x00;
    ANSELB = 0x00;
    ANSELC = 0x00;
    ANSELD = 0x00;
    ANSELE0 = 0;
    ANSELE1 = 0;
    ANSELE2 = 0;

    // RESET output pin
	LAT(I86_RESET) = 1;        // Reset
    TRIS(I86_RESET) = 0;       // Set as output

	// HOLDA
	WPU(I86_HOLDA) = 0;     // disable week pull up
	TRIS(I86_HOLDA) = 1;    // Set as input

	// ALE
	WPU(I86_ALE) = 0;     // disable week pull up
	LAT(I86_ALE) = 0;
    TRIS(I86_ALE) = 0;    // Set as output for PIC R/W RAM

	// Init address LATCH to 0
	// Address bus A7-A0 pin
    WPU(I86_ADBUS_L) = 0xff;       // Week pull up
    LAT(I86_ADBUS_L) = 0x00;
    TRIS(I86_ADBUS_L) = 0x00;      // Set as output

	// Address bus A15-A8 pin
    WPU(I86_ADBUS_H) = 0xff;       // Week pull up
    LAT(I86_ADBUS_H) = 0x00;
    TRIS(I86_ADBUS_H) = 0x00;      // Set as output

	WPU(I86_A16) = 1;     // A16 Week pull up
	LAT(I86_A16) = 1;     // init A16=0
    TRIS(I86_A16) = 0;    // Set as output

	WPU(I86_A17) = 1;     // A17 Week pull up
	LAT(I86_A17) = 1;     // init A17=0
    TRIS(I86_A17) = 0;    // Set as output

	// NMI definition
	WPU(I86_NMI) = 0;     // disable week pull up
	PPS(I86_NMI) = 0;     // set as latch port
	LAT(I86_NMI) = 0;     // NMI=0
	TRIS(I86_NMI) = 0;    // Set as output

	// HOLD output pin
	WPU(I86_HOLD) = 0;	 // disable week pull up
    LAT(I86_HOLD) = 1;
    TRIS(I86_HOLD) = 0;  // Set as output
	
	// #BHE
	WPU(I86_BHE) = 1;     // week pull up
	LAT(I86_BHE) = 1;     // BHE disactive
	TRIS(I86_BHE) = 0;    // Set as onput
	
	// M/IOX
	WPU(I86_MIOX) = 1;     // week pull up
	LAT(I86_MIOX) = 1;     // memory CE2 active
	TRIS(I86_MIOX) = 0;    // Set as onput

	// #WR output pin
	WPU(I86_WRX) = 1;		// /WR Week pull up
    LAT(I86_WRX) = 1;		// disactive
    TRIS(I86_WRX) = 0;		// Set as output

	// #RD output pin
	WPU(I86_RDX) = 1;		// /WR Week pull up
    LAT(I86_RDX) = 1;		// disactive
    TRIS(I86_RDX) = 0;		// Set as output

	// #SPI_SS
	WPU(SPI_SS) = 1;		// SPI_SS Week pull up
	LAT(SPI_SS) = 1;		// set SPI disable
	TRIS(SPI_SS) = 0;		// Set as onput

	WPU(I86_CLK) = 0;		// disable week pull up
	LAT(I86_CLK) = 1;		// 8088_CLK = 1
    TRIS(I86_CLK) = 0;		// set as output pin
	
	//INTR
	WPU(I86_INTR) = 0;		// disable week pull up
	LAT(I86_INTR) = 0;		// INTR = 0
    TRIS(I86_INTR) = 0;		// set as output pin
	
	//INTAX
	WPU(I86_INTAX) = 1;		// enable week pull up
	LAT(I86_INTAX) = 1;		// INTAX = 1
    TRIS(I86_INTAX) = 1;	// set as input pin
}
	
// UART3 Transmit
// if TXIF is busy, return status BUSY(not ZERO)
//
uint8_t out_chr(char c) {
	if (!U3TXIF) return 1;		// retrun BUSY
    U3TXB = c;                  // Write data
	return 0;
}

// UART3 Transmit
void putch(char c) {
    while(!U3TXIF);             // Wait or Tx interrupt flag set
    U3TXB = c;                  // Write data
}

// UART3 Recive
int getch(void) {
	char c;

	while(!rx_cnt);             // Wait for Rx interrupt flag set
	GIE = 0;                // Disable interrupt
	c = rx_buf[rx_rp];
	rx_rp = (rx_rp + 1) & ( U3B_SIZE - 1);
	rx_cnt--;
	GIE = 1;                // enable interrupt
    return c;               // Read data
}

// clear rx buffer and enable rx interrupt
void clr_uart_rx_buf(void) {
	rx_wp = 0;
	rx_rp = 0;
	rx_cnt = 0;
    U3RXIE = 1;          // Receiver interrupt enable
}

unsigned int get_str(char *buf, uint8_t cnt) {
	unsigned int c, i;
	
	U3RXIE = 0;					// disable Rx interruot
	i = ( (unsigned int)cnt > rx_cnt ) ? rx_cnt : (unsigned int)cnt;
	c = i;
	while(i--) {
		*buf++ = rx_buf[rx_rp];
		rx_rp = (rx_rp + 1) & ( U3B_SIZE - 1);
		rx_cnt--;
	}
	U3RXIE = 1;					// enable Rx interruot
	return c;
}

//
// define interrupt
//
// Never called, logically
void __interrupt(irq(default),base(8)) Default_ISR(){}

////////////// TIMER0 vector interrupt ////////////////////////////
//TIMER0 interrupt
/////////////////////////////////////////////////////////////////
void __interrupt(irq(TMR0),base(8)) TIMER0_ISR(){

	TMR0IF =0; // Clear timer0 interrupt flag

//	if (!time_dev) {
		if (++cnt_sec == 100) {
			cnt_sec = 0;
			
			if( ++tim_pb.TIM_SECS == 60 ) {
				tim_pb.TIM_SECS = 0;
				if ( ++tim_pb.TIM_MINS == 60 ) {
					tim_pb.TIM_MINS = 0;
					if ( ++tim_pb.TIM_HRS == 24 ) {
						tim_pb.TIM_HRS = 0;
						tim_pb.TIM_DAYS++;
					}
				}
			}
			tim_pb.TIM_HSEC = 0;
		}
//	}

	// IRQ request
	if (irq_flg) irq_sig = 1;
}

////////////// UART3 Receive interrupt ////////////////////////////
// UART3 Rx interrupt
// PIR9 (bit0:U3RXIF bit1:U3TXIF)
/////////////////////////////////////////////////////////////////
void __interrupt(irq(U3RX),base(8)) URT3Rx_ISR(){

	unsigned char rx_data;

	rx_data = U3RXB;			// get rx data

	if ( ctlq_ev == CTL_Q && rx_data == CTL_Q ) nmi_sig = 1;
	if (rx_cnt < U3B_SIZE) {
		ctlq_ev = (uint8_t)rx_data;
		rx_buf[rx_wp] = rx_data;
		rx_wp = (rx_wp + 1) & (U3B_SIZE - 1);
		rx_cnt++;
	}
}

//
// CLC4 interrupt. Detect second INTA falling edge
// Make int_vec type vector
//
void __interrupt(irq(CLC4),base(8)) CLC_ISR(){
	CLC4IF = 0;						// Clear CLC4 interrupt flag
	TRIS(I86_ADBUS_L) = 0x00;		// Set as output
	LAT(I86_ADBUS_L) = (unsigned char)int_vec;		//interrupt vector
#ifdef clk5m
	while (R(I86_INTAX)) {};
	while (!R(I86_INTAX)) {};
#endif
#ifdef clk8m
	while (!R(I86_INTAX)) {};
	while (!R(I86_INTAX)) {};
#endif
#ifdef clk9_10m
	while (!R(I86_INTAX)) {};
#endif
	TRIS(I86_ADBUS_L) = 0xff;		// Set as input

	LAT(I86_INTR) = 0;
}

void Check_integrity(void) {

	int flg = 0;

	if ( intr_req ) {
		#ifdef clk5m
		if ( clk_fs != 5 ) flg = 1;
		#endif

		#ifdef clk8m
		if ( clk_fs != 8 ) flg = 1;
		#endif
		
		#ifdef clk9_10m
		if ( !(clk_fs == 9 || clk_fs == 10) ) flg = 1;
		#endif
	
		if (!flg) {
			if ( clk_fs == 12 || clk_fs == 16 ) flg = 1;
		}
	}
	
	if ( flg ) {
		printf("Warrning! SET IRQ OFF. Check binarry code and %s file.\r\n", conf);
		intr_req = 0;
	}
}

uint32_t get_physical_addr(uint16_t ah, uint16_t al)
{
// real 32 bit address
//	return (uint32_t)ah*0x1000 + (uint32_t)al;

// 8086 : segment:offset
	return (uint32_t)ah*0x10 + (uint32_t)al;
}

static union address_bus_u ab;

static void write_byte(uint8_t dat, uint8_t flg )	// even : flg = 1, odd : flg = 0
{
	LAT(I86_ADBUS_L) = ab.ll;
	LAT(I86_ADBUS_H) = ab.lh;
	LAT(I86_A16) = ab.hl & 0x01;
	LAT(I86_A17) = (ab.hl & 0x02) >> 1;
	LAT(I86_A18) = (ab.hl & 0x04) >> 2;
	LAT(I86_A19) = (ab.hl & 0x08) >> 3;
	LAT(I86_BHE) = (__bit)flg;
	// Latch address(A0 - A19) & BHE by 74LS373
	LAT(I86_ALE) = 1;
	LAT(I86_ALE) = 0;
	
	// write byte data to sram
	LAT(I86_WRX) = 0;					// activate /WE
	if ( flg ) {		// even
		LAT(I86_ADBUS_L) = dat;
	}
	else {			// odd
		LAT(I86_ADBUS_H) = dat;
	}
	LAT(I86_WRX) = 1;					// deactivate /WE
}

static void write_word(uint8_t *buf)
{
	LAT(I86_ADBUS_L) = ab.ll;
	LAT(I86_ADBUS_H) = ab.lh;
	LAT(I86_A16) = ab.hl & 0x01;
	LAT(I86_A17) = (ab.hl & 0x02) >> 1;
	LAT(I86_A18) = (ab.hl & 0x04) >> 2;
	LAT(I86_A19) = (ab.hl & 0x08) >> 3;
	LAT(I86_BHE) = 0;		// enable BHE
	// Latch address(A0 - A19) & BHE by 74LS373
	LAT(I86_ALE) = 1;
	LAT(I86_ALE) = 0;
	
	// write byte data to sram
	LAT(I86_WRX) = 0;					// activate /WE
	LAT(I86_ADBUS_L) = *buf++;			// set even byte data
	LAT(I86_ADBUS_H) = *buf;			// set odd byte data
	LAT(I86_WRX) = 1;					// deactivate /WE
}

void write_sram(uint32_t addr, uint8_t *buf, unsigned int len)
{
	uint8_t flg;
	
	ab.w = addr;

	while( len ) {
		if ( len == 1 ) {
			flg = (ab.ll & 1) ? 0 : 1;
			write_byte(*buf, flg );
			return;
		}
		else {
			if (ab.ll & 1) {	// odd address
				write_byte(*buf, 0 );		// write odd data
				buf++;
				ab.w++;
				len--;
			}
			else {
				write_word(buf);
				buf += 2;
				ab.w +=2;
				len -=2;
			}
		}
	}
}


static void read_byte(uint8_t *buf, uint8_t flg )	// even : flg = 1, odd : flg = 0
{
	LAT(I86_ADBUS_L) = ab.ll;
	LAT(I86_ADBUS_H) = ab.lh;
	LAT(I86_A16) = ab.hl & 0x01;
	LAT(I86_A17) = (ab.hl & 0x02) >> 1;
	LAT(I86_A18) = (ab.hl & 0x04) >> 2;
	LAT(I86_A19) = (ab.hl & 0x08) >> 3;
	LAT(I86_BHE) = (__bit)flg;
	// Latch address(A0 - A19) & BHE by 74LS373
	LAT(I86_ALE) = 1;
	LAT(I86_ALE) = 0;
	
	if ( flg ) {		// even
		TRIS(I86_ADBUS_L) = 0xff;		// Set as input
		LAT(I86_RDX) = 0;				// activate /OE
		*buf = PORT(I86_ADBUS_L);
		LAT(I86_RDX) = 1;				// disable /OE
		TRIS(I86_ADBUS_L) = 0x00;		// Set as output
	}
	else {			// odd
		TRIS(I86_ADBUS_H) = 0xff;		// Set as input
		LAT(I86_RDX) = 0;				// activate /OE
		*buf = PORT(I86_ADBUS_H);
		LAT(I86_RDX) = 1;				// disable /OE
		TRIS(I86_ADBUS_H) = 0x00;		// Set as output
	}
}

static void read_word(uint8_t *buf)
{
	LAT(I86_ADBUS_L) = ab.ll;
	LAT(I86_ADBUS_H) = ab.lh;
	LAT(I86_A16) = ab.hl & 0x01;
	LAT(I86_A17) = (ab.hl & 0x02) >> 1;
	LAT(I86_A18) = (ab.hl & 0x04) >> 2;
	LAT(I86_A19) = (ab.hl & 0x08) >> 3;
	LAT(I86_BHE) = 0;		// enable BHE
	// Latch address(A0 - A19) & BHE by 74LS373
	LAT(I86_ALE) = 1;
	LAT(I86_ALE) = 0;
	
	TRIS(I86_ADBUS_L) = 0xff;		// Set as input
	TRIS(I86_ADBUS_H) = 0xff;		// Set as input
	LAT(I86_RDX) = 0;				// activate /OE
	*buf = PORT(I86_ADBUS_L);
	buf++;
	*buf = PORT(I86_ADBUS_H);
	LAT(I86_RDX) = 1;				// disable /OE
	TRIS(I86_ADBUS_L) = 0x00;		// Set as output
	TRIS(I86_ADBUS_H) = 0x00;		// Set as output
}

void read_sram(uint32_t addr, uint8_t *buf, unsigned int len)
{
	uint8_t flg;
	
	ab.w = addr;
	while( len ) {
		if ( len == 1 ) {
			flg = (ab.ll & 1) ? 0 : 1;
			read_byte(buf, flg );
			return;
		}
		else {
			if (ab.ll & 1) {	// odd address
				read_byte(buf, 0 );		// write odd data
				buf++;
				ab.w++;
				len--;
			}
			else {
				read_word(buf);
				buf += 2;
				ab.w +=2;
				len -=2;
			}
		}
	}
}

static void wait_for_programmer()
{
    //
    // Give a chance to use PRC (RB6) and PRD (RB7) to PIC programer.
    //
    printf("\n\r");
    printf("wait for programmer ...\r");
    __delay_ms(200);
    printf("                       \r");

    printf("\n\r");
}
