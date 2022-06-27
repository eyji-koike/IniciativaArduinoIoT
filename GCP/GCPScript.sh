#!/bin/bash

#Script .sh pra rodar e automatizar o provisionamento da infraestrutura na nuvem
#Este codigo provisiona do IoT hub, que funciano como porta de entrada e metodo de autenticacao, 
#ate o servico Pub/Sub que le as mensagens recebidas e as publica num topico
#Atencao, o script precisa ser modificado de acordo com a necessidade

export PROJECT_ID= #insira o nome do projeto aqui
export REGION= #insira a regiao aqui
export TOPIC_ID= #insisra a ID do topico aqui
export SUBSCRIPTION= #insisra a subscricao aqui
export REGISTRY= #insisra o nome do registro aqui
export DEVICE_ID= #insira a identificacao do dispositivo

#Aqui inicia o processo
#Autoriza o Cloud Shell para fazer alteracoes
gcloud auth login

#O comando abaixo cria um novo projeto. Caso ainda nao tenha criado via console descomente a linha abaixo
#gcloud projects create $PROJECT_ID

#Aqui temos certeza de que estamos provisionando a infraestrutura no projeto correto
gcloud config set project $PROJECT_ID

#Ativar as APIs CloudIoT e PubSub
gcloud services enable cloudiot.googleapis.com pubsub.googleapis.com cloudfunctions.googleapis.com

#Adicionar conta de servico do cloud IoT para poder publicar no Pub/Sub
gcloud projects add-iam-policy-binding $PROJECT_ID \
    --member=serviceAccount:cloud-iot@system.gserviceaccount.com \
    --role=roles/pubsub.publisher

#Criar um topico no Pub/Sub
gcloud pubsub topics create $TOPIC_ID

#Criar uma subscricao com o nome definido no topico
gcloud pubsub subscriptions create --topic $TOPIC_ID $SUBSCRIPTION

#cria o registro na regiao escolhida e ativa mqtt e http
gcloud iot registries create $REGISTRY \
    --region=$REGION \
    --event-notification-config=topic=temperature-topic \
    --enable-mqtt-config --enable-http-config

#cria uma chave privada 256
openssl ecparam -genkey -name prime256v1 -noout -out ec_private.pem

#cria uma chave publica 256
openssl ec -in ec_private.pem -pubout -out ec_public.pem

#realiza o registro do dispositivo com a chave publica
gcloud iot devices create $DEVICE_ID \
    --region=$REGION \
    --registry=$REGISTRY \
    --public-key="path=./ec_public.pem,type=es256"

#fim