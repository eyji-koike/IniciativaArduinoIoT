
# Internet do fim do mundo - Catraca 4.0

Projeto de implementação da oficinas 4.0 no IFRS Campus Caxias do Sul.
>*~Equipe Iniciativa Arduino IoT*

>*[Read this in English]()*
>
>*[Read this in Spanish]()*
## Abstract
Esse projeto utiliza uma Arduino Uno r3 com módulos: WiFi esp01 baseado no chip esp8266, GPS e dois botões. O protocolo de transmissão utilizado é o **[Mosquito (MQTT)](https://mosquitto.org/)**  e o host para o backend é a **[Google Cloud Platform](https://cloud.google.com/)** com os servicos de IoT Hub, e PUB/SUB. Para nossa solução de retenção de dados, utilizamos um gatilho da google cloud functions que, quando acionado, manda os dados recebidos para o [Google Firebase](https://firebase.google.com/).
>***Disclaimer***  
*Esse projeto pode causar cobranças e os autores não são responsáveis. Realize-o sob sua total consiência e leia o material disponível com atenção. Boa aprendizagem.*

**Visão Geral**

O fluxo de informação vai ocorrer como representado na figura abaixo:

![Fluxo de informação](https://github.com/eyji-koike/IniciativaArduinoIoT/blob/main/Assets/InformationFluxogram.png)

**Tabela de conteúdo**

1. [Configurando a GCP](#configurando-a-gcp)
    
    1. [Setup IoT Hub](#setup-iot-hub)
    2. [Setup Cloud Pub/Sub](#setup-cloud-pubsub)
    3. [Easy way - script de automação](#easy-way---script-de-automação)

2. [Configurando o Arduino Uno r3 e a ESP01](#configurando-o-arduino-uno-r3-e-a-esp01)

    1. [Instalando as bibliotecas na arduino IDE](#instalando-as-bibliotecas-na-arduino-ide)
    2. [Fluxograma da Função Main](#fluxograma-da-função-main)
    3. [Adaptando o código em C++](#adaptando-o-código-em-c)

3. [Testando a transferência de dados](#testando-a-transferência-de-dados)

    1. [Verificação do Cloud Pub/Sub](#verificação-do-cloud-pubsub)

4. [Routing e armazenamento de dados](#routing-e-armazenamento-de-dados)

    1. [Setup cloud functions](#setup-cloud-functions)
    2. [Setup DB](#setup-db)

5. [Construção da dashboard](#construção-da-dashboard)

    1. [*decidir ferramenta*](#decidir-ferramenta)

6. [Integraçao com aplicativo Móvel](#integraçao-com-aplicativo-móvel)

    1. [Desenvolvimento Ionic](#desenvolvimento-ionic)
---

## Configurando a GCP

A Google oferece vários serviços de cloud hosting nas mais variadas modalidades. Para mais informações sobre o free tier, acesse [este link](https://cloud.google.com/free/docs/gcp-free-tier#free-tier). A configuração pode ser feita utilizando os menus, ou utilizando o shell. Há também um script que pode ser adaptado para tornar o provisionamento de estrutura automático.

### Setup IoT Hub

Primeiramente precisamos configar nomes para nossos serviços. Copie e cole as variáveis abaixo no seu bloco de notas, e preencha com o valor que achar adequado para seu projeto. Depois do preenchimento essas variáveis podem ser coladas diretamente no cloud shell, ou você pode utilizá-las como guia para preencher os campos nos menus do Google Cloud Console.

```shell
export PROJECT_ID=      #insira o nome do projeto aqui
export REGION=          #insira a regiao aqui
export TOPIC_ID=        #insisra a ID do topico aqui
export SUBSCRIPTION=    #insisra a subscricao aqui
export REGISTRY=        #insisra o nome do registro aqui
export DEVICE_ID=       #insira a identificacao do dispositivo
```
Com seus nomes (ou identificões, se preferir) em mãos, podemos prosseguir para a próxima etapa. Crie um projeto com o mesmo nome que escolheu em PROJECT_ID e 

### Setup Cloud Pub/Sub

### Easy way - script de automação

## Configurando o Arduino Uno r3 e a ESP01

Em nosso sistema, o Arduíno Uno é o responsável por realizar a coleta dos dados de telemetria, equanto a ESP01 ficará responsável por mandar a telemetria via MQTT, conectar-se ao Access Point Wi-Fi e conectar-se ao GCP.


### Instalando as bibliotecas na arduino IDE

### Fluxograma da função main

### Adaptando o código em C++

## Testando a transferência de dados

### Verificação do Cloud Pub/Sub

## Routing e armazenamento de dados

### Setup cloud functions

### Setup DB

## Construção da dashboard

### *~decidir ferramenta~*

## Integraçao com aplicativo Móvel

### Desenvolvimento Ionic
   
    
