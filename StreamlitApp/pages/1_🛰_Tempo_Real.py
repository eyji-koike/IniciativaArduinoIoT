import pandas as pd
import pydeck as pdk
import streamlit as st
import time
from queue import Queue
from google.cloud import firestore
from google.oauth2 import service_account



def get_credentials():
    # create API Clients
    credentials = service_account.Credentials.from_service_account_info(
        st.secrets["gcp_service_account"]
    )
    st.session_state["Credentials"] = credentials


def on_snapshot(doc_snapshot, changes, read_time):
    for doc in doc_snapshot:
        print(u'Received document snapshot: {}'.format(doc.id))
        q.put(doc)



def make_df_from_firestore(collection):
    temp_dict = {}
    for doc in collection.stream():
        temp_dict[doc.id] = doc.to_dict()
    firestore_df = pd.DataFrame.from_dict(temp_dict, orient='index')
    return firestore_df


def insert_map_icon(df):
    icon_url = "data:image/svg+xml;utf8;base64,PHN2ZyB4bWxucz0iaHR0cDovL3d3dy53My5vcmcvMjAwMC9zdmciIGRhdGEtbmFtZT0iTGF5ZXIgMSIgdmlld0JveD0iMCAwIDUxMiA1MTIiPjxwYXRoIGZpbGw9IiM3ODc2ODEiIGQ9Ik00ODkuNSwxNTNhNy40OTk3NCw3LjQ5OTc0LDAsMCwxLTcuNS03LjVWMTIzSDQ0MGE3LjUsNy41LDAsMCwxLDAtMTVoNDIuNUExNC41MTY2OSwxNC41MTY2OSwwLDAsMSw0OTcsMTIyLjV2MjNBNy40OTk3NCw3LjQ5OTc0LDAsMCwxLDQ4OS41LDE1M1ptLTctMzBoMFoiLz48cmVjdCB3aWR0aD0iMzAiIGhlaWdodD0iNzQiIHg9IjQ3NC41IiB5PSIxNDUuNSIgZmlsbD0iI2JjZDQ0YyIgcng9IjYiLz48cGF0aCBmaWxsPSIjYWVjZTQ0IiBkPSJNNDk4LjUsMTQ1LjVoLTE1YTYsNiwwLDAsMSw2LDZ2NjJhNiw2LDAsMCwxLTYsNmgxNWE2LDYsMCwwLDAsNi02di02MkE2LDYsMCwwLDAsNDk4LjUsMTQ1LjVaIi8+PHBhdGggZmlsbD0iIzc4NzY4MSIgZD0iTTIyLjUsMTUzYTcuNDk5NzQsNy40OTk3NCwwLDAsMS03LjUtNy41di0yM0ExNC41MTY2OSwxNC41MTY2OSwwLDAsMSwyOS41LDEwOEg3MmE3LjUsNy41LDAsMCwxLDAsMTVIMzB2MjIuNUE3LjQ5OTc0LDcuNDk5NzQsMCwwLDEsMjIuNSwxNTNaIi8+PHJlY3Qgd2lkdGg9IjMwIiBoZWlnaHQ9Ijc0IiB4PSI3LjUiIHk9IjE0NS41IiBmaWxsPSIjYmNkNDRjIiByeD0iNiIgdHJhbnNmb3JtPSJyb3RhdGUoMTgwIDIyLjUgMTgyLjUpIi8+PHBhdGggZmlsbD0iI2FlY2U0NCIgZD0iTTMxLjUsMTQ1LjVoLTE1YTYsNiwwLDAsMSw2LDZ2NjJhNiw2LDAsMCwxLTYsNmgxNWE2LDYsMCwwLDAsNi02di02MkE2LDYsMCwwLDAsMzEuNSwxNDUuNVoiLz48cmVjdCB3aWR0aD0iMzY4IiBoZWlnaHQ9IjQyOSIgeD0iNzIiIHk9IjcuNSIgZmlsbD0iI2JjZDQ0YyIgcng9IjMwIi8+PHBhdGggZmlsbD0iI2FlY2U0NCIgZD0iTTQxMCw3LjVIMzY1YTMwLDMwLDAsMCwxLDMwLDMwdjM2OWEzMCwzMCwwLDAsMS0zMCwzMGg0NWEzMCwzMCwwLDAsMCwzMC0zMFYzNy41QTMwLDMwLDAsMCwwLDQxMCw3LjVaIi8+PHJlY3Qgd2lkdGg9IjE3NSIgaGVpZ2h0PSIzNjgiIHg9IjE2OC41IiB5PSItMTEiIGZpbGw9IiM4M2M5ZWUiIHRyYW5zZm9ybT0icm90YXRlKDkwIDI1NiAxNzMpIi8+PHJlY3Qgd2lkdGg9IjQ1IiBoZWlnaHQ9IjE3NSIgeD0iMzk1IiB5PSI4NS41IiBmaWxsPSIjNTJiYmU5Ii8+PHBhdGggZmlsbD0iI2E4ZDlmNSIgZD0iTTI2My41LDg1LjV2MTc1YTcuNSw3LjUsMCwwLDEtMTUsMFY4NS41WiIvPjxwYXRoIGZpbGw9IiNhZWNlNDQiIGQ9Ik00MTAsMzIwLjVIMTAyYTMwLDMwLDAsMCwxLTMwLTMwdi0zMEg0NDB2MzBBMzAsMzAsMCwwLDEsNDEwLDMyMC41WiIvPjxjaXJjbGUgY3g9IjM4MiIgY3k9IjM3OC41IiByPSIyOCIgZmlsbD0iI2ZkZGQ1YSIvPjxwYXRoIGZpbGw9IiNmZWQwMzAiIGQ9Ik0zODIsMzUwLjVhMjguMDA1MzQsMjguMDA1MzQsMCwwLDAtNy41LDEuMDI0MTcsMjcuOTk4NjcsMjcuOTk4NjcsMCwwLDEsMCw1My45NTE2NkEyNy45OTk1NiwyNy45OTk1NiwwLDEsMCwzODIsMzUwLjVaIi8+PGNpcmNsZSBjeD0iMTMwIiBjeT0iMzc4LjUiIHI9IjI4IiBmaWxsPSIjZmRkZDVhIi8+PHBhdGggZmlsbD0iI2ZlZDAzMCIgZD0iTTEzMCwzNTAuNWEyOC4wMDUzNCwyOC4wMDUzNCwwLDAsMC03LjUsMS4wMjQxNywyNy45OTg2NywyNy45OTg2NywwLDAsMSwwLDUzLjk1MTY2QTI3Ljk5OTU2LDI3Ljk5OTU2LDAsMSwwLDEzMCwzNTAuNVoiLz48cGF0aCBmaWxsPSIjNzg3NjgxIiBkPSJNMTAzLDQzNi41aDY2YTAsMCwwLDAsMSwwLDB2NTNhMTUsMTUsMCwwLDEtMTUsMTVIMTE4YTE1LDE1LDAsMCwxLTE1LTE1di01M0EwLDAsMCwwLDEsMTAzLDQzNi41WiIvPjxwYXRoIGZpbGw9IiM1NzU2NWMiIGQ9Ik0xMzksNDM2LjV2NTNhMTUuMDAwMDgsMTUuMDAwMDgsMCwwLDEtMTUsMTVoMzBhMTUuMDAwMDgsMTUuMDAwMDgsMCwwLDAsMTUtMTV2LTUzWiIvPjxwYXRoIGZpbGw9IiM3ODc2ODEiIGQ9Ik0zNDMsNDM2LjVoNjZhMCwwLDAsMCwxLDAsMHY1M2ExNSwxNSwwLDAsMS0xNSwxNUgzNThhMTUsMTUsMCwwLDEtMTUtMTV2LTUzQTAsMCwwLDAsMSwzNDMsNDM2LjVaIi8+PHBhdGggZmlsbD0iIzU3NTY1YyIgZD0iTTM3OSw0MzYuNXY1M2ExNS4wMDAwOCwxNS4wMDAwOCwwLDAsMS0xNSwxNWgzMGExNS4wMDAwOCwxNS4wMDAwOCwwLDAsMCwxNS0xNXYtNTNaIi8+PHBhdGggZmlsbD0iIzc4NzY4MSIgZD0iTTUwNC41LDUxMkg3LjVhNy41LDcuNSwwLDAsMSwwLTE1aDQ5N2E3LjUsNy41LDAsMCwxLDAsMTVaIi8+PHBhdGggZmlsbD0iI2ZmZiIgZD0iTTM1NCw1NEgxNThhNy41LDcuNSwwLDAsMSwwLTE1SDM1NGE3LjUsNy41LDAsMCwxLDAsMTVaIi8+PHJlY3Qgd2lkdGg9IjEzNiIgaGVpZ2h0PSI1NiIgeD0iMTg4IiB5PSIzNTAuNSIgZmlsbD0iI2ZmZiIgcng9IjE1Ii8+PHBhdGggZmlsbD0iI2UxZTFlMyIgZD0iTTMwOSwzNTAuNUgyNjRhMTUsMTUsMCwwLDEsMTUsMTV2MjZhMTUuMDAwMDgsMTUuMDAwMDgsMCwwLDEtMTUsMTVoNDVhMTUuMDAwMDgsMTUuMDAwMDgsMCwwLDAsMTUtMTV2LTI2QTE1LDE1LDAsMCwwLDMwOSwzNTAuNVoiLz48Y2lyY2xlIGN4PSIzOTUiIGN5PSI0Ni41IiByPSI3LjUiIGZpbGw9IiNmZGRkNWEiLz48Y2lyY2xlIGN4PSIxMTYuNDczIiBjeT0iNDYuNSIgcj0iNy41IiBmaWxsPSIjZmRkZDVhIi8+PC9zdmc+"
    icon_data = {
        "url": icon_url,
        "width": 242,
        "height": 242,
        "anchorY": 242,
    }
    df['icon_data'] = None
    for index in df.index:
        df['icon_data'][index] = icon_data
    return df


