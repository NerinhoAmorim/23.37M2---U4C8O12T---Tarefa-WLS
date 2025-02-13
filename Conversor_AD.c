#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "hardware/adc.h"
#include "hardware/pwm.h"
#include "include/ssd1306.h"
#include "include/font.h"

// Definição dos pinos
#define BUTTON_A 5
#define LED_G 11
#define LED_B 12
#define LED_R 13
#define I2C_SDA 14
#define I2C_SCL 15
#define JOYSTICK_BUTTON 22
#define JOYSTICK_X 26
#define JOYSTICK_Y 27

// Configuração I2C
#define I2C_ID i2c1
#define I2C_ADDR 0x3C
#define I2C_FREQ 100000 // 100kHz

// Configuração PWM
const uint16_t PWM_WRAP = 4095; // Valor máximo do contador PWM
const float PWM_DIV = 255.0;    // Divisor do clock para o PWM
uint16_t pwm_duty_cycle = 0;    // Ciclo de trabalho inicial do PWM
bool pwm_enabled = true;        // Estado do PWM (ligado/desligado)

// Variáveis globais
static volatile uint32_t last_interrupt_time = 0; // Tempo do último evento de interrupção
ssd1306_t display;                                // Estrutura do display SSD1306
uint8_t border_style = 0;                         // Estilo da borda (0: tracejada, 1: pontilhada)

// Protótipos das funções
void gpio_irq_handler(uint gpio, uint32_t events);
void initialize_peripherals();
void update_leds_and_display();
void draw_border(uint8_t style); // Função para desenhar a borda com o estilo especificado
void draw_dashed_rect(int x, int y, int width, int height); // Função para desenhar borda tracejada
void draw_dotted_rect(int x, int y, int width, int height); // Função para desenhar borda pontilhada

int main() {
    stdio_init_all();
    initialize_peripherals();

    // Configura interrupções para os botões
    gpio_set_irq_enabled_with_callback(BUTTON_A, GPIO_IRQ_EDGE_FALL, true, &gpio_irq_handler);
    gpio_set_irq_enabled_with_callback(JOYSTICK_BUTTON, GPIO_IRQ_EDGE_FALL, true, &gpio_irq_handler);

    // Desenha a borda inicial do display (tracejada)
    draw_border(border_style);
    ssd1306_send_data(&display);

    uint16_t x, y;                // Valores atuais dos eixos do joystick
    uint16_t prev_x = 0, prev_y = 0; // Valores anteriores dos eixos do joystick
    uint16_t x_offset = 2048, y_offset = 2048; // Offsets para centralizar o joystick

    // Leitura inicial dos offsets do joystick
    adc_select_input(1);
    x_offset = adc_read();
    adc_select_input(0);
    y_offset = adc_read();

    while (true) {
        // Leitura dos eixos X e Y do joystick
        adc_select_input(1);
        x = adc_read();
        pwm_set_gpio_level(LED_R, abs(x - x_offset));

        adc_select_input(0);
        y = adc_read();
        pwm_set_gpio_level(LED_B, abs(y - y_offset));

        // Normalização dos valores de X e Y para o display
        x = x / 39 + 8;           // Ajuste para o eixo X
        y = (4096 - y) / 91 + 7;  // Ajuste para o eixo Y

        // Atualiza a posição do quadrado no display
        ssd1306_rect(&display, prev_y, prev_x, 8, 8, 0, 1); // Apaga o quadrado anterior
        ssd1306_rect(&display, y, x, 8, 8, 1, 1);           // Desenha o quadrado na nova posição
        ssd1306_send_data(&display);

        // Atualiza os valores anteriores
        prev_x = x;
        prev_y = y;
    }
}

