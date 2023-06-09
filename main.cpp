#include "stm32f4xx.h"
#include <stdio.h>

int display, cont, borrar, dato, deco, numeros[16] = {63, 6, 91, 79, 102, 109, 125, 7, 127, 103, 119, 124, 57, 94, 121, 113};

void teclado(void){

	if(cont==0){
		switch(dato){
			case 0x400:GPIOD -> ODR = numeros[13]; dato=0;
				break;
			case 0x800:GPIOD -> ODR = numeros[12]; dato=0;
				break;
			case 0x1000:GPIOD -> ODR = numeros[11]; dato=0;
				break;
			case 0x8000:GPIOD -> ODR = numeros[10]; dato=0;
				break;
		}
	}else if(cont==1){
		switch(dato){
			case 0x400:GPIOD -> ODR = numeros[14]; dato=0;
				break;
			case 0x800:GPIOD -> ODR = numeros[9]; dato=0;
				break;
			case 0x1000:GPIOD -> ODR = numeros[6]; dato=0;
				break;
			case 0x8000:GPIOD -> ODR = numeros[3]; dato=0;
				break;
		}
	}else if(cont==2){
		switch(dato){
			case 0x400:GPIOD -> ODR = numeros[0]; dato=0;
				break;
			case 0x800:GPIOD -> ODR = numeros[8]; dato=0;
				break;
			case 0x1000:GPIOD -> ODR = numeros[5]; dato=0;
				break;
			case 0x8000:GPIOD -> ODR = numeros[2]; dato=0;
				break;
		}
	}else if(cont==3){
		switch(dato){
			case 0x400:GPIOD -> ODR = numeros[15]; dato=0;
				break;
			case 0x800:GPIOD -> ODR = numeros[7]; dato=0;
				break;
			case 0x1000:GPIOD -> ODR = numeros[4]; dato=0;
				break;
			case 0x8000:GPIOD -> ODR = numeros[1]; dato=0;
				break;
		}
	}
}

int main(void) {
	
	RCC -> AHB1ENR |= 0X9;
	RCC -> APB2ENR |= (1UL << 14);
	
	//PINES DEL TECLADO
	GPIOA -> MODER |= 0x55000;//DEL 6 AL 9 COMO SALIDA Y DEL 10 AL 12 Y 15 COMO ENTRADAS
	GPIOA -> OTYPER = 0;
	GPIOA -> OSPEEDR |= 0x5555;
	GPIOA -> PUPDR = 0x5555;
	GPIOA -> ODR &=0;//LOS INICIALIZO EN CERO POR SI LAS MOSCAS
	
	//DISPLAY
	GPIOD -> MODER = 0x5555;// DEL 0 AL 7
	GPIOD -> OTYPER = 0;//PUSH PULL
	GPIOD -> OSPEEDR = 0x5555;//VELOCIDAD MEDIA
	GPIOD -> PUPDR = 0x5555;//PULL UP
	
	//SYSTICK
	SystemCoreClockUpdate();
	SysTick_Config(SystemCoreClock);//1s
	
	//EXTI DEL 10 AL 12 Y 15 
	EXTI -> IMR |= 0x9C00;//DESENMASCARA LAS EXTI
	EXTI -> RTSR |= 0x9C00;//ENTRA A LA INTERRUPCION CON BORDE DE SUBIDA
	SYSCFG -> EXTICR[2] = 0X0;//PARA 10 Y 11
	SYSCFG -> EXTICR[3] = 0X0;//PARA 12 Y 15 DEL PUERTO A
	NVIC_EnableIRQ(EXTI15_10_IRQn);//HABILITACION DE LA INTERRUCION
		
	while(true){
	
		if(display==1){
			switch(cont){//AQUI HAGO UN BARRIDO EN CADA CASI EN 1ms REPETITIVAMENTE HASTA UNA INTERRUPCION
			case 0:GPIOA -> ODR &= ~(1UL << 9);GPIOA -> ODR |= (1UL << 6);
				break;
			case 1:GPIOA -> ODR &= ~(1UL << 6);GPIOA -> ODR |= (1UL << 7);
				break;
			case 2:GPIOA -> ODR &= ~(1UL << 7);GPIOA -> ODR |= (1UL << 8);
				break;
			case 3:GPIOA -> ODR &= ~(1UL << 8);GPIOA -> ODR |= (1UL << 9);
				break;
	  	}
		display=0;
		}
		
		if(deco==1){//SI ENTRO AL EXTI
			teclado();//BUSCO CUAL ES LA TECLA
			deco=0;
		}	
	}
}

extern "C"
{
	void SysTick_Handler(void){
	
		display = 1;
		cont++;
		if(cont>3){cont = 0;}
		borrar++;		
		if(borrar>2){GPIOD -> ODR = 0; borrar=0;}
	}
	
	void EXTI15_10_IRQHandler(void){
	
		EXTI -> PR |= 0x9C00;//limpia la bandera de interropcion 
		dato = (GPIOA -> IDR&0x9C00);
		deco=1;
		
	}
}
