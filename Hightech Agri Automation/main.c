#include<lpc21xx.h>
#include<string.h>
#include"lcd.h"

#define Motor (1 << 17)
#define valve1 (1 << 18)
#define valve2 (1 << 19)
#define soil_moisture 30

void UART_CONFIG(void);
void UART_TX(unsigned char ch);
void UART0_STR(const char *str);
unsigned char UART_RX(void);

int main()
{
    char buffer[256], ch; // Buffer to store the received SMS
    int index = 0, startmsg = 0,soil=1; // Flag to indicate the start of the message

    LCD_INIT();
    

    IODIR0 |= Motor | valve1 | valve2;
    IOSET0 |= Motor | valve1 | valve2;

    LCD_COMMAND(0XC0);
    LCD_STR("V25CE2S2");
	delay_milliseconds(4000);

    LCD_COMMAND(0X80);
    LCD_SCROLL("HIGH TECH AGRI AUTOMATION");

    // Initialize GSM module
	 UART_CONFIG();
    UART0_STR("AT\r\n"); // Test AT communication
    delay_milliseconds(1000);

    UART0_STR("AT+CMGF=1\r\n"); // Set SMS mode to text
    delay_milliseconds(1000);

    UART0_STR("AT+CNMI=1,2,0,0,0\r\n"); // Enable SMS notifications
    delay_milliseconds(1000);

    // Send SMS

    UART0_STR("AT+CMGS=\"+918825562004\"\r\n"); // Replace with your mobile number
    delay_milliseconds(1000);

    UART0_STR("WAITING FOR SMS"); // Message content

	LCD_COMMAND(0X01);
	LCD_COMMAND(0X80);
	LCD_STR("WAITING FOR SMS");

    UART_TX(26); // Send Ctrl+Z to send SMS
    delay_milliseconds(3000);

    // Wait for SMS and read it

    while (1)
    {
        ch = UART_RX(); // Read a character from UART
		if(ch == '\n' || ch == '\r')
		{  
            if(index == 0)
	        continue; // Ignore empty lines
		 
            buffer[index] = '\0'; // Null-terminate the string
		 
            if(strstr(buffer, "+CMT:")!=0)
		    {
		      // SMS header detected, next line contains the actual message		 

		      startmsg = 1;
              index = 0; // Reset index for storing the message
              continue;
            }

            if(startmsg == 1)
            {
                // Message received after header

                UART0_STR("Received SMS: ");
				UART0_STR(buffer); // Display the received SMS

				LCD_COMMAND(0X01);
				LCD_COMMAND(0x80);// display on LCD

				LCD_STR((unsigned char *)buffer);

                UART0_STR("\r\n");

                startmsg = 0; // Reset flag
            }
            index = 0; // Reset buffer index for next line
        }
        else if (index < sizeof(buffer) - 1)
        {
            buffer[index++] = ch; // Store character in buffer
        }
		if((((IOPIN0 >> soil_moisture) & 1) == 0) && (soil == 1))
		{
    LCD_COMMAND(0x01);
    LCD_COMMAND(0x80);
    LCD_STR("SOIL IS DRY");

    UART0_STR("AT+CMGS=\"+918825562004\"\r\n");
    delay_milliseconds(1000);

    UART0_STR("ALERT: SOIL IS DRY");
    UART_TX(26);   // Ctrl+Z ? SEND SMS

    soil = 0;      // Send only once
}
		if(strstr(buffer,"motoron"))// Motor control
		{
			IOCLR0 = Motor;
			delay_milliseconds(300);
		}
		else if(strstr(buffer,"motoroff"))
		{
			IOSET0 = Motor;
			delay_milliseconds(300);
			soil=1;
		}
		else if(strstr(buffer, "valve1on"))// Pump control
		{
			IOCLR0 = valve1;
			delay_milliseconds(300);
		}
		else if(strstr(buffer, "valve1off"))
		{
			IOSET0 = valve1;
			delay_milliseconds(300);
		}
		else if(strstr(buffer, "valve2on"))// Alarm control
		{
			IOCLR0 = valve2;
			delay_milliseconds(300);
		}
		else if(strstr(buffer, "valve2off"))
		{
			IOSET0 = valve2;
			delay_milliseconds(300);
		}
		else if(strstr(buffer, "alloff"))
		{
			IOSET0 = Motor | valve1 | valve2;
			LCD_COMMAND(0X01);
			LCD_COMMAND(0X80);
			LCD_STR("MT OFF");
			LCD_COMMAND(0X89);
			LCD_STR("VL1 OFF");
			LCD_COMMAND(0XC4);
			LCD_STR("VL2 OFF");
			delay_milliseconds(300);
		}
		else if(strstr(buffer, "allon"))
		{
			IOCLR0 = Motor | valve1 | valve2;
			LCD_COMMAND(0X01);
			LCD_COMMAND(0X80);
			LCD_STR("MT ON");
			LCD_COMMAND(0X89);
			LCD_STR("VL1 ON");
			LCD_COMMAND(0XC4);
			LCD_STR("VL2 ON");
			delay_milliseconds(300);
		}
    }
}  
void UART_CONFIG(void)
{
    PINSEL0|= 0X5;       // Configure P0.0 and P0.1 as TXD0 and RXD0
    U0LCR = 0X83;        // Enable DLAB to set baud rate
    U0DLL = 97;          // Set baud rate to 9600 (assuming 15 MHz clock)
    U0DLM = 0;
    U0LCR = 0X03;        // Disable DLAB
}
void UART_TX(unsigned char d)
{
    while ((U0LSR & (1 << 5)) == 0); // Wait until THR is empty
    U0THR = d;
}
void UART0_STR(const char *s)
{
    while (*s)
        UART_TX(*s++);
}
unsigned char UART_RX(void)
{
    while ((U0LSR & 0x01) == 0); // Wait until data is received
    return U0RBR;                // Return received data
}
