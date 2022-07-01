# Internet of the end of world - 4.0 Turnstile 

Implementation project of 4.0 workshops at [IFRS Campus Caxias do Sul](https://ifrs.edu.br/caxias/).


> *~Team Iniciativa Arduino IoT  
>  [Leia isso em Português](./readme.md)  
>  [Lea esto en Español]()* 

## Abstract  

This project uses an Arduino Uno r3 with: WiFi esp01 based on the esp8266 chip, GPS, and two buttons. The transmission protocol used is **[Mosquito (MQTT)](https://mosquitto.org/)** and our backend is made on **[Google Cloud Platform](https://cloud.google.com/)** using the services IoT Hub, and PUB/SUB. For our data retention, we use a google cloud function that is triggered whenever a new publish PUB/SUB event happens, and sends the received data to [Google Firebase](https://firebase.google.com/).

>***Disclaimer***  
*This project may incur charges and the authors are not responsible. Do it under your full awareness and read the available material carefully. Have a Good learning experience.*


