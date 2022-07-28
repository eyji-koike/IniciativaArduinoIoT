from datetime import datetime

import pandas as pd
import streamlit as st
from google.cloud import firestore
from google.oauth2 import service_account
import pydeck as pdk

# configure the page
st.set_page_config(page_title="Utilização",
                   page_icon="📊"
                   )
st.sidebar.header("Filtrar histórico")
st.write("# Histórico de utilização")
st.markdown("""
 Confira abaixo a utilização dos onibus para o período
""")


# get the credentials from the secret
def get_credentials():
    # create API Clients
    credentials = service_account.Credentials.from_service_account_info(
        st.secrets["gcp_service_account"]
    )
    st.session_state["Credentials"] = credentials


# make a datafram from a firestore collection
def make_df_from_firestore(collection):
    temp_dict = {}
    for doc in collection.stream():
        temp_dict[doc.id] = doc.to_dict()
    firestore_df = pd.DataFrame.from_dict(temp_dict, orient='index')
    firestore_df["DateTime"] = pd.to_datetime(firestore_df["Timestamp"], dayfirst=True).dt.date
    return firestore_df


# init our app
get_credentials()
firestore_access = firestore.Client(credentials=st.session_state["Credentials"], project="arduinoiotbackend")
collection_ref = firestore_access.collection("catracaData")
historic_df = make_df_from_firestore(collection_ref)


# map layers
ALL_LAYERS = {
    "Entradas": pdk.Layer('HeatmapLayer',
                          opacity=0.4,
                          data=historic_df[historic_df["Entrance"] == 1].drop(columns="DateTime"),
                          get_position=["Geo[1]", "Geo[0]"],
                          weightsTextureSize=256
                          ),
    "Saídas": pdk.Layer('HeatmapLayer',
                        opacity=0.4,
                        data=historic_df[historic_df["Entrance"] == 0].drop(columns="DateTime"),
                        get_position=["Geo[1]", "Geo[0]"],
                        color_range=[[255, 255, 204, [0.3]], [199, 233, 180, [0.3]], [127, 205, 187, [0.3]],
                                     [65, 182, 196, [0.3]], [44, 127, 184, [0.3]], [37, 52, 148, [0.3]]],
                        weightsTextureSize=256
                        )
}

selected_layers = [
    layer
    for layer_name, layer in ALL_LAYERS.items()
    if st.sidebar.checkbox(layer_name, True)
]
if selected_layers:
    st.pydeck_chart(pdk.Deck(map_provider="carto",
                             map_style='road',
                             initial_view_state=pdk.ViewState(latitude=-29.1668,
                                                              longitude=-51.1813,
                                                              zoom=10,
                                                              pitch=50,),
                             layers=[selected_layers]))
else:
    st.error("Please choose at least one layer.")
