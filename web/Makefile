NS      ?= j3ff
VERSION ?= $(shell git rev-parse --short HEAD)
IMAGE   := deadeye-web

.PHONY: build
build: .built

.built: Dockerfile package-lock.json nginx/nginx.conf
	docker build -t $(NS)/$(IMAGE):latest -t $(NS)/$(IMAGE):$(VERSION) .
	docker inspect -f '{{.Id}}' $(NS)/$(IMAGE):latest > $@

.PHONY: push
push: build
	docker push $(NS)/$(IMAGE):latest
	docker push $(NS)/$(IMAGE):$(VERSION)

.PHONY: clean
clean:
	rm -rf node_modules *.log* .built*
