// main.c
// Desenvolvido para a placa EK-TM4C1294XL
// Controle do motor DC
// Giovanni de Rosso Unruh
// Anderson Kmetiuk

#include <stdint.h>
#include <string.h>
#include "tm4c1294ncpdt.h"
#include <stdlib.h>
#include <time.h>


typedef enum estJogo
{
	MenuPrincipal,
	SelecionaModo,
	Jogo,
	GameOver
}estadosJogo;

//DECLARA INICIALIZACOES
void PLL_Init(void);
void SysTick_Init(void);
void SysTick_Wait1ms(uint32_t delay);
void GPIO_Init(void);
void SPI_Init1(void);
void SPI_Init2(void);
void ConversorAD_Init(void);
void Timer_INIT(void);

//DECLARA PORTAS
uint32_t PortJ_Input(void); //BOTÃO INTERRUPÇÃO
uint32_t PortL_Input(void); //TECLADO IN
uint32_t PortP_Input(void); //BOTÃO INTERRUPÇÃO
void PortK_Output(uint32_t lcd);
void PortM_Output(uint32_t teclado);

//DECLARA FUNÇOES
void Timer0A_Handler(void);
void Config_Interrupt_J(void);
void GPIOPortJ_Handler(void);
void writeSPI_dado(int data);
void writeSPI_linha(int data);
void Acha_Estado(estadosJogo estados);
void DisplayLCD_Init(void);
void Limpa_LCD(void);
void Escreve_LCD(uint32_t valor);
void Escreve_Frase(char *frase1, char *frase2);
void Escreve_Linha2(void);
uint32_t Verifica_Teclado(void);
uint32_t Identifica_Tecla(uint32_t tecla);
void pong(void);
void pong2(void);
void pong3(void);
uint32_t RandomBall(void);

//VERIAVEIS GLOBAIS
int Modo=0;
int reset=0,Ponto_Jogador=0,Ponto_AI=0,Ponto_Jogador2=0;
const int UTFPR[16]			={0x9FC0,0x943E,0x95ED,0x953E,0x9525,0xF525,0x0000,0xFFFF,0x0000,0x445C,0xAAD4,0x2A5C,0x4A44,0x8A44,0xE459,0x0000};
const int PONG[16]			={0xFFFF,0x87F1,0xBBED,0xBBED,0xBBED,0xBBED,0x87F0,0xBFFD,0xA2D9,0xAAD5,0xAA55,0xAA15,0xAA95,0xAAD5,0xA2DB,0xFFFF};
const int GAMEMODE[16]	={0x6107,0x91E4,0x8157,0xB154,0x9557,0x6A00,0x0E00,0x0A00,0x0A00,0x8000,0xF19C,0xAA52,0xAA52,0xAA52,0xAA52,0xA99C};
const int GAMEOVER[16]	={0x6107,0x91E4,0x8157,0xB154,0x9557,0x6A00,0x0E00,0x0A00,0x6A76,0xF045,0x9545,0x9566,0x9566,0x9545,0xF745,0x6275};


