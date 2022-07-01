/*********************************************************************
 *  Copyright 2022 IniciativaArduinoIoT                               *
 *  Licensed under the Apache License, Version 2.0 (the "License");   *
 *  you may not use this file except in compliance with the License.  *
 *  You may obtain a copy of the License at                           *
 *                                                                    *
 *  http://www.apache.org/licenses/LICENSE-2.0                        *
 *                                                                    *
 *  Unless required by applicable law or agreed to in writing,        *
 *  software distributed under the License is distributed on an       *
 *  "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND,      *
 *  either express or implied. See the License for the specific       *
 *  language governing permissions and limitations under the License. *
 **********************************************************************/
// This file contains the functions used in Google Cloud Functions API service.
// It's a serverless applicaation that publishes our telemetry to a collection
// on firestore. If it is a connection information, the message will go to a stats
// collection. Efforts will be made to write this function in python too.

// here we import everything from the firebase cli 
import * as functions from "firebase-functions";

// we set our function as admin, we will not get all fancy with IAM but you can do it
const admin = require('firebase-admin');
admin.initializeApp();

// here is where the magic happens. insertFromPubSub will be our function name.
// we have to get the topic id and utilize the method onPublish as our trigger.
exports.insertFromPubSub = functions.pubsub.topic('ColetasCatraca').onPublish((message, context) => {
    //timestamp when our function was triggered
    console.log('The function was triggered at ', context.timestamp);
    // this is for non json data on the body
    // const messsageBody = message.data ? Buffer.from(message.data, 'base64').toString() : null;
    // console.log('Full message: ', messsageBody);
    // json reference
    // we need to stantiate our object.
    // pay attention that we device id to our JSON
    var catracaInfo = {
        "Latitude": 0.00, 
        "Longitude": 0.00, 
        "HDOP": 0, 
        "Altitude": 0.00, 
        "Course": 0.00, 
        "Speed": 0.00, 
        "NumSat": 0, 
        "Time": 0, 
        "Entrance": 0, 
        "Exit": 0,
        "DeviceID": '',
        "Timestamp": ''
    };
    // initialize the flag to tell us if the message was json data or just a connection
    let isMessage: boolean = false;
    // try to get the json info 
    try {
        catracaInfo = {
            "Latitude": message.json.Latitude,
            "Longitude": message.json.Longitude,
            "HDOP": message.json.HDOP,
            "Altitude": message.json.Altitude,
            "Course": message.json.Course,
            "Speed": message.json.Speed,
            "NumSat": message.json.NumSat,
            "Time": message.json.Time,
            "Entrance": message.json.Entrance,
            "Exit": message.json.Exit,
            "DeviceID": message.attributes.deviceId,
            "Timestamp": context.timestamp
        };
        // so it looks like it was indeed a json
        console.log('The message is in json format.');
        // signalize it
        isMessage = true;
    } catch (e) {
        // damn, it was just a connection message
        console.log('Invalid JSON.');
    };
    // we need to instantiate ou stats object in case it wasn't a json
    var stats = {
        'device': '',
        'type': '',
        'status': "connected",
        'timestamp': ''
    };
    // look at the flag, was it json?
    if (isMessage == true) {
        // yes, it was. Let's send it to our collection on firestore!!!!
        return admin.firestore().collection('catracaData').add(catracaInfo);
    } else {
        // oh my, it wasn't. let's try other things
        try {
            // here we define some variables to play with
            let messageSubfolder = '';
            let deviceID = '';
            // now we can try to get the subfolder information.
            // if it exists, it means it was a connection message
            try {
                // try yo grab some stuff
                messageSubfolder = message.attributes.subFolder;
                deviceID = message.attributes.deviceId;
                // we got it, send it to the console
                console.log(deviceID, " connected");
                // create our stats JSON
                stats = {
                    'device': deviceID,
                    'type': messageSubfolder,
                    'status': "connected",
                    'timestamp': context.timestamp

                };
            } catch (e) {
                // ok, it wasn't a connection. This shouldn't happen so search for malformed stuff
                console.log("Message wasn't a connection either. Please see the error presented on the log file.");
            };
            // everything went ok, let publish our stats
            return admin.firestore().collection('stats').add(stats);
        } catch (e) {
            // damn, we couldn't publish anything... look for malformed stuff
            // send the error log to the logger on our project
            functions.logger.error("Couldn't publish anything. Error message: ", e);
        }//end try catch
    }//end else

});//end our main body
