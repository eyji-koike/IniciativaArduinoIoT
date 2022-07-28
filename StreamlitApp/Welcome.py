import streamlit as st

st.set_page_config(
    page_title="Boas-vindas",
    page_icon="🚌",
    initial_sidebar_state="collapsed"
)

st.write("# Bem vindo ao app Smart Bus")

st.markdown(
    """
    Smart Bus App foi desenvolvido como parte do projeto Internet do Fim do Mundo: Catraca 4.0 🚍
    **👈 Selecione ao lado a vizualização** que desejar
    
    ### Quer saber mais?
    
    - Acesse o [repositório do github](https://github.com/eyji-koike/IniciativaArduinoIoT)
    """
)