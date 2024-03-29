# Internet of the end of world - 4.0 Turnstile

Implementation project of 4.0 workshops at [IFRS Campus Caxias do Sul](https://ifrs.edu.br/caxias/).

> *~Team Iniciativa Arduino IoT  
> [Leia isso em Português](./readme.md)  
> [Lea esto en Español]()*

## Abstract  

The objective of this project is to simulate an IoT Turnstile connected to the cloud. This would broaden fleet management capabilities as well as allowing for the development of machine learning and data analytics over the public transportating usage and scheduling.  
This project uses an Arduino Uno r3 paired with an esp01 WiFi board (based on the esp8266 chip), GPS, and two buttons. The transmission protocol used is **[Mosquito (MQTT)](https://mosquitto.org/)** and our backend is made on **[Google Cloud Platform](https://cloud.google.com/)** using the services [IoT Core](https://cloud.google.com/blog/topics/developers-practitioners/what-cloud-iot-core), and [PUB/SUB](https://cloud.google.com/pubsub). For our data retention, we use a [Google Cloud Functions](https://cloud.google.com/functions) that is triggered whenever a new publish PUB/SUB event happens, and sends the received data to [Google Firebase](https://firebase.google.com/).

>***Disclaimer***  
*This project may incur charges and the authors are not responsible. Do it under your full awareness and read the available material carefully to avoid suprises. Have a Good learning experience.*

**Overview**

The figure below represents our data flux, with the services used.  
![Information Flux](./Assets/InformationFluxogram.png)

**Materials List**

* Arduino Uno R3
* ESP01 8266
* GPS Neo 6M
* LCD 16x2
* Adapter for ESP01 w/ voltage regulator and logic level shifter
* Potentiometer 10kOhm
* Adapter usb-serial ch340 for esp01
* Jumper Cables
* 2 push buttons
* 5v power supply
* Switch (optional)
* Account on Google Cloud Platform
* [Node package manager](https://nodejs.org/en/)
* [Microsoft VSCode](https://code.visualstudio.com/)
* [Gcloud CLI](https://cloud.google.com/sdk/docs/install)

**Table of Contents**

1. [Configuring GCP](#configuring-gcp)

    1. [Setup IoT Core](#setup-iot-core)
    2. [Setup Cloud Pub/Sub](#setup-cloud-pubsub)
    3. [Easy way - bash script](#easy-way---bash-script)

2. [Configuring Arduino Uno r3 and ESP01](#configuring-arduino-uno-r3-and-esp01)

    1. [Installing arduino IDE libraries](#installing-libraries-in-arduino-ide)
    2. [Arduino Uno block-diagram](#block-diagram---arduino-uno)
    3. [ESP01 block-diagram](#block-diagram---esp01)
    4. [Wiring](#conecting-the-components)  
    5. [Final assembly](#final-assembly)

3. [Data, data, and more data](#data-data-and-more-data)

    1. [Review Cloud Pub/Sub](#checking-pubsub)
    2. [Setup Firebase](#setup-firebase)
    3. [Setup cloud functions - Typescript](#setup-cloud-functions---typescript)
    4. [Setup cloud functions - Python](#setup-cloud-functions---python)
    5. [Testing the cloud function](#checking-the-cloud-functions)

4. [Building the Dashboard](#construção-da-dashboard)

    1. [*find a suitable tool*](#decidir-ferramenta)

5. [Hybrid app integration](#integraçao-com-aplicativo-móvel)

    1. [Ionic](#desenvolvimento-ionic)

---

## Configuring GCP

Google offers many services in various ways. For more billing information on their free tier, click on [this link](https://cloud.google.com/free/docs/gcp-free-tier#free-tier).  
The configuration can be done utilizing the user interface, called console, or a command line interface as a free service called cloud shell. There is also a script at the end of this section, made for those who know how to execute it and make everything automaticaly.  
Firstly we need to configure names for our stuff. Copy and paste the variables below on a notepad and fill it with what you find suitable. If you are going to use cloud shell, copy the filled version onto it but don't delete your file, as we are using it troughtout all the development.

```shell
export PROJECT_ID=      #fill with your project name
export REGION=          #choose a region
export TOPIC_ID=        #name of your pubsub topic
export SUBSCRIPTION=    #name of your pubsub subscription
export REGISTRY=        #name of your iot core regitry
export DEVICE_ID=       #iot device id
```

With everything prepared we can go to the next step. Create a project utilizing your **PROJECT_ID** and be sure to check if billing is activated, otherwise you will be prevente from proceeding when deploying any kind of service. To do that go to google cloud console and search for "billing" in the search bar.

To activate the necessary services, go to the menu at your top-left and find "APIs". Click on "add services and APIs", and then search and activate the following **IoT core, Pub/Sub, and Cloud Functions**. If you are using shell, paste the following:

```shell
gcloud services enable cloudiot.googleapis.com pubsub.googleapis.com cloudfunctions.googleapis.com
```  

Before setting up the services, you need to create a service account to give IoT Core the permissions to publish into a Pub/Sub Topic. Search for "IAM & Admin" and when inside IAM, press "Add". On the first field, insert the following email *cloud-iot@system.gserviceaccount.com* and on the field "Function" search for **Cloud PUB/SUB Publish** and then hit save. If you are using shell, this will do it:

```shell
gcloud projects add-iam-policy-binding $PROJECT_ID \
    --member=serviceAccount:cloud-iot@system.gserviceaccount.com \
    --role=roles/pubsub.publisher
```  

To proceed, one will need an eliptic curve key pair with 256bits to authenticate the device. Now, it's easier to use cloud shell for that:

```shell
openssl ecparam -genkey -name prime256v1 -noout -out ec_private.pem
openssl ec -in ec_private.pem -pubout -out ec_public.pem
```

This will create two keys and save them in the cloud editor. Open the editor and find your keys. Open it, copy and paste into a temporary file for later. If you are using the shell you don't need to do this.

>***Important***  
>**Don't share your keys with anyone. This represents a security risk to the integrity of your project.**  

### Setup Cloud Pub/Sub  

**Utilizando os menus**  
To create a [PUB/SUB](https://cloud.google.com/pubsub) topic, go to the GCP searchbox look for "Pub/Sub". You can also scroll until you find the service on the left-hand menu. Tap the button "Create topic". In the new window, fill in the "Topic ID" box with whatever you defined as **TOPIC_ID** and hit create. It might take a short while but a message will say when your topic has been created. Now go to "Subscriptions" and press "Create Subscription". In the new opened window, type in the name that you chose as **SUBSCRITION** and select the topic that we just created. Accept all default values and press Create.
**Utilizando o Shell**  
If you want to use the google shell, paste the following:  

```shell
gcloud iot registries create $REGISTRY \
    --region=$REGION \
    --event-notification-config=topic=$TOPIC_ID \
    --enable-mqtt-config --enable-http-config
 ```

### Setup IoT Core

**Using the GUI**  
To create a [IoT Core](https://cloud.google.com/blog/topics/developers-practitioners/what-cloud-iot-core), use the searchbox to look for "IoT Core" or browse the menu until you find it. When you opend it, your screen will probably be blank. Press "create registry". On the new page, type the registry name into **REGISTRY**, select the **REGION** (hint: try to leave all of them close to each other). To end the configuration, select the Cloud Pub/Sub topic that you wish to publish the device messages. After creating the registry, go in *devices* and click on "Create new Device". Add the **DEVICE_ID** on the name field. Now, open the advanced options and roll until you find "Authentication". Select "type manually" and for the format select ES256. Below that, copy and paste everthing including the ---BEGIN PUBLIC KEY--- and ---END PUBLIC KEY--- at the end.

**Using Shell**  
To create an IoT Core using shell:  

```shell
gcloud iot registries create $REGISTRY \
    --region=$REGION \
    --event-notification-config=topic=$TOPIC_ID \
    --enable-mqtt-config --enable-http-config
 ```

To create a device:  

```shell
gcloud iot devices create $DEVICE_ID \
    --region=$REGION \
    --registry=$REGISTRY \
    --public-key="path=./ec_public.pem,type=es256"
```

### Easy way - bash script

We made a shell script to make it easy to deploy all of the above infrastructure. Don't forget to modify it to suit your needs. Download it [here](./GCP/GCPScript.sh), and open **cloud shell**. Now open editor and hit "upload file". To run it:

```shell
./GCPScript.sh
```

## Configuring Arduino Uno r3 and ESP01

In this system, the Arduino Uno bpoard is responsible for collecting telemetry data from the sensors and inputs. Whereas, the ESP01 will be responsible for sending the telemetry throught MQTT, connecting to the Wi-Fi Access Point and to GCP.  
There are two approaches to work with all the sensors and inputs. We can build our own libraries to make them work or utilize the ones available in the Arduino community. In this project we opt for the latter, as they can certainly do a solid job and reduce our workload.

### Installing Libraries in Arduino IDE

To program the ESP01 we need to make the IDE recognize it. On the field "manage additional boards", input the following link:

```
https://arduino.esp8266.com/stable/package_esp8266com_index.json
```

Hit ok and go to tools > board > Boards Manager. In the new window, search for esp8266 community. You will need to do this every time to flash the esp8266. 

![ESP01Menu](./Assets/ESP01Menu.png)  

To add the libraries go to menu > add libraries > manage libraries. Install the following:

1. LiquidCrystal by Arduino
2. WifiESP by Bruno Portaluri
3. TinyGPS by Mikal Hart
4. SerialTransfer by PowerBroker2
5. MQTT by Joel Gaewhiler
6. Google Cloud IoT Core JWT by Guss Class
7. DailyStruggleButton by cygig

During the installation, a pop-up message may ask to install dependent libraries, allow it.

### Block Diagram - Arduino UNO

In this project the arduino Uno works agreggating sensorial information. If a button was pressed, the Uno identifies which one was pressed and insert the information into a *struct* that has the already polled GPS information, and sends it to the ESP01 throught Software Serial. The block Diagram will likely be the following:
<p align="middle">
<img src="./Assets/FluxogramaENUS.svg" height="75%" width="75%" align="center"/>
</p>
The exploded version can be found [here](/Assets/MainLoopExplodedPTBr.svg). THe code can be found [here](/BoardPrograms/Uno/).

### Block Diagram - ESP01

The ESP01 program is a little simpler. When the Uno sends a message, the ESP01 creates a .JSON and send it to PUB/SUB. The message format is:

```json
 { 
    "latitude": 0.0000,
    "longitude": 0.0000,
    "HDOP": 0.0000,
    "Altitude": 0.0000, 
    "Course": 0.0000,
    "Speed": 0.0000, 
    "NumSat": 0, 
    "Entrance": 0,
    "Exit": 0,
 }
```

The block diagram can be found below and the code can be found [here](./BoardPrograms/Esp8266-lwmqtt/).
<p align="middle">
<img src="./Assets/ESP01ENUS.svg" height="45%" width="45%" align="center"/>
</p>
**Remainder**, the ESP01 must be in Flash mode. To achieve this, the GPIO00 pin must be connected to GND when the board is booted up. We suggest the addtion of a switch between the GND and GPIO00 pins to make it easier to upload to the ESP01, as represented on the figure below to the left. Another tool used in this project is the usb ch340 adaptor to esp01, shown below to the left.  
<p align="middle">
<img src="./Assets/Esp01FlashMode.png" height="30%" width="30%" align="center"/>
<img src ="./Assets/ESP-8266-ESP-01-Adaptador-USB.jpg" height="35%" width="35%" align="center"/>
</p>

### Conecting the components

Almost all the Arduino pins are used in this project. The wiring will look like the following:  
<p align="middle">
<img src="./Assets/GY-NEO6MV2_bb.png" height="75%" width="75%"  align="center"/>
</p>
Please pay attention that on the eletrical drawing we used a voltage-level-shifter because esp01 works with 3.3v. This can be simplified with the adaptor shown below:  
<p align="middle">
<img src="./Assets/sku_404644_1.jpg" height="35%" width="35%" align="center"/>
</p>

### Final assembly

At the end your project will look like the following:
<p align="middle">
<img src="./Assets/Vis%C3%A3oSuperior.jpeg" height="51%" width="51%" align="center"/>
<img src="./Assets/Vis%C3%A3oFrontal.jpeg" height="38%" width="38%" align="center"/>
</p>

## Data, data and more data

With the hardware up and running, now we can worry about the backend. So far, we only deployed the infrastructure. There are many ways we can test the IoT Core, but this requires the installation of Eclipse Mosquitto Client and it's out of the scope of this project. For more information [this project](http://nilhcem.com/iot/cloud-iot-core-with-the-esp32-and-arduino) by @nilhcem, on "connect to http/mqtt bridge" can help you.

### Checking Pub/Sub

Turn everything on. If everything is correctly setup and coded, when you press a button the Uno will display the count, and the ESP01's blue light will blink upon received serial data. To verify Pub/Sub, open the Google Cloud Console and navigate to the Pub/Sub service. Click on the topic ID and search for the **$SUBSCRIPTION**. Click on it and, to display the messages select "Enable ack messages" and then hit the **pull** button. Your screen will look like this:

<p align="middle">
<img src="./Assets/PubSubTopicMessages.png" align="center"/>
</p>

To do the same, but use the cli [Google Cloud CLI](https://cloud.google.com/sdk/docs/install-sdk) or the shell:  

```shell
gcloud pubsub subscriptions pull --auto-ack $SUBSCRIPTION --limit=1
```

### Setup Firebase

If you arrived here, congrats!! Now we can send information anywhere. But, for this project we are going to send it to Firebase as we aim a simpler integration with an App on the future. More info about [Firebase](https://firebase.google.com/). Login into it with the same GCP account. Add a new project and select the name as you created earlier on **PROJECT_ID**. Confirm the we are going to use **blaze**, and confirm  again. When it asks about Google Analytics, confirm that it is activated. For the Google Analytics, select the standard, hit confirm, and hit create Firebase.  
Now we need to create a Firestore database e enable cloud functions. On the left hand menu, select Firestore Database and hit create database. Select the test mode, click next, chose the region closest to your previosly defined **REGION**.

### Setup cloud functions - Typescript

To build your cloud function we need [node package manager](https://nodejs.org/en/), microsoft [VSCode](https://code.visualstudio.com/) and [gcloud CLI](https://cloud.google.com/sdk/docs/install). Install all of them and then in terminal use the following command:

```shell
npm install -g firebase-tools
```

Now create a new directory and init the firebase funcations:

```shell
mkdir myProject
cd myProject
firebase init functions
```
>by default Firebase only allows for js functions... we can use python through GCP Cloud functions

Use the arrow to choose "Use an existing project", then select the Google Cloud project name, and finally the TypeScript language. When it asks about ESLint type "n". When it asks "Do you want to install dependencies?" type "y" and use the following command to open it on vsCode:

```
. code
```

Go to the **src** folder and open the file **index.ts**. Now you can copy and paste [from this link](./GCP/TSCloudFunction/index.ts) or download and replace the file. Feel free to modify it to ensure the best fit for you.
Now use the following commands:

```shell
gcloud auth login 
# Web browser login

gcloud projects list 
# show available projects

gcloud config set project $PROJECT_ID 
# set the default working project
```

 Now utilize the following command to deploy the function:

```shell
firebase deploy --only functions
```

### Setup cloud functions - Python

Our function can also be written in python. To start, find that you have *Cloud Functions* in your terminal. To do so, paste the following:

```shell
mkdir $PROJECT_ID
# project name

cd $PROJECT_ID
# go into that folder

. code
# open vscode there

```

A Cloud Function needs two files. Create a *main.py* and a *requirements.txt*. Type the follwing in the requirements:

```
google-cloud-firestore>=2.1.3
```

Now we can edit *main.py*. Copy and paste, or replace the file from this [link](./GCP/PythonCloudFunction/main.py). Modufy it as you please. Then deploy using:

```shell
gcloud functions deploy NOME_DA_FUNCAO --runtime python39 --trigger-topic $TOPIC_ID
```

### Checking the Cloud functions  

To verify that firabse is receiving our communications, please do the following:

```shell
gcloud pubsub topics publish $TOPIC_ID --message='Catracat02-connected' --attribute='deviceId=Catraca02,subFolder=events'

# or

gcloud pubsub topics publish $TOPIC_ID --message='{"Latitude":-29.19,"Longitude":-51.24,"HDOP":159,"Altitude":724.30,"Course":0.00,"Speed":0.00,"NumSat":6,"Date":2012479491,"Time":131006486,"fixAge":0,"Entrance":1,"Exit":0}' --attribute='deviceId=Catraca02'
```

Errors and events can be found at heatlh and logs.

## Streamlit Web App  

Here you can find a [Streamlit dashboard](./StreamlitApp/readme.md).


## Mobile App

