IMAGE   := deadeye-shutdownd
NS      ?= j3ff
VERSION ?= $(shell git rev-parse --short HEAD)

.PHONY: build
build: .built

.built: Dockerfile $(SRC)
	docker build -t $(NS)/$(IMAGE):latest -t $(NS)/$(IMAGE):$(VERSION) -f Dockerfile .
	docker cp $(NS)/$(IMAGE):latest:/build/shutdownd ./build/
	docker inspect -f '{{.Id}}' $(NS)/$(IMAGE):latest > $@

.PHONY: push
push: build
	docker push $(NS)/$(IMAGE):latest
	docker push $(NS)/$(IMAGE):$(VERSION)

.PHONY: copy
copy:
	export ID=$(shell docker create ${NS}/${IMAGE}:latest); \
	docker cp $$ID:build/shutdownd ./; \
	docker rm -v $$ID

.PHONY: clean
clean:
	rm -rf .built* shutdownd
