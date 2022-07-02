'''/*********************************************************************
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
 **********************************************************************/'''
# here we do the imports
import base64
import json
import re
import os
from firebase_admin import credentials
from firebase_admin import firestore

from google.cloud import firestore

# now, stantiate what project we are working on
client = firestore.Client(project='arduinoiotbackend')


# here we define the name of our function. This has to be the same when we deploy the function throught command line
def pubsub_fire(event, context):
    # display on the console when the funtion was triggered
    print(
        f'This function was triggered by messageId {context.event_id}, published at {context.timestamp} to {context.resource["name"]}!')
    # if there is data in the event, we decode it. Otherwise, we display hello world
    if 'data' in event:
        name = base64.b64decode(event['data']).decode('utf-8')
    else:
        name = 'World'
    # here we display that message in the console
    print(f'Hello {name}!')
    # here we search for the keyword -connected in 'name' to see if the message was a telemetry or just a connection
    if re.search("-connected", name) is not None:
        # if it was a connection, we route to our 'stats' collection
        doc = client.collection('stats').document()
        # and we get the attributes to post as a json document
        data = event['attributes']
        # here we set how our json document will look like
        doc.set({
            'device': data['deviceId'],
            'type': data['subFolder'],
            'status': "connected",
            'timestamp': context.timestamp
        })
    else:
        # if it wasn't a connection, we have to be able to read it as a JSON
        data = json.loads(name)
        # and we have to route it to the propper collection
        doc = client.collection('catracaData').document()
        # then we set how our document will loook like
        doc.set(
            {
                "Latitude": data["Latitude"],
                "Longitude": data["Longitude"],
                "HDOP": data["HDOP"],
                "Altitude": data["Altitude"],
                "Course": data["Course"],
                "Speed": data["Speed"],
                "NumSat": data["NumSat"],
                "Entrance": data["Entrance"],
                "Exit": data["Exit"],
                "DeviceID": context.resource["name"],
                "Timestamp": context.timestamp
            }
        )
