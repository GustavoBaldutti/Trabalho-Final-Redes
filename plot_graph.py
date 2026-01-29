import pandas as pd
import matplotlib.pyplot as plt
import sys

# Nome do arquivo de log
FILENAME = 'congestion_log.csv'

try:
    # Ler o CSV
    df = pd.read_csv(FILENAME)
except FileNotFoundError:
    print(f"Erro: Arquivo {FILENAME} não encontrado. Rode o sender.exe primeiro!")
    sys.exit(1)

# --- Geração de Estatísticas ---
total_time = df['time'].iloc[-1]
total_packets = len(df) # Aproximação baseada nas linhas de log
avg_cwnd = df['cwnd'].mean()
max_cwnd = df['cwnd'].max()

print("="*30)
print("     ESTATÍSTICAS DA REDE")
print("="*30)
print(f"Tempo Total:      {total_time:.2f} segundos")
print(f"Média da Janela:  {avg_cwnd:.2f} pacotes")
print(f"Pico da Janela:   {max_cwnd:.0f} pacotes")
print(f"Vazão Média (aprox): {total_packets / total_time:.2f} atualizações/seg")
print("="*30)

# --- Plotagem do Gráfico ---
plt.figure(figsize=(10, 6))

# Plotar CWND (Tamanho da Janela)
plt.plot(df['time'], df['cwnd'], label='CWND (Janela de Congestionamento)', color='blue', linewidth=1.5)

# Plotar SSTHRESH (Limiar)
plt.plot(df['time'], df['ssthresh'], label='SSTHRESH (Limiar)', color='red', linestyle='--', alpha=0.7)

plt.title('Análise de Controle de Congestionamento (TCP Reno sobre UDP)')
plt.xlabel('Tempo (segundos)')
plt.ylabel('Tamanho da Janela (Pacotes)')
plt.grid(True, linestyle=':', alpha=0.6)
plt.legend()

# Salvar o gráfico em arquivo
plt.savefig('grafico_congestionamento.png', dpi=300)
print("Gráfico salvo como 'grafico_congestionamento.png'")

# Mostrar na tela
plt.show()