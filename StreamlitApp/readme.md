# Smartbus App

This app runs on a container on Cloud Run. You need to setup your secrets as strealit guides [here](https://docs.streamlit.io/streamlit-cloud/get-started/deploy-an-app/connect-to-data-sources/secrets-management).  
Parse Your GCP Credentials there.... And please ***DO NOT SHARE THEM WITH ANYONE***.  

### About the APP

The app uses pydeck, a ported implementation of [deck.gl](deck.gl) and a firestore callback [function](https://firebase.google.com/docs/firestore/query-data/listen)  
This is a real strech of Streamlit capabilities and has many performace issues, mainly with deck.gl experimental features that would not diplay correctly in some devices.  
Besides that, the app shows a heatmap and a realtime location map, each on its own page and can work as inpiration for more projects.  
