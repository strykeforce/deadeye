.PHONY: javadoc
javadoc:
	cd ../client && \
		./gradlew javadoc

.PHONY: book
book:
	@echo "====> building book"
	mdbook build

.PHONY: deploy
deploy: book javadoc
	@echo "====> deploying to github"
	$(eval TMPDIR := $(shell mktemp -d))
	git worktree add $(TMPDIR) gh-pages
	rm -rf $(TMPDIR)/*
	cp -rp book/* $(TMPDIR)
	cp -rp ../client/build/docs/javadoc $(TMPDIR)
	cd $(TMPDIR) && \
		git add --all && \
		git commit --message="deployed on $(shell date) by ${USER}" && \
		git push origin gh-pages
	git worktree remove $(TMPDIR)
	rm -rf $(TMPDIR)

# vim: set noexpandtab:
