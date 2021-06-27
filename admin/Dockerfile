# syntax=docker/dockerfile:1

FROM python:3.9.5-slim
LABEL maintainer=“jeff@j3ff.io”

ARG wheel

WORKDIR /app

COPY requirements.txt .
RUN pip install --no-cache-dir -r requirements.txt

COPY dist/${wheel} .
RUN pip install --no-cache-dir ${wheel} && rm -rf ${wheel}

EXPOSE 5000
CMD ["deadeye-server"]
