
NS := j3ff
VERSION := $(shell git rev-parse --short HEAD) 
IMAGE_NAME := deadeye-cpp-base
BUILD_LOG := /opt/build.log

.PHONY: build

build:
	docker build $(BUILD_ARGS) -t $(NS)/$(IMAGE_NAME):$(VERSION) -f Dockerfile .
logs:
	docker run --rm $(NS)/$(IMAGE_NAME):$(VERSION) cat $(BUILD_LOG)
