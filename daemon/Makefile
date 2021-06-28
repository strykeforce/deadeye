NS ?= j3ff
VERSION ?= $(shell git rev-parse --short HEAD)
IMAGE_NAME := deadeye-daemon
DEADEYE_NT_SERVER := 192.168.1.11

.PHONY: build push run

build:
	docker build $(EXTRA_BUILD_ARGS) -t $(NS)/$(IMAGE_NAME):latest -t $(NS)/$(IMAGE_NAME):$(VERSION) -f Dockerfile .

push: build
	docker push $(NS)/$(IMAGE_NAME):latest
	docker push $(NS)/$(IMAGE_NAME):$(VERSION)

run:
	docker run -it --rm --name $(IMAGE_NAME) \
		--env DEADEYE_NT_SERVER=$(DEADEYE_NT_SERVER) \
		--env DEADEYE_DOCKER=1 \
		$(NS)/$(IMAGE_NAME)