//-------------------------INICIALIZAÇÃO--------------------------------
int main(void)
{
	PLL_Init();
	SysTick_Init();
	GPIO_Init();
	SysTick_Wait1ms(10);
	DisplayLCD_Init();
	SPI_Init1();
	SPI_Init2();
	Timer_INIT();
	Limpa_LCD();
	Acha_Estado(MenuPrincipal);		// ESTADO INICIAL MenuPrincipal
	
}
//---------------FUNÇÃO DOS ESTADOS-----------------------------------
void Acha_Estado(estadosJogo estados)
{	
	Config_Interrupt_J();
	int linha = 0x8000;
	int i, txt=0, x, aux=0xFF;

	while(1)
	{
	if(reset==1){reset=0; aux = 0xFF; txt=0; Modo=0; linha = 0x8000; estados = MenuPrincipal; Ponto_Jogador=0;Ponto_AI=0;Ponto_Jogador2=0;} // if reset sw1 - reinicia variaveis 
		switch (estados){
		
		//MENU INICIAL
		case MenuPrincipal:	
			
		if(txt==0){
		
		Limpa_LCD();
		Escreve_Frase("Segure '*' p/","Iniciar -> ");
		txt++;
		}
		
		aux = Verifica_Teclado();
		if(aux==0xE7){
			Identifica_Tecla(aux);
			txt=0;
			aux=0xFF;
			estados=SelecionaModo;
		}
		
		for(x=0;x<25;x++){
			for(i=0;i<16;i++){
				writeSPI_dado(~(PONG[i]));	//Dado
				writeSPI_linha((linha>>i));	//LINHA
				SysTick_Wait1ms(1);
			}
		}
		for(x=25;x>0;x--){
			for(i=0;i<16;i++){
				writeSPI_dado((PONG[i]));	//Dado
				writeSPI_linha((linha>>i));	//LINHA
				SysTick_Wait1ms(1);
			}
		}
		
			break;
		
		//MENU DE MODOS	
		case SelecionaModo:
		
		if(txt==0){
		Limpa_LCD();
		Escreve_Frase("'1'Treino '2'vs ", "AI '3' 2 players");
		txt++;
		}
		
		aux = Verifica_Teclado();
		if(aux==0xEE||aux==0xDE||aux==0xBE){
			Modo=Identifica_Tecla(aux);
			SysTick_Wait1ms(200);
			txt=0;
			aux=0xFF;
			estados=Jogo;
		}
		
		for(i=0;i<16;i++){
				writeSPI_dado(~(GAMEMODE[i]));	//Dado
				writeSPI_linha((linha>>i));	//LINHA
				SysTick_Wait1ms(1);
		}
			
			break;
		
		//JOGO
		case Jogo:
			
		//***MODO TREINO***	
		if(Modo==1)
		{
			if(txt==0){
				Limpa_LCD();
				Escreve_Frase("Modo Treino", "");
				txt++;
			}
			pong();
			
		}
		
		//***VS AI***
		if(Modo==2)
		{
			if(txt==0){
				Limpa_LCD();
				Escreve_Frase("Vs AI", "");
				txt++;
			}
			pong2();
		}
		//**2 JOGADORES***
		if(Modo==3)
		{
			if(txt==0){
				Limpa_LCD();
				Escreve_Frase("2 Jogadores", "");
				txt++;
			}
			pong3();
		}
			break;	
		
		//FIM DE JOGO
		case GameOver:
		
		if(Modo==1){
		if(txt==0){
		Limpa_LCD();		
		Escreve_Frase("Fim de Treino","Aperte '*'");
		txt++;
		}
		}
		if(Modo==2){
		if(txt==0){
		Limpa_LCD();	
			//-----PONTUAÇÃO FINAL----
			if(Ponto_Jogador==7){Escreve_Frase("** VENCEU!!! **","APERTE '*'");} //Jogador
			if(Ponto_AI==7){Escreve_Frase("** PERDEU :( **","APERTE '*'");} //AI
		
		txt++;
		}
		}
		if(Modo==3){
		if(txt==0){
		Limpa_LCD();	
			//-----PONTUAÇÃO FINAL----
			if(Ponto_Jogador==7){Escreve_Frase("VENCEU PLAYER1!","APERTE '*'");} //Jogador
			if(Ponto_Jogador2==7){Escreve_Frase("VENCEU PLAYER2!","APERTE '*'");} //Jogador2
		
		txt++;
		}
		}
		
		aux = Verifica_Teclado();
		if(aux==0xE7){
			Identifica_Tecla(aux);
			SysTick_Wait1ms(200);
			txt=0;
			aux=0xFF;
			Modo=0;
			Ponto_Jogador=0;
			Ponto_AI=0;
			Ponto_Jogador2=0;
			estados=MenuPrincipal;
		}
		
		for(i=0;i<16;i++){
				writeSPI_dado(~(GAMEOVER[i]));	//Dado
				writeSPI_linha((linha>>i));	//LINHA
				SysTick_Wait1ms(1);
			}
		
			break;
		}
	}
}
//----------------FUNÇÃO DO JOGO----------------------------

