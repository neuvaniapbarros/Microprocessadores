; TODO INSERT CONFIG CODE HERE USING CONFIG BITS GENERATOR
 ;Implemente um programa, em assembler para o PIC18F4550, que apresente, continuamente no PORTD,
 ;o valor de uma variável de 8 bits. O programa deve ler o estado lógico de duas chaves (SW1 e SW2)
 ;e incrementar a variável, a cada 1 segundo, quando SW1 estiver pressionada e decrementar a variável,
 ;a cada 1 segundo, quando SW2 estiver pressionada. Se nenhuma chave estiver pressionada, 
 ;o programa apenas atualiza o PORTD com o valor atual da variável.

#include <p18f4550.inc>

CONFIG  FOSC = XT_XT          ; Oscillator Selection bits (XT oscillator)
CONFIG  WDT = OFF             ; Watchdog desabilitado
CONFIG  LVP = OFF             ; Single-Supply ICSP disabled    

;SESSÃO DE VARIÁVEIS 
UDATA_ACS 0X00
 CONT res 1		;Contador 
 CONTA500ms res 1      ;Gera o atraso de tempo
 CONTA2ms   res 1	
 

RES_VECT  CODE    0x0000 ; processor reset vector
  GOTO    START          ; go to beginning of program

MAIN_PROG CODE ; let linker place main program
 
START
    CLRF CONT	     ;Zera a variável cont
    MOVLW b'00000000';Configura todos os pinos como saida
    MOVWF TRISD
    MOVLW b'11000000';Configura RC7 e RC6 como saida e demais como entrada
    MOVWF TRISC
    
LOOP 
    CALL SW1
    CALL SW2   
    MOVFF CONT, PORTD  ;Move contador para portd
    GOTO LOOP          ;Reinicia o processo 
    
SW1 
    CALL ATRASO1sec
    BTFSC PORTC,6     ;Verifica estado de SW1
    INCF CONT	      ;Se SW1 estiver pressionada incrementa
    RETURN
SW2
    CALL ATRASO1sec
    BTFSC PORTC, 7    ;Verifica o estado de SW2
    DECF CONT	      ;Se SW2 estiver pressionada decrementa
    RETURN
    
    
ATRASO1sec
    CALL ATRASO500ms; Espera 500 ms
    CALL ATRASO500ms; Espera 500 ms
    RETURN
    
ATRASO500ms
    MOVLW .250
    MOVWF CONTA500ms
LOOP500ms
    CALL ATRASO2ms
    DECFSZ ATRASO500ms
    GOTO LOOP500ms   ;Ate zerar
    RETURN
    
ATRASO2ms
    MOVLW .152
    MOVWF CONTA2ms
LOOP2ms
    NOP
    DECFSZ CONTA2ms
    GOTO LOOP2ms   ;Ate zerar
    RETURN   
END