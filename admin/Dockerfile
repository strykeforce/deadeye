# syntax=docker/dockerfile:1

FROM python:3.8-buster
LABEL maintainer=“jeff@j3ff.io”

WORKDIR /app

COPY ./requirements.txt /app/requirements.txt

RUN pip install --no-cache-dir -r requirements.txt

COPY . /app

EXPOSE 5000
ENTRYPOINT [ "python" ]
CMD [ "/app/deadeye/scripts/server.py" ]


