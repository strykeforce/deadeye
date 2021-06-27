# syntax=docker/dockerfile:1

FROM python:3.9.5-slim
LABEL maintainer=“jeff@j3ff.io”

WORKDIR /app

COPY requirements.txt .
RUN pip install --no-cache-dir -r requirements.txt

COPY dist/*.whl .
RUN pip install --no-cache-dir *.whl && rm -rf *.whl

EXPOSE 5000
CMD ["deadeye-server"]