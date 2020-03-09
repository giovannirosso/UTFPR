// gpio.c
// Desenvolvido para a placa EK-TM4C1294XL
// Controle do motor de passo
// Giovanni de Rosso Unruh
// Anderson Kmetiuk

#include <stdint.h>

#include "tm4c1294ncpdt.h"
																													//.Q.P.N.M.L.K.J.H.G.F.E.D.C.B.A.
																													//.0.1.0.1.1.1.1.0.0.0.1.0.0.0.0.
#define GPIO_PORTAS (0x2F10)  //INICIALIZA PORTA	

void SysTick_Wait1ms(uint32_t delay);
// -------------------------------------------------------------------------------
// Função GPIO_Init
// Inicializa os portas
// Parâmetro de entrada: Não tem
// Parâmetro de saída: Não tem
void GPIO_Init(void)
{
	//1a. Ativar o clock para a porta setando o bit correspondente no registrador RCGCGPIO
	SYSCTL_RCGCGPIO_R |= GPIO_PORTAS;
	//1b.   após isso verificar no PRGPIO se a porta está pronta para uso.
  while((SYSCTL_PRGPIO_R & (GPIO_PORTAS) ) != (GPIO_PORTAS) ){};
	
	// 2. Limpar o AMSEL para desabilitar a analógica
	GPIO_PORTE_AHB_AMSEL_R = 0x00;	
	GPIO_PORTJ_AHB_AMSEL_R = 0x00;
	GPIO_PORTK_AMSEL_R = 0x00;
	GPIO_PORTL_AMSEL_R = 0x00;
	GPIO_PORTM_AMSEL_R = 0x00;
	GPIO_PORTP_AMSEL_R = 0x00;
		
	// 3. Limpar PCTL para selecionar o GPIO
	GPIO_PORTE_AHB_PCTL_R = 0x00;
	GPIO_PORTJ_AHB_PCTL_R = 0x00;
	GPIO_PORTK_PCTL_R = 0x00;
	GPIO_PORTL_PCTL_R = 0x00;
	GPIO_PORTM_PCTL_R = 0x00;
	GPIO_PORTP_PCTL_R = 0x00;
		
	// 4. DIR para 0 se for entrada, 1 se for saída
	GPIO_PORTE_AHB_DIR_R = 0x07; //ouput 0,1,2
	GPIO_PORTJ_AHB_DIR_R = 0x00; //input em 1,0
	GPIO_PORTK_DIR_R = 0xFF;		//output 7,6,5,4,3,2,1,0
	GPIO_PORTL_DIR_R = 0x00;		//input 3,2,1,0
	GPIO_PORTM_DIR_R = 0xF7;		//output 7,6,5,4,2,1,0
	GPIO_PORTP_DIR_R = 0x00;		//input 1,0
	
	// 5. Limpar os bits AFSEL para 0 para selecionar GPIO sem função alternativa	
	GPIO_PORTE_AHB_AFSEL_R = 0x00; 
	GPIO_PORTJ_AHB_AFSEL_R = 0x00;
	GPIO_PORTK_AFSEL_R = 0x00;
	GPIO_PORTL_AFSEL_R = 0x00;
	GPIO_PORTM_AFSEL_R = 0x00;
	GPIO_PORTP_AFSEL_R = 0x00;
	
	// 6. Setar os bits de DEN para habilitar I/O digital	
	GPIO_PORTE_AHB_DEN_R = 0x07;	//ouput 0,1,2
	GPIO_PORTJ_AHB_DEN_R = 0x03;	//input em 1,0
	GPIO_PORTK_DEN_R = 0xFF;	//pk 7,6,5,4,3,2,1,0
	GPIO_PORTL_DEN_R = 0x0F;	//input 3,2,1,0
	GPIO_PORTM_DEN_R = 0xF7;	//pm 7,6,5,4,2,1,0
	GPIO_PORTP_DEN_R = 0x03;	//input 1,0
	
	// 7. Habilitar resistor de pull-up interno, setar PUR para 1
	GPIO_PORTJ_AHB_PUR_R = 0x03;   //PINO 1,0	
	GPIO_PORTL_PUR_R = 0x0F;			//PINO 3,2,1,0
	GPIO_PORTP_PUR_R = 0x03; // PINO 1,0

}	