//----***MODO TREINO***------
void pong()
{
	int linha=0x1;
	int player1=0x7C0,dir_jogador=0;
	int aux=0xFF;
	int BolaCol=RandomBall(),BolaLin=0x8000; //limite Colunas: ESQ 0x8000, DIR 0x1 ;; Linhas: 0x1,0x8000
	int x=RandomBall()%2,y=0; //x=0 esq,x=1 dir,y=0 desce,y=1 sobe
	int ponto=0,i;
	
	while(ponto==0){
		
		for(i=0;i<13;i++){
		writeSPI_dado(~player1);
		writeSPI_linha(linha);
		SysTick_Wait1ms(1);
		writeSPI_dado(~BolaCol);
		writeSPI_linha(BolaLin);
		SysTick_Wait1ms(3);
		aux = Verifica_Teclado();
		}

		//-------------------------MOVIMENTO PLAYER1------------------------		
		if(aux==0xBE) //tecla 3 ESQUERDA
		{
			dir_jogador=1; //movendo pra esquerda
			
			if(player1!=0xF800) //LIMITE DA ESQUERDA
				player1=player1<<1;

			aux=0xFF;
		}
		if(aux==0xEE) //tecla 1 DIREITA
		{
			dir_jogador=2;//movendo pra direita
			
				if(player1!=0x1F) //LIMITE DA DIREITA
					player1=player1>>1;
				
				aux=0xFF;
		}
		
		//----------------MOVIMENTO BOLA----------------			
		//DESCE
		if(BolaLin!=0x0 && y==0) 
		{
			BolaLin=BolaLin>>1;
			
			//COLIDIU COM O JOGADOR
			if(BolaLin==0x1 && ( (BolaCol & player1) || ((BolaCol>>1) & player1) || ((BolaCol<<1) & player1) )) //verifica se colidiu com o player
			{
				y=1;
				BolaLin=0x2;									
				
				//Analisa o sentido do jogador para mudar direção da bolinha
				//só muda quando os dois estão vindo de direções opostas
				//mantem a enquanto está parado ou vindo na mesma direção
				if(dir_jogador==2 && x==0){x=1;dir_jogador=0;}
				if(dir_jogador==1 && x==1){x=0;dir_jogador=0;} 				
			
			}
			
			//NÃO COLIDIU COM O JOGADOR
			if(BolaLin==0x1 && ~( (BolaCol & player1) || ((BolaCol>>1) & player1) || ((BolaCol<<1) & player1) )) //se não colidiu conta ponto
			{
				dir_jogador=0;
				if(x==0)
				{
					writeSPI_dado(~(BolaCol<<1));
					writeSPI_linha(BolaLin);
					SysTick_Wait1ms(200);
					ponto=1;
					Ponto_AI++;
				}
				if(x==1)
				{
					writeSPI_dado(~(BolaCol>>1));
					writeSPI_linha(BolaLin);
					SysTick_Wait1ms(200);
					ponto=1;
					Ponto_AI++;
				}
				
			}
	
		}
	
		//SOBE
		if(BolaLin!=0x8000 && y==1) 
		{
			BolaLin=BolaLin<<1;
			
			if(BolaLin==0x8000)
				y=0;
		}
		
		//ESQUERDA
		if(BolaCol!=(0x8000<<1) && x==0) 
		{
			BolaCol=BolaCol<<1;
			
			if(BolaCol==(0x8000<<1)){
				x=1;
				BolaCol=0x8000;
			}
		}
		
		//DIREITA
		if(BolaCol!=0x1 && x==1) 
		{
			BolaCol=BolaCol>>1;
			
			if(BolaCol==0x1)
				x=0;
		}
			
	}
	
	//MUDA DE ESTADO QUANDO CHEGAR EM 7 PONTOS
	if(Ponto_AI==7) {Acha_Estado(GameOver);}
	
	
}

