#include <stdint.h>

#define PERIPH_BASE         (0x40000000UL)
#define APB2PERIPH_BASE     (PERIPH_BASE + 0x10000UL)
#define AHBPERIPH_BASE      (PERIPH_BASE + 0x20000UL)

#define RCC_BASE            (AHBPERIPH_BASE + 0x1000UL)
#define GPIOA_BASE          (APB2PERIPH_BASE + 0x0800UL)
#define GPIOB_BASE          (APB2PERIPH_BASE + 0x0C00UL)

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

    GPIOA->CRH &= ~(0x0FUL << 16);
    GPIOA->CRH |= (0x08UL << 16);
    GPIOA->ODR |= (1UL << 12);

    GPIOB->CRH &= ~(0x0FUL << 12);
    GPIOB->CRH |= (0x02UL << 12);
    GPIOB->ODR &= ~(1UL << 11);

    while (1) {
        if (!(GPIOA->IDR & (1UL << 12))) {
            delay_ms(20);

            if (!(GPIOA->IDR & (1UL << 12))) {
                while (!(GPIOA->IDR & (1UL << 12)));
                delay_ms(20);

                GPIOB->ODR ^= (1UL << 11);
            }
        }
    }
}