// -------------------------------------------------------------------------------
// Função PortJ_Input
// Lê os valores de entrada do port J
uint32_t PortJ_Input(void)
{
	return GPIO_PORTJ_AHB_DATA_R;
}
// -------------------------------------------------------------------------------
// Função PortL_Input
// Lê os valores de entrada do port L
uint32_t PortL_Input(void)
{
	return GPIO_PORTL_DATA_R;
}
// -------------------------------------------------------------------------------
// Função PortP_Input
// Lê os valores de entrada do port P
uint32_t PortP_Input(void)
{
	return GPIO_PORTP_DATA_R;
}
// -------------------------------------------------------------------------------
// Função PortE_Output
void PortE_Output(uint32_t valor)
{
		uint32_t temp;
    //para uma escrita amigável nos bits 0 e 1
    temp = GPIO_PORTE_AHB_DATA_R & 0xF8;	
    //agora vamos fazer o OR com o valor recebido na função
    temp = temp | valor;
    GPIO_PORTE_AHB_DATA_R = valor; 
}
// -------------------------------------------------------------------------------
// Função PortK_Output
void PortK_Output(uint32_t valor)
{
		uint32_t temp;
    //para uma escrita amigável nos bits 0 e 1
    temp = GPIO_PORTK_DATA_R & 0x00;	
    //agora vamos fazer o OR com o valor recebido na função
    temp = temp | valor;
    GPIO_PORTK_DATA_R = valor; 
}
// -------------------------------------------------------------------------------
// Função PortM_Output
void PortM_Output(uint32_t valor)
{
    uint32_t temp;
    //vamos zerar somente os bits menos significativos
    //para uma escrita amigável nos bits 0 e 1
    temp = GPIO_PORTM_DATA_R & 0x08;
    //agora vamos fazer o OR com o valor recebido na função
    temp = temp | valor;
    GPIO_PORTM_DATA_R = temp; 
}

void Timer_INIT(void){
//===========================PASSO DOS TIMERS====================================================
	
		//1)Habilita o Timer 0
		SYSCTL_RCGCTIMER_R |= 0x1;
		while((SYSCTL_PRTIMER_R & 0x1) != 0x1) {} //garante que o timer já está pronto
		
		//2) Limpar o bit TnEN
		TIMER0_CTL_R &= 0x0;
		//3)timer no modo 16 ou 32 escrevendo 0x4 ou 0x0
		TIMER0_CFG_R |= 0x0;
		//4) configura com 0x2 para o modo periódico
		TIMER0_TAMR_R |= 0x2;
		//5) Carregar o valor da contagem desejado
		TIMER0_TAILR_R = 80000; //80000; //   1ms /12ns =80000 -> multiplicar esse valor por alguma porcentagem pra saber o tempo que ele vai ficar em alto ou baixo.

		//7)escrever 1 no bit TnTOCINT
		TIMER0_ICR_R  |= 0x1;
		//8)escrever 1 no bit TnTOIM
		TIMER0_IMR_R |= 0x1;
		//b)Setara prioridadeda interrupção do timerrespectivo no respectivo registrador NVIC PriorityRegister.
		//c)Habilitara interrupção do timerrespectivo no respectivo registrador NVIC InterruptEnableRegister
		NVIC_PRI4_R |= 3 << 29;
		NVIC_EN0_R |= 1 << 19;
			
		void EnableInterrupts(void);
		//9.Habilitar o bit TnENno registrador GPTMCTL (Control) para começar o timer e iniciar a contagem de modo decrescente do valor da contagem.
		//habilita o timer
		//TIMER0_CTL_R |= 0x1;
}

