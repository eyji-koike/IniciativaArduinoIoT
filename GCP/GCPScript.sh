export PROJECT_ID= #insira o nome do projeto aqui
export REGION= #insira a regiao aqui
export TOPIC_ID= #insisra a ID do topico aqui
export SUBSCRIPTION= #insisra a subscricao aqui
export REGISTRY= #insisra o nome do registro aqui
export DEVICE_ID= #insira a identificacao do dispositivo
export USER_NAME= #insisra o nome de usuario
export IMAGE_NAME= #insira o nome da imagem
export SERVICE_NAME= #insira o nome do servico

gcloud services enable cloudiot.googleapis.com pubsub.googleapis.com

gcloud projects add-iam-policy-binding $PROJECT_ID \
    --member=serviceAccount:cloud-iot@system.gserviceaccount.com \
    --role=roles/pubsub.publisher

gcloud pubsub topics create $TOPIC_ID

gcloud pubsub subscriptions create --topic $TOPIC_ID $SUBSCRIPTION

gcloud iot registries create $REGISTRY \
    --region=$REGION \
    --event-notification-config=topic=temperature-topic \
    --enable-mqtt-config --enable-http-config