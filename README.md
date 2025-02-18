
# Conversor AD com Controle de LEDs RGB e Display OLED

Este projeto utiliza um microcontrolador Raspberry Pi Pico para ler valores de um joystick e controlar a intensidade de dois LEDs RGB via PWM. Além disso, um display OLED SSD1306 é utilizado para exibir a posição do joystick e um quadrado que se move proporcionalmente aos valores capturados.

## Funcionalidades

- **Leitura do Joystick:** O projeto lê os valores dos eixos X e Y do joystick, que são utilizados para controlar a posição de um quadrado no display OLED e a intensidade dos LEDs RGB.
  
- **Controle de LEDs RGB:** A intensidade dos LEDs RGB é controlada suavemente com base nos valores do joystick. O LED vermelho e o LED verde são mapeados diretamente para os eixos X e Y, enquanto o LED azul é definido como a cor inversa do vermelho.

- **Movimento do Quadrado:** Um quadrado de 8x8 pixels é desenhado no display OLED e se move de acordo com os valores lidos do joystick.

- **Botão A:** O botão A do joystick é utilizado para ativar ou desativar os LEDs RGB. Cada vez que o botão é pressionado, o estado dos LEDs é alternado.

## Requisitos

- Raspberry Pi Pico ou Raspberry Pi Pico W  
- Joystick analógico  
- Display OLED SSD1306  
- LEDs RGB  
- Resistores apropriados para os LEDs  
- Fios de conexão  
- Ambiente de desenvolvimento configurado para Raspberry Pi Pico (incluindo o SDK)  

## Como Usar

### Clonar o Repositório  
Clone o repositório do GitHub para o seu ambiente local:  

```bash
git clone https://github.com/NerinhoAmorim/23.37M2---U4C8O12T---Tarefa-WLS.git
```

## Autor

Desenvolvido por [Nerinho Amorim](https://github.com/NerinhoAmorim).

## Vídeo de Apresentação
[Assistir Aqui](https://vimeo.com/1057980559/e54b766953).

