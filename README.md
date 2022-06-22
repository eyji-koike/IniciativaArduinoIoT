
# Internet do fim do mundo - Catraca 4.0

Projeto de implementação da oficinas 4.0 no IFRS Campus Caxias do Sul.
>*~Equipe Iniciativa Arduino IoT*  
>*[Read this in English]()*  
>*[Lea esto en Español]()*  
## Abstract
Esse projeto utiliza uma Arduino Uno r3 com módulos: WiFi esp01 baseado no chip esp8266, GPS e dois botões. O protocolo de transmissão utilizado é o **[Mosquito (MQTT)](https://mosquitto.org/)**  e o host para o backend é a **[Google Cloud Platform](https://cloud.google.com/)** com os servicos de IoT Hub, e PUB/SUB. Para nossa solução de retenção de dados, utilizamos um gatilho da google cloud functions que, quando acionado, manda os dados recebidos para o [Google Firebase](https://firebase.google.com/).
>***Disclaimer***  
*Esse projeto pode causar cobranças e os autores não são responsáveis. Realize-o sob sua total consiência e leia o material disponível com atenção. Boa aprendizagem.*

**Visão Geral**

O fluxo de informação vai ocorrer como representado na figura abaixo:  
![Fluxo de informação](https://github.com/eyji-koike/IniciativaArduinoIoT/blob/main/Assets/InformationFluxogram.png)

**Tabela de conteúdo**

1. [Configurando a GCP](#configurando-a-gcp)
    
    1. [Setup IoT Core](#setup-iot-core)
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
Primeiramente precisamos configar nomes para nossos serviços. Copie e cole as variáveis abaixo no seu bloco de notas, e preencha com o valor que achar adequado para seu projeto. Depois do preenchimento essas variáveis podem ser coladas diretamente no cloud shell, ou você pode utilizá-las como guia para preencher os campos nos menus do Google Cloud Console.  
```shell
export PROJECT_ID=      #insira o nome do projeto aqui
export REGION=          #insira a regiao aqui
export TOPIC_ID=        #insisra a ID do topico aqui
export SUBSCRIPTION=    #insisra a subscricao aqui
export REGISTRY=        #insisra o nome do registro aqui
export DEVICE_ID=       #insira a identificacao do dispositivo
```
Com seus nomes (ou identificões, se preferir) em mãos, podemos prosseguir para a próxima etapa. Crie um projeto com o mesmo nome que escolheu em **PROJECT_ID** e tenha certeza de que a Cobrança está ativada, caso contrário o Google não permitirá a criação das nossas ferramentas. Para fazer isso, basta acessar o Cloud Shell e procurar por "Cobrança" ou "Billing" no menu de sanduíche que fica no lado esquerdo da tela. 

Para ativar os serviços necessários, va no menu à esquerda e selecione "APIs". No novo painel, aperte em adicionar serviços e APIs. Procure e ative **Cloud IoT, Pub/Sub e Cloud Functions**. Se quiser utilizar o shell, use o comando abaixo.  
```shell
gcloud services enable cloudiot.googleapis.com pubsub.googleapis.com cloudfunctions.googleapis.com
```  
Antes de iniciar a configuração dos serviços é necessário criar um conta para o serviço Cloud IoT pode publicar no PUB/SUB. Para isso, pesquise no menu por 
"IAM & Admin" e em IAM, aperte em "Adicionar". No primeiro campo, coloque o emails da nossa conta interna de serviço *cloud-iot@system.gserviceaccount.com* e em "Função" encontre a **Cloud PUB/SUB Publicador** e salve. Se estiver no Shell, use o comando abaixo.
```shell
gcloud projects add-iam-policy-binding $PROJECT_ID \
    --member=serviceAccount:cloud-iot@system.gserviceaccount.com \
    --role=roles/pubsub.publisher
```  
Também precisamos criar uma chave elíptica privada e uma pública de 256bits para autenticar nosso dispositivo. Aqui recomendamos que utilize o cloud shell pois também armazenamos ela no projeto.  
```shell
openssl ecparam -genkey -name prime256v1 -noout -out ec_private.pem
openssl ec -in ec_private.pem -pubout -out ec_public.pem
```
Agora vá em "Abrir editor" e ache suas chaves. Abra-as no editor e copie e cole em um arquivo de texto em lugar seguro para sua consulta posterior.   

>***Importante***  
>**Não compartilhe suas chaves com ninguém. Isso representa um risco a segurança e integridade do seu projeto.**  
### Setup Cloud Pub/Sub  

**Utilizando os menus**  
Para criar um [PUB/SUB](https://cloud.google.com/pubsub), pesquise na caixa do GCP por "Pub/Sub" ou role o menu em sanduíche até encontrar o serviço. Sua tela provavelmente estará em branco, então aperte no botão "Criar um Tópico". No novo menu que aparecerá, em "ID do tópico", coloque o nome definido anteriormente em **TOPIC_ID** e crie seu tópico. Após a mensagem de que o tópico foi criado com sucesso, no menu da esquerda vá em "Subescrições" e selecione "Criar Subescrição". Na nova tela aberta, coloque o nome que definiu em **SUBSCRITION** e selecione o nome do tópico que foi criado na etapa anterior. Aceite os valores pré-selecionados e crie a subescrição. 

**Utilizando o Shell**  
Para criar o IoT Core utilizando o shell, siga os seguintes comandos:  
```shell
gcloud iot registries create $REGISTRY \
    --region=$REGION \
    --event-notification-config=topic=$TOPIC_ID \
    --enable-mqtt-config --enable-http-config
 ```

### Setup IoT Core

**Utilizando os menus**  
Para criar um [IoT Core](https://cloud.google.com/blog/topics/developers-practitioners/what-cloud-iot-core), pesquise na caixa do GCP por "IoT Core" ou role o menu em sanduíche até encontrar o serviço. Sua tela provavelmente estará em branco, então aperte no botão "Criar um registro". No novo menu que aparecerá, em "ID do registro", coloque o nome definido anteriormente em **REGISTRY**, selecione a região definida em **REGION** (a dica é tentar deixar todos os seus serviços em uma única região ou regiões próximas. Para finalizar a configuração basta selecionar o tópico do menu Cloud Pub/Sub no qual deseja publicar as mensagens do dispositivo. Após criar o registro, va no menu "Dispositivos" e clique em "criar novo dispositivo". Adicione o nome desejado do seu dispositivo, que foi definido em **DEVICE_ID**. Abra as opções avançadas e role até autenticação. Coloque como método de entrada "Digitar manualmente" e para o formato selecione ES256. Logo abaixo, cole exatamente como copiou, incluíndo ---BEGIN PUBLIC KEY--- no começo e ---END PUBLIC KEY--- no final.

**Utilizando o Shell**  
Para criar o IoT Core utilizando o shell, siga os seguintes comandos:  
```shell
gcloud iot registries create $REGISTRY \
    --region=$REGION \
    --event-notification-config=topic=$TOPIC_ID \
    --enable-mqtt-config --enable-http-config
 ```
Para criar um dispositivo no nosso registro, utilize os comandos:  
```shell
gcloud iot devices create $DEVICE_ID \
    --region=$REGION \
    --registry=$REGISTRY \
    --public-key="path=./ec_public.pem,type=es256"
```

### Easy way - script de automação

Disponibilizamos um script shell para facilitar toda o setup da GCP para quem já entende da automação. 

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
   
    
