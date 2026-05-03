# ADR 0006 — Frame binário de 24 bytes com HMAC-SHA256 + CRC-16

- **Status:** aceito
- **Data:** 2026-05-03

## Contexto

O Pico TX precisa transmitir leituras de 3 sensores + metadados via LoRa para o Pico RX. O LoRa impõe restrições severas de payload: em SF10/BW125 (configuração de maior alcance usada no projeto), o payload útil máximo é ~51 bytes. Qualquer formato de dados precisa:

1. Caber nesse limite sem fragmentação.
2. Garantir integridade dos dados (o canal de rádio está sujeito a ruído).
3. Autenticar a origem (evitar que um dispositivo externo injete leituras falsas na API).
4. Incluir um número de sequência para deduplicação no receptor.

## Decisão

Frame binário de **24 bytes**, little-endian:

```
[device_id: 8B][seq: 2B][s1: 2B][s2: 2B][s3: 2B][bat_mv: 2B][hmac4: 4B][crc16: 2B]
```

- `device_id`: serial único do chip (8 bytes, lido do registro de hardware do RP2040/RP2350).
- `seq`: uint16 incrementado a cada transmissão, persistido em flash para sobreviver a resets.
- `s1/s2/s3`: leitura ADC 12-bit do sensor capacitivo, com oversampling ×8, armazenado em uint16.
- `bat_mv`: tensão da bateria em mV (uint16), permite alerta de bateria fraca.
- `hmac4`: primeiros 4 bytes do HMAC-SHA256 calculado sobre os 18 bytes anteriores com a PSK do dispositivo.
- `crc16`: CRC-16/CCITT-FALSE sobre os 22 bytes anteriores (payload + hmac4), detecta corrupção em trânsito.

As implementações de SHA-256, HMAC e CRC-16 vivem na biblioteca compartilhada `hardware/firmware/shared` (`aquasense_proto`) e têm testes de unidade nativos (PlatformIO + Unity).

## Consequências

**Positivas**
- 24 B cabe folgado em qualquer SF do LoRa, inclusive SF12.
- Dois níveis de proteção independentes: CRC-16 detecta erros de transmissão; HMAC-SHA256 autentica a origem.
- `seq` permite ao receptor descartar retransmissões duplicadas (deduplicação por janela deslizante — Etapa 6).
- Formato determinístico: `frame_pack()` / `frame_unpack()` sem alocação dinâmica, seguro para microcontroladores.

**Negativas**
- Binário é mais difícil de depurar que JSON (requer ferramenta para inspecionar bytes).
- PSK por dispositivo exige gestão de chaves: cada par TX/RX precisa ter o mesmo `secrets.h`.
- HMAC truncado a 4 bytes oferece ~32 bits de segurança — suficiente para evitar injeção casual, mas não é segurança criptográfica robusta.
- Sem espaço para campos extras sem redesenhar o frame (ex: temperatura do nó).

## Alternativas consideradas

- **JSON** — descartado: `{"device_id":"...","s1":0.72,...}` ocupa >80 bytes, não cabe em SF10 sem fragmentação.
- **MessagePack** — considerado: compacto, mas requer biblioteca adicional no firmware e não agrega segurança.
- **CRC-16 sem HMAC** — descartado: detecta corrupção mas não autentica; qualquer dispositivo próximo com um Pico poderia injetar leituras falsas.
- **HMAC completo (32 bytes)** — descartado: sozinho já ultrapassa o limite de payload LoRa em SF10.
- **AES-CCM** — descartado para MVP: criptografia + autenticação integradas seriam ideais, mas a implementação em C puro para RP2040 está fora do escopo do MVP.
