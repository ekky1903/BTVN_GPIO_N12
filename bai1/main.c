#include <stdint.h>

#define PERIPH_BASE        (0x40000000UL)
#define APB2PERIPH_BASE    (PERIPH_BASE + 0x10000UL)
#define AHBPERIPH_BASE     (PERIPH_BASE + 0x20000UL)

#define RCC_BASE           (AHBPERIPH_BASE + 0x1000UL)
#define GPIOC_BASE         (APB2PERIPH_BASE + 0x1000UL)

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

#define RCC       ((RCC_TypeDef *) RCC_BASE)
#define GPIOC     ((GPIO_TypeDef *) GPIOC_BASE)
#define SysTick   ((SysTick_TypeDef *) 0xE000E010UL)

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
    RCC->APB2ENR |= (1 << 4);

    GPIOC->CRH &= ~(0x0FUL << 20);
    GPIOC->CRH |=  (0x02UL << 20);

    GPIOC->BSRR = (1 << 13);

    while (1) {
        GPIOC->ODR ^= (1 << 13);
        delay_ms(1000);
    }
}