//----------***VS AI***----------------
void pong2()
{
	int linha=0x1;
	int player1=0x7C0,dir_jogador=0;
	int aux=0xFF;
	int BolaCol=RandomBall(),BolaLin=0x100; //limite Colunas: ESQ 0x8000, DIR 0x1 ;; Linhas: 0x1,0x8000
	int x=RandomBall()%2,y=RandomBall()%2; //x=0 esq,x=1 dir,y=0 desce,y=1 sobe
	int ponto=0;
	int AI=0x7C0, dir_AI=0, linha_AI=0x8000,ativa_AI=0,mov_AI,sorteio_AI=0,bola_AI=0;
	int i;
	
	while(ponto==0){
		
		for(i=0;i<13;i++){
		writeSPI_dado(~player1);
		writeSPI_linha(linha);
		SysTick_Wait1ms(1);
		writeSPI_dado(~AI);
		writeSPI_linha(linha_AI);
		SysTick_Wait1ms(1);
		writeSPI_dado(~BolaCol);
		writeSPI_linha(BolaLin);
		SysTick_Wait1ms(5);
		aux = Verifica_Teclado();
		}

		//-------------------------MOVIMENTO PLAYER1------------------------		
		if(aux==0xBE) //tecla 3 ESQUERDA
		{
			dir_jogador=1; //movendo pra esquerda
			
			if(player1!=0xF800) //LIMITE DA ESQUERDA
				player1=player1<<1;

			aux=0xFF;
		}
		if(aux==0xEE) //tecla 1 DIREITA
		{
			dir_jogador=2;//movendo pra direita
			
				if(player1!=0x1F) //LIMITE DA DIREITA
					player1=player1>>1;
				
				aux=0xFF;
		}
		
		//-------------------------MOVIMENTO AI------------------------	
		
		if(BolaLin==0x100 && y==1) //quando a bola chega na linha 0x100 (metade da matriz de led) ativa a IA
		{
			ativa_AI=1;
			if(BolaCol & 0xFF00){bola_AI=1;} //bola na metade esquerda do display
			if(BolaCol & 0x00FF){bola_AI=2;} //bola na metade direita do display
		}
		
		//quando AI está ativa
		// da posição inicial 0xFC0 temos >>6 (bits) e <<5 (bits)
		//11 posições no total de deslocamento
		if(ativa_AI==1 && y==1) 
		{
			if(sorteio_AI==0){
				mov_AI=RandomBall()%12; //sorteia uma quantidade de mov aleatória
				sorteio_AI=1;
			}
			
			if(x==0 && mov_AI!=0 && bola_AI==1) //bola indo pra esq da metade esq
			{				
					dir_AI=1; //movendo pra esquerda
				
					
					if(AI!=0xF800) //LIMITE DA ESQUERDA
						AI=AI<<1;
					
				
			}
			if(x==0 && mov_AI!=0 && bola_AI==2) //bola indo pra esq da metade direita
			{				
					dir_AI=1; //movendo pra esquerda
					
					
					if(AI!=0xF800) //LIMITE DA ESQUERDA
						AI=AI<<1;					
				
			}
			if(x==1 && mov_AI!=0 && bola_AI==1) //bola indo pra direita da metade esq
			{				
					dir_AI=2; //movendo pra direita
					
					
					if(AI!=0x1F) //LIMITE DA DIREITA
						AI=AI>>1;													
			}
			if(x==1 && mov_AI!=0 && bola_AI==2) //bola indo pra direita da metade direita
			{				
					dir_AI=2; //movendo pra direita
					
					
					if(AI!=0x1F) //LIMITE DA DIREITA
						AI=AI>>1;													
			}
			
			
			if(mov_AI==0)
			{
				ativa_AI=0; //desativa AI
				bola_AI=0;
			}
			
			mov_AI--;
		}
		
		//----------------MOVIMENTO BOLA----------------			
		//DESCE
		if(BolaLin!=0x0 && y==0) 
		{
			BolaLin=BolaLin>>1;
			
			//COLIDIU COM O JOGADOR
			if(BolaLin==0x1 && ( (BolaCol & player1) || ((BolaCol>>1) & player1) || ((BolaCol<<1) & player1) )) //verifica se colidiu com o player
			{
				y=1;
				BolaLin=0x2;									
				
				//Analisa o sentido do jogador para mudar direção da bolinha
				//só muda quando os dois estão vindo de direções opostas
				//mantem a enquanto está parado ou vindo na mesma direção
				if(dir_jogador==2 && x==0){x=1;dir_jogador=0;}
				if(dir_jogador==1 && x==1){x=0;dir_jogador=0;} 				
			
			}
			
			//NÃO COLIDIU COM O JOGADOR
			if(BolaLin==0x1 && ~( (BolaCol & player1) || ((BolaCol>>1) & player1) || ((BolaCol<<1) & player1) )) //se não colidiu conta ponto
			{
				dir_jogador=0;
				if(x==0)
				{
					writeSPI_dado(~(BolaCol<<1));
					writeSPI_linha(BolaLin);
					SysTick_Wait1ms(200);
					ponto=1;
					Ponto_AI++;
				}
				if(x==1)
				{
					writeSPI_dado(~(BolaCol>>1));
					writeSPI_linha(BolaLin);
					SysTick_Wait1ms(200);
					ponto=1;
					Ponto_AI++;
				}
				
			}
	
		}
	
		//SOBE
		if(BolaLin!=0x8000 && y==1) 
		{
			BolaLin=BolaLin<<1;
			

			
			//COLIDIU COM AI
			if(BolaLin==0x8000 && ( (BolaCol & AI) || ((BolaCol>>1) & AI) || ((BolaCol<<1) & AI) )) //verifica se colidiu com o player
			{
				y=0;
				BolaLin=0x4000;									
				
				//Analisa o sentido do jogador para mudar direção da bolinha
				//só muda quando os dois estão vindo de direções opostas
				//mantem a enquanto está parado ou vindo na mesma direção
				if(dir_AI==2 && x==0){x=1;dir_AI=0;}
				if(dir_AI==1 && x==1){x=0;dir_AI=0;} 				
			
			}
			
			//NÃO COLIDIU COM AI
			if(BolaLin==0x8000 && ~( (BolaCol & AI) || ((BolaCol<<1) & AI) )) //se não colidiu conta ponto
			{
				dir_AI=0;
				if(x==0)
				{
					writeSPI_dado(~(BolaCol<<1));
					writeSPI_linha(BolaLin);
					SysTick_Wait1ms(200);
					ponto=1;
					Ponto_Jogador++;
				}
				if(x==1)
				{
					writeSPI_dado(~(BolaCol>>1));
					writeSPI_linha(BolaLin);
					SysTick_Wait1ms(200);
					ponto=1;
					Ponto_Jogador++;
				}
				
			}
		}
		
		//ESQUERDA
		if(BolaCol!=(0x8000<<1) && x==0) 
		{
			BolaCol=BolaCol<<1;
			
			if(BolaCol==(0x8000<<1)){
				x=1;
				BolaCol=0x8000;
			}
		}
		
		//DIREITA
		if(BolaCol!=0x1 && x==1) 
		{
			BolaCol=BolaCol>>1;
			
			if(BolaCol==0x1)
				x=0;
		}
			
	}
	
	//MUDA DE ESTADO QUANDO CHEGAR EM 7 PONTOS
	if(Ponto_AI==7 || Ponto_Jogador==7) { Acha_Estado(GameOver);}
	
	
}