// -------------------------------------------------------------------------------
void Config_Interrupt_J(void)
{
		int temp;
	
		temp = 0x00 | GPIO_PORTJ_AHB_IM_R;
		GPIO_PORTJ_AHB_IM_R = temp;
		
		temp = 0x00 | GPIO_PORTJ_AHB_IS_R;
		GPIO_PORTJ_AHB_IS_R = temp;
		
		temp = 0x00;
		temp = temp | GPIO_PORTJ_AHB_IBE_R;
		GPIO_PORTJ_AHB_IBE_R = temp;
		
		temp = 0x00;
		temp = temp | GPIO_PORTJ_AHB_IEV_R;
		GPIO_PORTJ_AHB_IEV_R = temp;
	
		temp = 0x01;
		temp = temp | GPIO_PORTJ_AHB_ICR_R;
		GPIO_PORTJ_AHB_ICR_R = temp;	
	
		temp = 0x01;
		temp = temp | GPIO_PORTJ_AHB_IM_R;
		GPIO_PORTJ_AHB_IM_R = temp;
	
		temp = 0xA0000000;
		temp = temp | NVIC_PRI12_R;
		NVIC_PRI12_R = temp;
		
		temp = 0x00080000;
		temp = temp | NVIC_EN1_R;
		NVIC_EN1_R = temp;			
}

void SPI_Init1(void)
{
	
	//1a. Ativar o clock para a porta setando o bit correspondente no registrador RCGCGPIO
	//1b.   após isso verificar no PRGPIO se a porta está pronta para uso.
    SYSCTL_RCGCGPIO_R |= SYSCTL_RCGCGPIO_R3;  
    while((SYSCTL_PRGPIO_R & (SYSCTL_RCGCGPIO_R3) ) != (SYSCTL_RCGCGPIO_R3) ){};
        
   	// 2. Desabilitar a funcionalidade analógica dos pinos do GPIO no registrador GPIOAMSEL.
		GPIO_PORTD_AHB_AMSEL_R = 0x00;

	// 3. Preencher a função alternativa dos pinos do GPIO, para o DAT1, DAT0, Fss e clk, no registrador
	// GPIOPCTL (verificar a tabela 10-2 no datasheet páginas 743-746)
	//COLOCAR 1111 NO primeiro,segundo,terceiro e quarto BLOCO (PD 0,1,2,3) CONFORME TABELA E PAG 788
    GPIO_PORTD_AHB_PCTL_R = 0xFFFF;
	
	// 4. Habilitar os bits de função alternativa no registrador GPIOAFSEL para o pino do GPIO. PINOS 0-3 - 1111 = F 
    GPIO_PORTD_AHB_AFSEL_R = 0xF;
	
	// 5. Habilitar a função digital no pino do GPIO no registrador GPIODEN. PINOS 0-3 - 1111 = F
		GPIO_PORTD_AHB_DEN_R = 0xF;
	
	// 6. Habilitar o clock no módulo SSI no registrador RCGCSSI(cada bit representa uma SSI) //SSI2
	//e esperar até que a respectiva SSI esteja pronta para ser acessada no registrador PRSSI(cada bit representa uma SSI).
		SYSCTL_RCGCSSI_R |= SYSCTL_RCGCSSI_R2;  
    while((SYSCTL_RCGCSSI_R & (SYSCTL_RCGCSSI_R2) ) != (SYSCTL_RCGCSSI_R2) ){};
    
	// 7.Desabilitar o SSI no registrador SSICR1 escrevendo 0no bit SSE.
		SSI2_CR1_R &= ~(1<<1);
	
	// 8.Configurar o SSI como masterno registrador SSICR1 escrevendo 0 no bit MS.
		SSI2_CR1_R = 0x00;
			
  // 9.Configurar o registrador de prescalerSSICPSRcomo divisor de clock, e o campo SCR no registrador SSICR0 segundo a equação de baudrate.
		SSI2_CC_R = 0x00;
		SSI2_CPSR_R = 100;
			
	// 10.Configurar o registrador SSICR0, os bits SPH, SPO para 0 (se o SPI estiver sendo executado no modo normal) e o campo FRFpara 0.
		SSI2_CR0_R = 0x0000;
			
	// 11.Configurar o tamanho do número de bits na FIFO no campo DSSdo registrador SSICR0.
		SSI2_CR0_R = (0xF<<0);
			
	// 12.Habilitar o SSI no registrador SSICR1 escrevendo 0no bit SSE.
		SSI2_CR1_R |= (1<<1);	
}

