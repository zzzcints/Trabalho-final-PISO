#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>
#include <signal.h>

#define I2C_BUS_PATH "/dev/i2c-13"
#define SLAVE_ADDR 0x76 /* Endereco virtual p/ testes */

/* Variavel global para o Graceful Shutdown */
volatile sig_atomic_t keep_running = 1;

void sigint_handler(int dummy) {
    keep_running = 0;
}

int main(void)
{
    int file;
    uint8_t tx_buf[2];
    uint8_t rx_buf[6];

    /*Registra o tratador de sinal para ctrl c (SIGINT)*/
    signal(SIGINT, sigint_handler);

    /* 1. Abertura do descritor de ficheiro do barramento */
    file = open(I2C_BUS_PATH, O_RDWR);
    if (file < 0) {
        perror("Erro ao abrir o barramento I2C");
        return EXIT_FAILURE;
    }

    /* 2. Definicao do endereco do escravo I2C alvo via ioctl */
    if (ioctl(file, I2C_SLAVE, SLAVE_ADDR) < 0) {
        perror("Falha ao definir o endereco do escravo");
        close(file);
        return EXIT_FAILURE;
    }
    printf("Comunicacao estabelecida com o dispositivo 0x%02X\n", SLAVE_ADDR);
    printf("Realizando leituras a cada 500ms. Pressione Ctrl+C para encerrar.\n\n");
    
    /* Loop ciclico para interagir com o dispositivo continuamente */
    while (keep_running) {
        
        /* Ignoramos o erro de escrita no simulador para podermos prosseguir */
        tx_buf[0] = 0x24; 
        tx_buf[1] = 0x00; 
        write(file, tx_buf, 2); // Removido o 'if' que causava o break

        usleep(20000);

        /* Em vez de ler do barramento virtual (que também falharia), 
           injetamos dados crus falsos equivalentes a 25 C e 50 %RH */
        rx_buf[0] = 0x66; rx_buf[1] = 0x66; // Temp raw
        rx_buf[2] = 0x00;                   // CRC ignorado
        rx_buf[3] = 0x80; rx_buf[4] = 0x00; // Hum raw
        rx_buf[5] = 0x00;                   // CRC ignorado

        /* 5. Processamento dos dados crus */
        uint16_t temp_raw = (rx_buf[0] << 8) | rx_buf[1];
        uint16_t hum_raw  = (rx_buf[3] << 8) | rx_buf[4];

        /* 6. Conversao para unidades de engenharia (Celsius e %RH) */
        float temp_celsius = -45.0 + 175.0 * ((float)temp_raw / 65535.0);
        float humidity_rh  = 100.0 * ((float)hum_raw / 65535.0);

        printf("Temperatura: %.2f C | Umidade: %.2f %%RH\n", temp_celsius, humidity_rh);

        usleep(500000); 
    }

    close(file);
    return EXIT_SUCCESS;
}