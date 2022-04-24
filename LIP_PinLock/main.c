/**
 * @file main.c
 * @author Luan Phelippe
 * @brief Realiza o acionamento e desacionamento de uma saída de acordo com o fornecimento correto de um PIN atrav�s das teclas do display LCD.
 * 
 */

#define F_CPU 16000000UL

#include <avr/io.h>
#include <util/delay.h>

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "adc.h"
#include "lcd.h"

#define	OFF 0
#define RIG 1
#define UPP 2
#define DOW 3
#define LEF 4
#define SEL 5

#define PASSWORD "1973" //PIN correto


/**
 * @brief Verifica qual botão foi pressionado
 * 
 * @return unsigned char 
 */
unsigned char botaoPressionado(void) {
	
	unsigned int result = 0;
	char bot;
	
	result = adc_read(0);
	
	if(result < 50)
		bot = RIG;
	else if(result < 150)
		bot = UPP;
	else if(result < 300)
		bot = DOW;
	else if(result < 600)
		bot = LEF;
	else if(result < 900)
		bot = SEL;
	else
		bot = OFF;
	
	while(adc_read(0) < 900){;} //Aguarda botao ser solto
		
	return bot;
}


int main(void)
{
	unsigned char bot = OFF;
	unsigned char pinDigitado[5] = {"____"};
	unsigned char estado = 0;
	unsigned char digito = '0';
	unsigned char casa = 0xC5;
	unsigned char i = 0;
	
	// Inicializacoes;
	adc_init();
	DisplayLCD_Init();
	DisplayLCD_Cmd(LCD_Clear);
	
	ddrd.bit0 = 1; //Define pino PB5 como saída
    
    for(;;)
    {		
		switch(estado) {
			
			case 0:
				
				portd.bit0 = 0; //Ativa bloqueio
				DisplayLCD_Out(1, 4, (char*)"BLOQUEADO       ");
				DisplayLCD_Out(2, 1, (char*)"PIN:            ");
				DisplayLCD_Cmd(LCD_Cursor_Blink);
				estado = 1;
				
			break;
			
			
			case 1: //Inserir PIN
				
				bot = botaoPressionado();
				
				if(bot == UPP && digito < '9')
					digito++;
				else if(bot == UPP && digito == '9')
					digito = '0';
				else if(bot == DOW && digito > '0')
					digito--;
				else if(bot == DOW && digito == '0')
					digito = '9';
				else if(bot == RIG && casa < 0xC8) {
					pinDigitado[i] = digito;
					digito = '0';
					i++;
					casa++;
				}
				else if(bot == LEF && casa > 0xC5) {
					pinDigitado[i] = digito;
					i--;
					casa--;
					digito = pinDigitado[i];
				}
				else if(bot == LEF && casa > 0xC5) {
					pinDigitado[i] = digito;
					i--;
					casa--;
					digito = pinDigitado[i];
				}
				else if(bot == SEL && casa == 0xC8) {
					estado = 2;
				}
				
				pinDigitado[i] = digito;
				DisplayLCD_Out(2, 6, (char*) pinDigitado);
				DisplayLCD_Cmd(casa);
				_delay_ms(200);
				
			break;
				
				
			case 2: //Verificar PIN
			
				DisplayLCD_Cmd(LCD_Cursor_Off);
			
				if(strcmp((char*) pinDigitado, (char*) PASSWORD) == 0) {
					portd.bit0 = 1; //Desativa bloqueio
					DisplayLCD_Out(1, 1, (char*) "    LIBERADO    ");
					DisplayLCD_Out(2, 1, (char*) "SEL P/ BLOQUEAR ");
					estado = 3;
				}
				else {
					DisplayLCD_Out(1, 1, (char*) "      ERRO      ");
					estado = 0;
					digito = '0';
					casa = 0xC5;
					
					for(i=0;i<4;i++)
						pinDigitado[i] = '_';
					
					i = 0;
					_delay_ms(5000);
				}
			
			break;
			
			case 3: //Liberado
				
				if(botaoPressionado() == SEL) {
					estado = 0;
					digito = '0';
					casa = 0xC5;
					
					for(i=0;i<4;i++)
					pinDigitado[i] = '_';
					
					i = 0;
				}
			
			break;
		}
    }
}