st.session_state['ProjectID'] = 'arduinoiotbackend'
get_credentials()
print('Initializing Firestore connection...')
# Credentials and Firebase App initialization. Always required


# Get access to Firestore
firestoredb = firestore.Client(credentials=st.session_state["Credentials"], project="arduinoiotbackend")
collection_ref = firestoredb.collection("lastLoc")
print('Connection initialized')

q = Queue()
snap = st.empty()
snap2 = st.empty()
col_watch = collection_ref.on_snapshot(on_snapshot)
doc_dict = {}
st.session_state["lastLocDF"] = insert_map_icon(make_df_from_firestore(collection_ref))


# live layer
live_layer = pdk.Layer(type="IconLayer",
                       opacity=0.9,
                       data=st.session_state["lastLocDF"],
                       get_icon='icon_data',
                       get_position=["Geo[1]", "Geo[0]"],
                       get_size=5,
                       size_scale=15,
                       pickable=True)
# base map
base_map_live = pdk.Deck(
    map_provider="carto",
    map_style='road',
    initial_view_state=pdk.ViewState(
        latitude=float(-29.1668),
        longitude=float(-51.1813),
        zoom=10,
        pitch=50,
    ),
    layers=[live_layer],
    tooltip={"text": "{tags}"}
)

snap2.pydeck_chart(base_map_live)

# Keep the app running
while True:
    time.sleep(1)
    print('processing...')

    if not q.empty():
        doc_dict = {}
        for i in range(q.qsize()):
            retrieved_doc = q.get()
            doc_dict['{}'.format(retrieved_doc.id)] = retrieved_doc.to_dict()
        st.session_state['lastLocDF'] = insert_map_icon(pd.DataFrame.from_dict(doc_dict, orient='index'))
        live_layer.data = st.session_state['lastLocDF']
        base_map_live.update()
        snap2.pydeck_chart(base_map_live)

    snap.write(st.session_state["lastLocDF"])
