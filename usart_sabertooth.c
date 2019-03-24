
//PB_10 ==Tx  USART  //GPIO Pin Configuration = Alternate Function Push Pull Output  //USART becomes half duplex UART when Rx=disabled
//PB_11 ==Rx  USART  //ignore for sabertooth
//PB10 & PB11 come under USART3 which uses APB2

#include "stm32f10x.h"

#define gpio_init_flag 1
#define uart_init_flag 1
//#define usart_dr_check 1 

void pin_tx_init(void);
void uart_tx_init(void);
//void uart_transmit(void);
//int flag_check();

uint32_t usart_sr = 0;


int main()
{
		pin_pb10_tx_init();                                            //initialize gpio & required clocks
		uart_pb10_tx_init();

		usart_sr = USART->SR ;										   //copy value of SR register as a 32bit unsigned integer 

		//send data 1 //this hex code is written into usart_dr register's bits [8:0]
		//while TXE bit is zero & TC bit is 1
		//according to one website, usart_dr is written when TXE bit is 0.   !!!CHECK!!!
		while(usart_sr >= 64)                                          // page 818 ref_man
		    { 
		    	USART->DR |= 0x01;                                     // page 820 ref_man          
		    }	

}

/**************************************************************************************************************************************/

pin_pb10_tx_init()
{

                                                 //Inintializing RCC & GPIO Pin(PB10)



		//enable clock for APB2 //port B uses APB2
		RCC->APB2ENR |= 0x00000009;                                    // page 146 ref_man



		//enable clock for APB2 //PB10 is Serial3/USART3
		//Serial3 uses APB1
		RCC->APB1ENR |= 0x00040000;                                    // page 148 ref_man
		


		//we are using pin 10 which lies in CRH register
		//CNF10-bits 11,10 //MODE10-bits 9,8
		//instead of 0xb0000101100000000 we can also use  **********(  (0<<8) | (1<<9) | (0<<10) | (1<<11)  );**********
		//bit 8=1 //bit 9=1 //bit 10=0 //bit 11=1
		GPIOB->CRH |= 0x00000B00;                                       // page 172 ref_man



		//return gpio_init_flag;
}

/**************************************************************************************************************************************/		

uart_pb10_tx_init()
{
                                        // Configuring GPIO Pin (PB10)for UART //page 787, 791, 807 ref_man



		// set word length using bit 12, enable usart using bit 13, enable transmitter using bit 3
		//write 0 to bit 12 & write 1 to bits 3, 13	
		USART->CR1 |= (  (1<<3) | (0<<12) | (1<<13)  );                 // page 821 ref_man



		//adjusting number of stop bits in bits 13 & 12 of USART_CR2 & selecting asynchronous mode
		//note at the bottom of the page states bits 10, 9, 8 are not to be written while transmitter is enabled 
		//this means we can't write even '0' in these bits , hence we just shift required bits instead of  writing those bits  
		//writing 1 to CLKEN [bit 11] in usart_cr2 selects synchronous mode.writing 0 , therefore will select asynchronous mode
		//we also have to clear LINEN [bit 14] in USART_CR2
		USART->CR2 |= (   (0<<11) | (0<<12) | (0<<13) | (0<<14)  );     // page 824 ref_man



		//on page 807 of ref_man it states that SCEN & IREN [bits 5, 1] have to be disabled
		//on page 807 of ref_man it states that HDSEL is to be enabled by writing 1 for half duplex mode [bit 3]
		USART->CR3 |= (  (0<<1) | (1<<3) | (0<<5)  );



		//value to be prgrammed in BRR is (468.75) for baud rate 9600(9.6kbps) refer the table on page 798 ref_man
		//USART3 is being used. USART 3 comes under PCLK1
		//For configuring BAUD rate , refer page 789 , 798 of ref_man
		//USARTDIV = DIV_Mantissa + (DIV_Fraction / 16)
		//Tx/ Rx baud =Fck/(16*USARTDIV)		
		//legend:   Fck - Input clock to the peripheral (PCLK1 for USART2, 3, 4, 5 or PCLK2 for USART1)
		//converting baud rate to kbps :  https://www.calculator.org/properties/data_rate.html
		//register bit map is on page 820 ref_man
		//here we have ignored the fractional part and have written 469 to BRR after roounding of the calculated value 468.75 to 469
		USART->BRR |= (0x0000) | (  (1<<12) | (1<<11) | (1<<10) | (1<<8) | (1<<6) | (1<<4) );

		
		return 1;
}

/**************************************************************************************************************************************/		
