# Trabalho Final – Redes de Computadores  
## Protocolo de Transporte Confiável sobre UDP (RUDP)

Este repositório contém a implementação do Trabalho Final da disciplina de **Redes de Computadores**.  
O objetivo do projeto é desenvolver um **protocolo de transporte confiável**, inspirado nos mecanismos do TCP, operando sobre o protocolo **UDP**, que é inerentemente não confiável.

A implementação é realizada na **camada de aplicação**, com foco didático, reproduzindo conceitos fundamentais de protocolos de transporte, como confiabilidade, controle de fluxo e controle de congestionamento.

---

## Objetivos do Projeto

- Implementar confiabilidade sobre o protocolo UDP  
- Garantir entrega ordenada de dados  
- Simular controle de congestionamento inspirado no TCP Reno  
- Implementar controle de fluxo baseado na janela do receptor  
- Analisar o comportamento da janela de congestionamento por meio de gráficos  

---

## Funcionalidades Implementadas

### 1. Entrega Ordenada
Utilização de números de sequência (`seq_num`) e números de reconhecimento (`ack_num`) para garantir que os dados sejam entregues ao receptor na ordem correta.

---

### 2. Confiabilidade
- Uso de **ACKs cumulativos**  
- Retransmissão de pacotes em caso de perda  
- Detecção de perdas por:
  - Estouro de tempo (timeout)  
  - Recebimento de ACKs duplicados  

---

### 3. Controle de Fluxo
O receptor informa sua janela de recepção disponível (`rwnd`) ao emissor.  
O emissor respeita esse valor para evitar sobrecarregar o receptor.

A janela efetiva de envio é definida por:
effective_window = min(cwnd, rwnd)


---

### 4. Controle de Congestionamento (Baseado no TCP Reno)

O controle de congestionamento é inspirado no algoritmo **TCP Reno**, implementado de forma simplificada, contendo os seguintes mecanismos:

- **Slow Start**  
  Crescimento exponencial da janela de congestionamento enquanto `cwnd < ssthresh`.

- **Congestion Avoidance**  
  Crescimento linear da janela quando `cwnd >= ssthresh`.

- **Fast Retransmit**  
  Retransmissão imediata do pacote após o recebimento de 3 ACKs duplicados.

- **Timeout**  
  Em caso de estouro de tempo:
  - `ssthresh` é reduzido para metade da janela atual  
  - `cwnd` é reiniciado para 1  

---

### 5. Ofuscação de Dados
O payload dos pacotes é ofuscado utilizando uma criptografia simples baseada em **XOR** com chave fixa.

Observação: este mecanismo possui finalidade **didática** e **não oferece segurança real**.

---

### 6. Simulação de Perda de Pacotes
O receptor pode descartar pacotes aleatoriamente, de acordo com uma probabilidade configurável, permitindo observar o comportamento do controle de congestionamento em cenários adversos.

---

## Tecnologias Utilizadas

- **Linguagem:** C (Padrão C17)  
- **Sockets:** Winsock2 (Windows) e BSD Sockets (Linux)  
- **Análise de Dados:** Python 3  
  - Pandas  
  - Matplotlib  
- **Compilador:** GCC (MinGW / MSYS2)  

---

## Como Compilar e Executar

### Pré-requisitos

- GCC instalado  
- Para geração de gráficos:
  - Python 3  
  - Bibliotecas Pandas e Matplotlib  

Instalação das bibliotecas Python:

pip install pandas matplotlib


## Compilação (Windows)

Abra o terminal na pasta do projeto e execute:
gcc receiver.c -o receiver.exe -lws2_32
gcc sender.c -o sender.exe -lws2_32

Execução

Abra dois terminais.

Terminal 1 – Receptor (Servidor)
./receiver.exe

Terminal 2 – Emissor (Cliente)
./sender.exe

O cliente iniciará um handshake simplificado, enviará os pacotes e gerará um arquivo de log do controle de congestionamento.


## Geração de Gráficos de Desempenho

Durante a execução, o emissor gera o arquivo:

congestion_log.csv

Formato do arquivo:

time,cwnd,ssthresh


# Legenda:

time: tempo desde o início da transmissão (segundos)

cwnd: janela de congestionamento

ssthresh: limiar de slow start

# Para gerar o gráfico:

execute:

python plot_graph.py


O script gera o arquivo:

grafico_congestionamento.png

O gráfico apresenta o comportamento típico em "dente de serra" do controle de congestionamento inspirado no TCP Reno.


## Configurações do Protocolo

Os parâmetros do protocolo podem ser ajustados diretamente no código-fonte:

Simulação de perda
Arquivo: receiver.c

#define LOSS_PROB 5


Tamanho máximo do payload (MSS)
Arquivo: protocol.h

#define MSS 1024


Quantidade total de pacotes enviados
Arquivo: sender.c

#define TOTAL_PACKETS 1000



## Limitações Conhecidas ##

Não há retransmissão seletiva (SACK)

Não há buffer de reordenação no receptor

O tempo de timeout é fixo, sem cálculo adaptativo de RTT

A criptografia XOR é apenas ilustrativa

Não há suporte a múltiplas conexões simultâneas

Implementação com foco didático, não destinada a uso em produção


-----

## Estrutura de Arquivos

1 - protocol.h
Definições de cabeçalhos, estruturas de pacotes, flags e funções auxiliares.

2 - receiver.c
Implementação do receptor (servidor), responsável por:

- Recebimento de pacotes

- Envio de ACKs

- Simulação de perda

3 - sender.c
Implementação do emissor (cliente), responsável por:

- Envio de pacotes

- Controle de congestionamento

- Timeout e retransmissões

- Geração do log CSV

4 - plot_graph.py

Script Python para análise e geração de gráficos do controle de congestionamento.


# -------------------------------------------------------------------------------

### Autores ###

- Ryan Carlos dos Reis Moreira – Matrícula: 202265089A

- Gustavo Henrique baldutti da silva - 202165566C

- Davi
