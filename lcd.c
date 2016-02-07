#define F_CPU 16000000UL //16 MHz
#include <avr/io.h>
#include <util/delay.h>

// Defining Ports and Pins 
#define RS_port PORTB  //RS_bit is for register selection
#define RS_DDR DDRB
#define RS_bit PORTB0

#define E_bit PORTB1	//E_bit is the Enable bit
#define E_port PORTB
#define E_DDR DDRB

#define D4_DDR DDRD		//D4 pin connections
#define D4_port PORTD
#define D4_bit PORTD4

#define D5_DDR DDRD		//D5 pin connections
#define D5_port PORTD
#define D5_bit PORTD5

#define D6_DDR DDRD		//D6 pin connections
#define D6_port PORTD
#define D6_bit PORTD6

#define D7_DDR DDRD		//D7 pin connections
#define D7_port PORTD
#define D7_bit PORTD7

// Bit values
uint8_t line_one = 0x00;          //start of line 1
uint8_t Reset = 0b00110000;       //reset the lcd
uint8_t Set4_bit = 0b00101000;    //4 bit data, 2-line display, 5x7 font 
uint8_t Display_off = 0b00001000; //turn display off
uint8_t Display_on = 0b00001100;  //turn display on 
uint8_t Entry_mode = 0b00000110;  //shift cursor from left to right
uint8_t clear = 0b00000001;		  //clear the lcd i.e replace all characters with ASCII space
uint8_t home = 0b00000010;		  //set cursor to home position i.e first position of first line
uint8_t cursor_set = 0b10000000;  //set cursor position

// Defining function prototypes
void print_string(uint8_t *);
void write_character(uint8_t);
void send_data(uint8_t);
void initialise(void);
void write_instruction(uint8_t);

int main()
{
	//4 data lines
	D7_DDR |= (1<<D7_bit); //Sets PD7 as output
	D6_DDR |= (1<<D6_bit); //Sets PD6 as output
	D5_DDR |= (1<<D5_bit); //Sets PD5 as output
	D4_DDR |= (1<<D4_bit); //Sets PD4 as output

	E_DDR |= (1<<E_bit);	//sets enable pin as output
	RS_DDR |= (1<<RS_bit);	//sets Register_select pin as output

	uint8_t Name1[] = "Akash Deep Singh"; //String Name1
	uint8_t Name2[] = "Vahini Ummalaneni";//String Name2

	initialise();
	while(1)
	{
		print_string(Name1);		       //Prints Name1
		_delay_ms(2000);				   //Wait for 2s
		write_instruction(clear);          //clear the screen
		_delay_ms(4);					   //wait time
		print_string(Name2);		       //Prints Name2
		_delay_ms(2000);				   //Wait for 2s
		write_instruction(clear);          //clear the screen
		_delay_ms(4);					   //wait time

	}
	return 0;
}

void initialise(void) 				//make the LCD ready for 4-bit data interface
{
	_delay_ms(20);   				//Power-up delay
	RS_port &= ~(1<<RS_bit);		//RS-bit low i.e selecting the Instruction Register
	E_port &= ~(1<<E_bit);			//Enable low

	//3-step reset sequence as mentioned in the Instruction manual

	send_data(Reset);				//Step-1
	_delay_ms(5);

	send_data(Reset);				//Step-2
	_delay_us(200);

	send_data(Reset);				//Step-3
	_delay_us(200);

	send_data(Set4_bit);			//4-bit interface set-up
	_delay_ms(5);					//Data up-time

	write_instruction(Set4_bit);	//Set lines,font and mode
	_delay_ms(5);					//Data up-time

	write_instruction(Display_off);	//Turn off the display
	_delay_ms(5);					//Delay

	write_instruction(clear);		//Clear everyhthing from screen i.e empty the display RAM
	_delay_ms(4);					//Delay

	write_instruction(Entry_mode);	//Instruct the chip how you want to enter the data i.e left-to-right / right-to-left
	_delay_us(80);					//We have chosen left-to-right mode i.e the cursor moves from left-to-right

	write_instruction(Display_on);	//Finally, turn the display-on so that we can start printing the data
	_delay_us(80);					//Delay
}

void print_string(uint8_t String[])	//Print a string on the LCD 
{
	volatile int a = 0;				//create a volatile integer a and initialise it with 0
	while(String[a] != 0)			// Check if the string is not empty or we have reached the end of the string
	{
		write_character(String[a]); //Write the character at the index a of the string
		a += 1;						//increase the value of a by 1 i.e move to the next character(if any) in the string
		_delay_ms(5);				//Data set-up time
	}
}

void write_character(uint8_t character)	//Sends a byte of information to the LCD data register
{
	RS_port |= (1<<RS_bit);				//RS-bit high i.e selecting the Data Register
	E_port |= (1<<E_bit);				//Enable low
	send_data(character);				//Write upper-4 digits of data
	//Since we have only 4-bit interface, we can only send 4-bits at a time but the data is 8-bits. So we right shift the data and send the lower 4-bits
	send_data(character << 4);			//Right shift by 4 bits and write the lower 4 bits of data
}

void write_instruction(uint8_t Instruction) //Sends a byte of information to LCD instruction register
{
	RS_port &= ~(1 << RS_bit);				//RS-bit low i.e selecting the Instruction Register
	E_port &= ~(1 << E_bit);				//Enable low
	send_data(Instruction);					//Write upper-4 digits of data
	//Since we have only 4-bit interface, we can only send 4-bits at a time but the data is 8-bits. So we right shift the data and send the lower 4-bits
	send_data(Instruction << 4);			//Right Shift by 4 bits and write the lower 4 bits of data
}

void send_data(uint8_t Byte)				//Sends a byte of information to the LCD
{
	//Assuming that the Bit is 0
	D7_port &= ~(1<<D7_bit);				// & operation on D7 and ~D7
	if(Byte & 1<<7) D7_port |= (1<<D7_bit); // making the bit 1 if necessary

	D6_port &= ~(1<<D6_bit);				// & operation on D7 and ~D7
	if(Byte & 1<<6) D6_port |= (1<<D6_bit); // making the bit 1 if necessary

	//repeat the above steps for the rest of bits

	D5_port &= ~(1<<D5_bit);
	if(Byte & 1<<5) D5_port |= (1<<D5_bit);

	D4_port &= ~(1<<D4_bit);
	if(Byte & 1<<4) D4_port |= (1<<D4_bit);

	E_port |= (1<<E_bit);					//Enable pin-high
	_delay_us(1);							//Data set-up time
	E_port &= ~(1<<E_bit);					//Enable pin-low
	_delay_us(1);							//Data set-up time
}