//----***PLAYER2***------********************************
void pong3()
{
	int linha=0x1;
	int player1=0x7C0,dir_jogador=0;
	int aux=0xFF;
	int BolaCol=RandomBall(),BolaLin=0x100; //limite Colunas: ESQ 0x8000, DIR 0x1 ;; Linhas: 0x1,0x8000
	int x=RandomBall()%2,y=RandomBall()%2; //x=0 esq,x=1 dir,y=0 desce,y=1 sobe
	int ponto=0;
	int player2=0x7C0, dir_jogador2=0, linha_jogador2=0x8000;
	int i,a=5,b=0;
	
	while(ponto==0){
		
		if(b==10){ if(a!=1) {a--;} b=0;}
		
		for(i=0;i<13;i++){
		writeSPI_dado(~player1);
		writeSPI_linha(linha);
		SysTick_Wait1ms(1);
		writeSPI_dado(~player2);
		writeSPI_linha(linha_jogador2);
		SysTick_Wait1ms(1);
		writeSPI_dado(~BolaCol);
		writeSPI_linha(BolaLin);
		SysTick_Wait1ms(a);
		aux = Verifica_Teclado();
		}

		//-------------------------MOVIMENTO PLAYER1------------------------		
		if(aux==0xBE) //tecla 3 ESQUERDA
		{
			dir_jogador=1; //movendo pra esquerda
			
			if(player1!=0xF800) //LIMITE DA ESQUERDA
				player1=player1<<1;

			aux=0xFF;
		}
		if(aux==0xEE) //tecla 1 DIREITA
		{
			dir_jogador=2;//movendo pra direita
			
				if(player1!=0x1F) //LIMITE DA DIREITA
					player1=player1>>1;
				
				aux=0xFF;
		}
		//-------------------------MOVIMENTO PLAYER2------------------------		
		if(PortP_Input() == 0x2) //tecla 3 ESQUERDA
		{
			dir_jogador2=1; //movendo pra esquerda
			
			if(player2!=0xF800) //LIMITE DA ESQUERDA
				player2=player2<<1;

			
		}
		if(PortP_Input() == 0x1) //tecla 1 DIREITA
		{
			dir_jogador2=2;//movendo pra direita
			
				if(player2!=0x1F) //LIMITE DA DIREITA
					player2=player2>>1;
				
				
		}
		
		
		//----------------MOVIMENTO BOLA----------------			
		//DESCE
		if(BolaLin!=0x0 && y==0) 
		{
			BolaLin=BolaLin>>1;
			
			//COLIDIU COM O JOGADOR
			if(BolaLin==0x1 && ( (BolaCol & player1) || ((BolaCol>>1) & player1) || ((BolaCol<<1) & player1) )) //verifica se colidiu com o player
			{
				y=1;
				BolaLin=0x2;									
				
				//Analisa o sentido do jogador para mudar direção da bolinha
				//só muda quando os dois estão vindo de direções opostas
				//mantem a enquanto está parado ou vindo na mesma direção
				if(dir_jogador==2 && x==0){x=1;dir_jogador=0;}
				if(dir_jogador==1 && x==1){x=0;dir_jogador=0;} 				
			
			}
			
			//NÃO COLIDIU COM O JOGADOR
			if(BolaLin==0x1 && ~( (BolaCol & player1) || ((BolaCol>>1) & player1) || ((BolaCol<<1) & player1) )) //se não colidiu conta ponto
			{
				dir_jogador=0;
				if(x==0)
				{
					writeSPI_dado(~(BolaCol<<1));
					writeSPI_linha(BolaLin);
					SysTick_Wait1ms(200);
					ponto=1;
					Ponto_Jogador2++;
				}
				if(x==1)
				{
					writeSPI_dado(~(BolaCol>>1));
					writeSPI_linha(BolaLin);
					SysTick_Wait1ms(200);
					ponto=1;
					Ponto_Jogador2++;
				}
				
			}
	
		}
	
		//SOBE
		if(BolaLin!=0x8000 && y==1) 
		{
			BolaLin=BolaLin<<1;
			

			
			//COLIDIU COM player2
			if(BolaLin==0x8000 && ( (BolaCol & player2) || ((BolaCol>>1) & player2) || ((BolaCol<<1) & player2) )) //verifica se colidiu com o player
			{
				y=0;
				BolaLin=0x4000;									
				
				//Analisa o sentido do jogador para mudar direção da bolinha
				//só muda quando os dois estão vindo de direções opostas
				//mantem a enquanto está parado ou vindo na mesma direção
				if(dir_jogador2==2 && x==0){x=1;dir_jogador2=0;}
				if(dir_jogador2==1 && x==1){x=0;dir_jogador2=0;} 				
			
			}
			
			//NÃO COLIDIU COM AI
			if(BolaLin==0x8000 && ~( (BolaCol & player2) || ((BolaCol<<1) & player2) )) //se não colidiu conta ponto
			{
				dir_jogador2=0;
				if(x==0)
				{
					writeSPI_dado(~(BolaCol<<1));
					writeSPI_linha(BolaLin);
					SysTick_Wait1ms(200);
					ponto=1;
					Ponto_Jogador++;
				}
				if(x==1)
				{
					writeSPI_dado(~(BolaCol>>1));
					writeSPI_linha(BolaLin);
					SysTick_Wait1ms(200);
					ponto=1;
					Ponto_Jogador++;
				}
				
			}
		}
		
		//ESQUERDA
		if(BolaCol!=(0x8000<<1) && x==0) 
		{
			BolaCol=BolaCol<<1;
			
			if(BolaCol==(0x8000<<1)){
				x=1;
				BolaCol=0x8000;
			}
		}
		
		//DIREITA
		if(BolaCol!=0x1 && x==1) 
		{
			BolaCol=BolaCol>>1;
			
			if(BolaCol==0x1)
				x=0;
		}
		b++;
	}
	
	
	//MUDA DE ESTADO QUANDO CHEGAR EM 7 PONTOS
	if(Ponto_Jogador2==7 || Ponto_Jogador==7) { Acha_Estado(GameOver);}

	
}


//---------------------POSIÇÃO ALEATÓRIA DA BOLINHA----------------
uint32_t RandomBall(void)
{
	int aux,pos; 
	aux= rand()%16; //gera o número de bits que vai deslocar pra direita 0 a 16 bits
	pos=(0x8000)>>aux;//0x8000 é o limite da esq
	return pos;	
}


void GPIOPortJ_Handler(void)
{
		int temp;
		temp = 0x00000001;
		temp = temp | GPIO_PORTJ_AHB_ICR_R;
		GPIO_PORTJ_AHB_ICR_R = temp;
	
		reset=1;
		Limpa_LCD();
		Escreve_Frase("-----RESET-----","---------------");
		SysTick_Wait1ms(500);
}

		
