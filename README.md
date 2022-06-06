# Iniciativa Arduino IoT - IFRS Caxias do Sul - Catraca 4.0 – Internet do fim do mundo.
## Projeto de pesquisa - IMPLEMENTAÇÃO DA OFICINA INDÚSTRIA 4.0
Esse projeto utiliza uma Arduino Uno r3 com modulos: WiFi esp8266, GPS e dois botoes.

[TOC]




## Fluxograma da função main
```mermaid
graph TD;
A((Início)) --> B[Iniciar módulo GPS]
    B[Iniciar módulo GPS] --> C[Iniciar Serial]
    C[Iniciar Serial] --> D[Iniciar módulo WiFi]
    D[Iniciar módulo WiFi] --> E{Conectado?} 
    E{Conectado?} -- yes --> F[Chamar main]
    E{Conectado?} -- no --> D[Iniciar módulo WiFi]
    F[Chamar main] --> G[Checar Cliente MQTT]
    G[Checar Cliente MQTT] --> H{Cliente MQTT Conectado?}
    H{Cliente MQTT Conectado?} -- no --> I[Conectar cliente MQTT]
    H{Cliente MQTT Conectado?} -- yes --> J[Adquirir telemetria]
    subgraph Adquirir e Enviar Telemetria
        J[Adquirir telemetria] --> K[Mandar telemetria para GCP]
        K[Mandar telemetria para GCP] --> H{Cliente MQTT Conectado?}
    end
    subgraph Conectar MQTT
    I[Conectar cliente MQTT] --> H{Cliente MQTT Conectado?}
    end
```
##
   
    