// Manipulador de interrupção para os botões
void gpio_irq_handler(uint gpio, uint32_t events) {
    uint32_t current_time = to_us_since_boot(get_absolute_time());

    // Debouncing: verifica se passou tempo suficiente desde o último evento
    if (current_time - last_interrupt_time > 300000) { // 300 ms de debouncing

        if (gpio == BUTTON_A) {
            // Alterna o estado do PWM
            uint slice = pwm_gpio_to_slice_num(LED_B);
            pwm_set_enabled(slice, pwm_enabled = !pwm_enabled);
            slice = pwm_gpio_to_slice_num(LED_R);
            pwm_set_enabled(slice, pwm_enabled);
        }

        if (gpio == JOYSTICK_BUTTON) {
            // Alterna o estado do LED verde
            gpio_put(LED_G, !gpio_get(LED_G));

            // Alterna o estilo da borda entre tracejada e pontilhada
            border_style = (border_style + 1) % 2; // Alterna entre 0 e 1
            draw_border(border_style); // Desenha a borda com o novo estilo
        }

        last_interrupt_time = current_time; // Atualiza o tempo do último evento
    }
}

// Função para desenhar a borda com o estilo especificado
void draw_border(uint8_t style) {
    if (style == 0) {
        // Borda tracejada
        draw_dashed_rect(3, 3, 122, 60);
    } else if (style == 1) {
        // Borda pontilhada
        draw_dotted_rect(3, 3, 122, 60);
    }
    ssd1306_send_data(&display);
}

// Função para desenhar uma borda tracejada
void draw_dashed_rect(int x, int y, int width, int height) {
    // Desenha linhas tracejadas nos quatro lados do retângulo
    for (int i = x; i < x + width; i += 4) {
        ssd1306_draw_pixel(&display, i, y, 1); // Linha superior
        ssd1306_draw_pixel(&display, i, y + height - 1, 1); // Linha inferior
    }
    for (int j = y; j < y + height; j += 4) {
        ssd1306_draw_pixel(&display, x, j, 1); // Linha esquerda
        ssd1306_draw_pixel(&display, x + width - 1, j, 1); // Linha direita
    }
}

// Função para desenhar uma borda pontilhada
void draw_dotted_rect(int x, int y, int width, int height) {
    // Desenha pontos nos quatro lados do retângulo
    for (int i = x; i < x + width; i += 2) {
        ssd1306_draw_pixel(&display, i, y, 1); // Linha superior
        ssd1306_draw_pixel(&display, i, y + height - 1, 1); // Linha inferior
    }
    for (int j = y; j < y + height; j += 2) {
        ssd1306_draw_pixel(&display, x, j, 1); // Linha esquerda
        ssd1306_draw_pixel(&display, x + width - 1, j, 1); // Linha direita
    }
}

// Inicialização dos periféricos
void initialize_peripherals() {
    // Inicializa o LED verde
    gpio_init(LED_G);
    gpio_set_dir(LED_G, GPIO_OUT);
    gpio_put(LED_G, 0);

    // Inicializa os LEDs Azul e Vermelho como PWM
    gpio_set_function(LED_B, GPIO_FUNC_PWM);
    uint slice = pwm_gpio_to_slice_num(LED_B);
    pwm_set_clkdiv(slice, PWM_DIV);
    pwm_set_wrap(slice, PWM_WRAP);
    pwm_set_gpio_level(LED_B, pwm_duty_cycle);
    pwm_set_enabled(slice, true);

    gpio_set_function(LED_R, GPIO_FUNC_PWM);
    slice = pwm_gpio_to_slice_num(LED_R);
    pwm_set_clkdiv(slice, PWM_DIV);
    pwm_set_wrap(slice, PWM_WRAP);
    pwm_set_gpio_level(LED_R, pwm_duty_cycle);
    pwm_set_enabled(slice, true);

    // Inicializa os botões
    gpio_init(BUTTON_A);
    gpio_set_dir(BUTTON_A, GPIO_IN);
    gpio_pull_up(BUTTON_A);

    gpio_init(JOYSTICK_BUTTON);
    gpio_set_dir(JOYSTICK_BUTTON, GPIO_IN);
    gpio_pull_up(JOYSTICK_BUTTON);

    // Inicializa o ADC para o joystick
    adc_init();
    adc_gpio_init(JOYSTICK_X);
    adc_gpio_init(JOYSTICK_Y);

    // Inicializa o I2C
    i2c_init(I2C_ID, I2C_FREQ);
    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);

    // Inicializa o display SSD1306
    ssd1306_init(&display, WIDTH, HEIGHT, false, I2C_ADDR, I2C_ID);
    ssd1306_config(&display);
    ssd1306_send_data(&display);

    // Limpa o display
    ssd1306_fill(&display, false);
    ssd1306_send_data(&display);
}