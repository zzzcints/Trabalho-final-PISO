# Trabalho-final-PISO

como rodar o arquivo de testes:

para simular um barramento I2C no Kernel Linux, carregue o módulo i2c-stub:

sudo modprobe i2c-stub chip_addr=0x76

para verificar em qual número de barramento o dispositivo virtual foi alocado (exemplo: i2c-13), inspecione a lista de barramentos:

i2cdetect -l

compilar:

gcc -Wall -O2 i2c_app-testes.c -o i2c_app-testes

executar:
como o acesso aos dispositivos /dev/i2c-* exige privilégios de superusuário, execute o binário gerado com sudo:

sudo ./i2c_app-testes

saída esperada:

Comunicacao estabelecida com o dispositivo 0x44
Realizando leituras a cada 500ms. Pressione Ctrl+C para encerrar.

Temperatura: 25.00 C | Umidade: 50.00 %RH
Temperatura: 25.00 C | Umidade: 50.00 %RH
…

limpeza (opcional):

sudo rmmod i2c-stub

observações:
1. é necessário ter instalado o i2c-tools
2. talvez seja necessário alterar a linha 10 do cod. C dependendo da saída de 
i2cdetect -l