void writeSPI_dado(int data){
	SSI2_DR_R = data;
	while( (SSI2_SR_R & (1<<0))==0);
}

void SPI_Init2(void)
{
	
	//1a. Ativar o clock para a porta setando o bit correspondente no registrador RCGCGPIO
	//1b.   após isso verificar no PRGPIO se a porta está pronta para uso.
    SYSCTL_RCGCGPIO_R |= SYSCTL_RCGCGPIO_R0;  
    while((SYSCTL_PRGPIO_R & (SYSCTL_RCGCGPIO_R0) ) != (SYSCTL_RCGCGPIO_R0) ){};
        
   	// 2. Desabilitar a funcionalidade analógica dos pinos do GPIO no registrador GPIOAMSEL.
		GPIO_PORTA_AHB_AMSEL_R = 0x00;

	// 3. Preencher a função alternativa dos pinos do GPIO, para o DAT1, DAT0, Fss e clk, no registrador
	// GPIOPCTL (verificar a tabela 10-2 no datasheet páginas 743-746)
	//COLOCAR 1111 NO primeiro,segundo,terceiro e quarto BLOCO (PA 2,3,4,5) CONFORME TABELA E PAG 788
    GPIO_PORTA_AHB_PCTL_R = 0xFFFF00;
	
	// 4. Habilitar os bits de função alternativa no registrador GPIOAFSEL para o pino do GPIO. PINOS 5-2 - 111100 = 3C
    GPIO_PORTA_AHB_AFSEL_R = 0x3C;
	
	// 5. Habilitar a função digital no pino do GPIO no registrador GPIODEN. PINOS 5-2 - 111100 = 3C
		GPIO_PORTA_AHB_DEN_R = 0x3C;
	
	// 6. Habilitar o clock no módulo SSI no registrador RCGCSSI(cada bit representa uma SSI)  //SSI0
	//e esperar até que a respectiva SSI esteja pronta para ser acessada no registrador PRSSI(cada bit representa uma SSI).
		SYSCTL_RCGCSSI_R |= SYSCTL_RCGCSSI_R0;  
    while((SYSCTL_RCGCSSI_R & (SYSCTL_RCGCSSI_R0) ) != (SYSCTL_RCGCSSI_R0) ){};
    
	// 7.Desabilitar o SSI no registrador SSICR1 escrevendo 0no bit SSE.
		SSI0_CR1_R &= ~(1<<1);
	
	// 8.Configurar o SSI como masterno registrador SSICR1 escrevendo 0 no bit MS.
		SSI0_CR1_R = 0x00;
			
  // 9.Configurar o registrador de prescalerSSICPSRcomo divisor de clock, e o campo SCR no registrador SSICR0 segundo a equação de baudrate.
		SSI0_CC_R = 0x00;
		SSI0_CPSR_R = 100;
			
	// 10.Configurar o registrador SSICR0, os bits SPH, SPO para 0 (se o SPI estiver sendo executado no modo normal) e o campo FRFpara 0.
		SSI0_CR0_R = 0x0000;
			
	// 11.Configurar o tamanho do número de bits na FIFO no campo DSSdo registrador SSICR0.
		SSI0_CR0_R = (0xF<<0);
			
	// 12.Habilitar o SSI no registrador SSICR1 escrevendo 0no bit SSE.
		SSI0_CR1_R |= (1<<1);	
}

void writeSPI_linha(int data){
	SSI0_DR_R = data;
	while( (SSI0_SR_R & (1<<0))==0);
}
