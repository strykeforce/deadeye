IMAGE   := deadeye-cpp-base
NS      ?= j3ff
VERSION ?= $(shell git rev-parse --short HEAD)

PLATFORM := $(shell uname -m)

.PHONY: build
build: .built

.built: Dockerfile.$(PLATFORM)
	docker build -t $(NS)/$(IMAGE):latest -t $(NS)/$(IMAGE):$(VERSION) -f Dockerfile.$(PLATFORM) .
	docker inspect -f '{{.Id}}' $(NS)/$(IMAGE):latest > $@

.PHONY: push
push: build
	docker push $(NS)/$(IMAGE_NAME):latest
	docker push $(NS)/$(IMAGE_NAME):$(VERSION)

.PHONY: clean
clean:
	rm -rf .built*
