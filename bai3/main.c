#include <stdint.h>

#define PERIPH_BASE        (0x40000000UL)
#define APB2PERIPH_BASE    (PERIPH_BASE + 0x10000UL)
#define AHBPERIPH_BASE     (PERIPH_BASE + 0x20000UL)

#define RCC_BASE           (AHBPERIPH_BASE + 0x1000UL)
#define GPIOA_BASE         (APB2PERIPH_BASE + 0x0800UL)
#define GPIOB_BASE         (APB2PERIPH_BASE + 0x0C00UL)

typedef struct {
    volatile uint32_t CR;
    volatile uint32_t CFGR;
    volatile uint32_t CIR;
    volatile uint32_t APB2RSTR;
    volatile uint32_t APB1RSTR;
    volatile uint32_t AHBENR;
    volatile uint32_t APB2ENR;
    volatile uint32_t APB1ENR;
} RCC_TypeDef;

typedef struct {
    volatile uint32_t CRL;
    volatile uint32_t CRH;
    volatile uint32_t IDR;
    volatile uint32_t ODR;
    volatile uint32_t BSRR;
    volatile uint32_t BRR;
    volatile uint32_t LCKR;
} GPIO_TypeDef;

typedef struct {
    volatile uint32_t CTRL;
    volatile uint32_t LOAD;
    volatile uint32_t VAL;
    volatile uint32_t CALIB;
} SysTick_TypeDef;

#define RCC         ((RCC_TypeDef *) RCC_BASE)
#define GPIOA       ((GPIO_TypeDef *) GPIOA_BASE)
#define GPIOB       ((GPIO_TypeDef *) GPIOB_BASE)
#define SysTick     ((SysTick_TypeDef *) 0xE000E010UL)

void delay_ms(uint32_t ms) {
    if (ms == 0) return;

    SysTick->LOAD = 8000 - 1;
    SysTick->VAL = 0;
    SysTick->CTRL = 0x05;

    for (uint32_t i = 0; i < ms; i++) {
        while (!(SysTick->CTRL & (1 << 16)));
    }

    SysTick->CTRL = 0;
}

int main(void) {
    RCC->APB2ENR |= (1 << 2) | (1 << 3);

    GPIOA->CRL = 0x88888888UL;
    GPIOA->ODR |= 0x00FFUL;

    GPIOA->CRH &= ~(0x000FFFFFUL | (0xFUL << 28)); 
    GPIOA->CRH |=  (0x00022222UL | (0x2UL << 28));

    GPIOB->CRH &= ~(0x00FF0000UL);
    GPIOB->CRH |=  (0x00220000UL);

    uint8_t led_state = 0x00;
    uint8_t last_btn_state = 0xFF;

    while (1) {
        uint8_t read1 = (uint8_t)(GPIOA->IDR & 0xFFUL);
        delay_ms(20);
        uint8_t read2 = (uint8_t)(GPIOA->IDR & 0xFFUL);

        if (read1 == read2) {
            uint8_t current_btn = read1;
            uint8_t pressed = last_btn_state & (~current_btn);

            if (pressed) {
                led_state ^= pressed;

                GPIOA->ODR &= ~((0x1FUL << 8) | (1UL << 15));
                GPIOA->ODR |= ((uint32_t)(led_state & 0x1F) << 8);
                GPIOA->ODR |= ((uint32_t)((led_state >> 7) & 0x01) << 15);

                GPIOB->ODR &= ~(0x1UL << 13);
                GPIOB->ODR &= ~(0x1UL << 14);
                GPIOB->ODR |= ((uint32_t)((led_state >> 5) & 0x03) << 13);
            }

            last_btn_state = current_btn;
        }
    }
}
