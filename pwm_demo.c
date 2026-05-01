/*
 * ATmega328P PWM Demo - LED Fade & Servo Control
 * See PWM_DOCUMENTATION.txt for detailed technical reference
 */

#define F_CPU 16000000UL
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

/* Millis counter for non-blocking timing */
volatile uint32_t millis = 0;

/* Timer2 ISR: increments millis every 1 ms */
ISR(TIMER2_COMPA_vect) {
    millis++;
}

/* TIMER0: LED Fade (8-bit, OC0A on PD6, 976 Hz PWM) */

void timer0_init(void) {
    DDRD |= (1 << PD6);
    TCCR0A = (1 << COM0A1) | (0 << COM0A0) | (1 << WGM01) | (1 << WGM00);
    TCCR0B = (0 << FOC0A) | (0 << FOC0B) | (0 << WGM02) | (0 << CS02) | (1 << CS01) | (1 << CS00);
    OCR0A = 0;
}

/* TIMER1: Servo Control (16-bit, OC1A on PB1, 50 Hz PWM) */

void timer1_init(void) {
    DDRB |= (1 << PB1);
    TCCR1A = (1 << COM1A1) | (0 << COM1A0) | (1 << WGM11) | (0 << WGM10);
    TCCR1B = (1 << WGM13) | (1 << WGM12) | (1 << CS11) | (1 << CS10);
    ICR1 = 4999;
    OCR1A = 250;
}

/* Initialize Timer2 for millis counter (1 ms resolution) */
void init_millis(void) {
    /* Timer2: CTC mode, prescaler 64, 16MHz -> 250kHz, OCR2A = 249 -> 1ms interrupt */
    TCCR2A = (1 << WGM21);
    TCCR2B = (1 << CS22);   /* prescaler 64 */
    OCR2A = 249;
    TIMSK2 = (1 << OCIE2A);
    sei();
}

/* Non-blocking LED fade update (call regularly in main loop) */
void update_led_fade(uint32_t now) {
    static uint32_t last_update = 0;
    static uint8_t brightness = 0;
    static int8_t direction = 1;
    
    if (now - last_update >= 8) {
        last_update = now;
        OCR0A = brightness;
        brightness += direction;
        if (brightness == 255) direction = -1;
        else if (brightness == 0) direction = 1;
    }
}

/* Non-blocking servo sweep update (call regularly in main loop) */
void update_servo_sweep(uint32_t now) {
    static uint32_t last_update = 0;
    static uint16_t pulse = 250;
    static int8_t dir = 1;
    
    if (now - last_update >= 30) {
        last_update = now;
        OCR1A = pulse;
        pulse += dir;
        if (pulse >= 500) dir = -1;
        else if (pulse <= 250) dir = 1;
    }
}

/* ============================================================================
 * MAIN PROGRAM - Non-blocking event loop with interrupt-driven timing
 * ============================================================================
 */

int main(void) {
    timer0_init();
    timer1_init();
    init_millis();
    
    while (1) {
        uint32_t now = millis;
        update_led_fade(now);
        update_servo_sweep(now);
        /* CPU is free to do other tasks */
    }
    
    return 0;
}
