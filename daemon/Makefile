IMAGE   := deadeye-daemon
NS      ?= j3ff
VERSION ?= $(shell git rev-parse --short HEAD)

DEADEYE_NT_SERVER := 192.168.1.11
PLATFORM := $(shell uname -m)

include .depend

.PHONY: build
build: .built

.built: Dockerfile $(SRC)
	docker build -t $(NS)/$(IMAGE):latest -t $(NS)/$(IMAGE):$(VERSION) -f Dockerfile .
	docker inspect -f '{{.Id}}' $(NS)/$(IMAGE):latest > $@

.PHONY: depend
depend: .depend

.depend:
	echo -n  " SRC = " > $@
	find src -type f -printf "%p \\\\\n" >> $@
	echo "" >> $@

.PHONY: push
push: build
	docker push $(NS)/$(IMAGE):latest
	docker push $(NS)/$(IMAGE):$(VERSION)

.PHONY: run
run:
	docker run -it --rm --name $(IMAGE) \
		--env DEADEYE_NT_SERVER=$(DEADEYE_NT_SERVER) \
		--env DEADEYE_DOCKER=1 \
		$(NS)/$(IMAGE)

.PHONY: clean
clean:
	rm -rf .built*
