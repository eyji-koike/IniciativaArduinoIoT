# syntax=docker/dockerfile:1

FROM python:3.10.5-bullseye

RUN pip install --upgrade pip

WORKDIR /app

COPY requirements.txt .

RUN apt-get update -y && apt-get install -y gcc

RUN pip install --no-cache-dir -r requirements.txt

COPY . .

ENV PORT=

CMD streamlit run Welcome.py --server.port=${PORT} --browser.serverAddress="0.0.0.0"