/*
 * File:   newmain.c
 * Author: neuva_pwxiyql
 *
 * Created on 05 de Dezembro de 2022, 14:22
 */

#include <xc.h>
#include <stdio.h>
#include <stdlib.h>
#include "lcd.h"

//Configurações necessárias para o programa
#pragma config FOSC = XT_XT     // Oscillator Selection bits (XT oscillator (XT))
#pragma config WDT = OFF        // Watchdog Timer Enable bit (WDT disabled (control is placed on the SWDTEN bit))
#pragma config PBADEN = OFF     // PORTB A/D Enable bit (PORTB<4:0> pins are configured as digital I/O on Reset)
#pragma config MCLRE = ON       // MCLR Pin Enable bit (MCLR pin enabled; RE3 input pin disabled)
#pragma config LVP = OFF        // Single-Supply ICSP Enable bit (Single-Supply ICSP disabled)

#define _XTAL_FREQ 4000000 //Define a frequencia do clock

// Variaveis de controles
int mediador;
int jogador1;
int jogador2;
// armazenam a contagem de tempo
int cj1 = 0;
int cj2 = 0;


void __interrupt()isr(){
    // Evento quando o botão do mediador é precionado
    if(INT0IF){
        if((jogador1 == 0) && (jogador2 == 0)){
            mediador = 1;
            //Aciona o buzz
            PORTAbits.RA0 = 1;
            inicializa_lcd();
            limpa_lcd();
            //define ponto de inicio da primeira frase
            caracter_inicio(1,1);
            printf("COMECEM!");
            __delay_ms(250);
            limpa_lcd();
             // desliga o buzz
            //PORTAbits.RA0 = 0;
            // habilita interrupção do int1 e 2
            INT1IE = 1;
            INT2IE = 1;
            // zera contadores
            cj1 = 0;
            cj2 = 0;
        }else if((jogador1 == 1) && (jogador2 ==1)){ // Caso os botões ja tenham sido precionados
            mediador = 0;
            jogador1 = 0;
            jogador2 = 0;
            // apaga os leds 
            PORTAbits.RA1=0;
            PORTAbits.RA2=0;
            limpa_lcd();
        }
        // Zera a flag de interrupção
        INT0IF = 0;
        return;
    }
// Atendimento de evento dos jogadores 1 e 2
    if (INT1IF) {
        jogador1 = 1;
     // Zera a flag de interrupção
        INT1IF = 0;
        return;
    }
    if(INT2IF){
        jogador2 = 1;
     // Zera a flag de interrupção
        INT2IF = 0;
        return;
    }
    return; 
}

// para escrever caracteres no lcd com printf()
void putch(char data)
{
  escreve_lcd(data);   
}

void main(void) {
    
    GIE = 1;  // Desabilita interrupções globais
    PEIE = 1; // Habilita interrupções por perifericos
    INT0IE  = 1; // Habilita interrupção externa
    TMR2 = 0;    // sinaliza interrupção para PR2
    
    //CONFIGURANDO OS REGITRADORES DE ENTRADA E SAIDA
    
    // Configurações do LCD
    TRISD = 0;  // Todos os pinos do tris D serão de saida
    //Configura os tres bits menos significaftivos como saídas
    // RA(0, 1 e 2) serão saídas || Buzzer, led_jogador1 e led_jogador2 respectivamente
    TRISAbits.RA0 = 0;
    TRISAbits.RA1 = 0;
    TRISAbits.RA2 = 0;
    // Configura todos os pinos de RB como entradas
    // RB(0, 1 e 2) serão entradas || botões do mediador, jogador1 e jogador2 respectivamente
    TRISBbits.RB0 = 1;
    TRISBbits.RB1 = 1;
    TRISBbits.RB2 = 1;  
    
    // INICIALIZANDO OS PORTS
     PORTD = 0;
    
   // configura inicio do jogo
    int inicio = 0;
    //inicializando variaveis de controles em 0
    mediador = 0;
    jogador1 = 0;
    jogador2 = 0;
    
    T2CONbits.T2CKPS1 = 0; // Pre scaler 1:4
    T2CONbits.T2CKPS0 = 1; // Pre scaler 1:4
    PR2 = 250;  // 250 Para estourar a cada 1 ms    
    T2CONbits.T2OUTPS3 = 0;
    T2CONbits.T2OUTPS2 = 0;
    T2CONbits.T2OUTPS1 = 0;
    T2CONbits.T2OUTPS0 = 1;
    
    // Inicia o timer
    T2CONbits.TMR2ON = 1; 
    
   while(1){
       while(mediador == 1){
            if(TMR2IF){
               TMR2IF = 0;
               if(jogador1 == 0) cj1++;
               if(jogador2 == 0) cj2++; 
            }
            if((jogador1 == 1) && (jogador2 == 1)){
                mediador = 0;
           // Verifica o tempo dos jogadores e acende o led do jogador com menor tempo
            if(cj1 < cj2) PORTAbits.RA1 = 1;       
            else if (cj1 > cj2) PORTAbits.RA2 = 1;  
            if (cj1 == cj2) {
            PORTAbits.RA1 = 1;
            PORTAbits.RA2 = 1;  
            }
            inicio = ~inicio; // inverte o estado
        }
    } 
    while(inicio){
     inicializa_lcd();
     limpa_lcd();
     //define ponto de inicio da primeira frase
     caracter_inicio(1,1);
     printf("Play_1: %d ms", cj1);
     __delay_ms(500);
     //define ponto de inicio da segunda frase
     caracter_inicio(2,1);
     printf("Play_2: %d ms", cj2);
     __delay_ms(500); 
     inicio = ~inicio;
    }
}
return;
}


