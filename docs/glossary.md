# Glossário

Termos recorrentes no projeto.

## Hardware / RF

- **Pico W** — Raspberry Pi Pico W, microcontrolador RP2040 com WiFi nativo.
- **LoRa** — *Long Range*, modulação de rádio sub-GHz para baixa potência e longo alcance.
- **SX1276 / RFM95** — chips/módulos LoRa típicos, comunicação SPI.
- **SF (Spreading Factor)** — fator de espalhamento LoRa (SF7–SF12). Maior SF = mais alcance, menos throughput.
- **BW (Bandwidth)** — largura de banda do canal LoRa (tipicamente 125 kHz).
- **CR (Coding Rate)** — taxa de codificação de correção de erro (4/5 a 4/8).
- **ADC** — conversor analógico-digital. No RP2040, 12-bit lido como 16-bit (0–65535).
- **Sensor capacitivo de umidade** — sensor de solo sem corrosão (mais durável que resistivo).
- **Duty cycle** — fração do tempo em que o transmissor está no ar. Importante para regulamentação ISM.

## Agricultura

- **Evapotranspiração (ET)** — perda de água do solo por evaporação + transpiração da planta.
- **Lâmina d'água** — quantidade de água aplicada por área (mm).
- **Zona radicular** — profundidade do solo onde estão as raízes ativas. Irrigar abaixo dela é desperdício.
- **CODEVASF** — Companhia de Desenvolvimento dos Vales do São Francisco e do Parnaíba.
- **Perímetro irrigado** — área agrícola servida por infraestrutura pública de irrigação (ex. Nilo Coelho).

## Software

- **API REST** — interface HTTP que recebe leituras (`POST /readings`).
- **Dashboard** — interface web que mostra os dados em tempo real.
- **WhatsApp Cloud API** — meio de envio de alertas prescritivos ao produtor.
