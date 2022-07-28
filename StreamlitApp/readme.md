# Smartbus App

This app runs on a container on Cloud Run. You need to setup your secrets as strealit guides [here](https://docs.streamlit.io/streamlit-cloud/get-started/deploy-an-app/connect-to-data-sources/secrets-management).  
Parse Your GCP Credentials there.... And please ***DO NOT SHARE THEM WITH ANYONE***.  

### About the App

The app uses pydeck, a ported implementation of [deck.gl](deck.gl) and a firestore callback [function](https://firebase.google.com/docs/firestore/query-data/listen)  
This is a real strech of Streamlit capabilities and has many performace issues, mainly with deck.gl experimental features that would not diplay correctly in some devices (maybe web.gl problem?).  
*Besides that*, the app shows a heatmap and a realtime location map, each on its own page and can work as inpiration for more projects. Demo Screenshots presented as below

<p align="middle">
    <img src="./images/Screen Shot 2022-07-28 at 5.17.17 PM.png" height="45%" width="45%" align="center"/>
    <img src="./images/Screen Shot 2022-07-28 at 5.17.25 PM.png" height="45%" width="45%" align="center"/>
</p>
<p align="middle">
    <img src="./images/Screen Shot 2022-07-28 at 5.17.44 PM.png" height="45%" width="45%" align="center"/>
    <img src="./images/Screen Shot 2022-07-28 at 5.17.56 PM.png" height="45%" width="45%" align="center"/>
</p>
