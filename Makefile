CXXFLAGS=-std=c++11 -pedantic -Wall -Wsign-conversion -I.
CXXF=$(CXX) $(CXXFLAGS)
DESTDIR=
PREFIX=/usr/local

SNPRINTF_NAME=snprintf_2.2
SNPRINTF_URL=http://www.ijs.si/software/snprintf/$(SNPRINTF_NAME).tar.gz

all:

clean:
	rm -fr dist

install: all
	install -d $(DESTDIR)$(PREFIX)/include/calico
	install -m644 -t $(DESTDIR)$(PREFIX)/include/calico calico/*

uninstall:
	rm -fr $(DESTDIR)$(PREFIX)/include/calico

doc:
	mkdir -p dist/doc
	doxygen

doc-upload: doc dist/doc/.git/config
	cd dist/doc \
	  && git add -A \
	  && git commit --amend -q -m Autogenerated \
	  && git push -f origin master:gh-pages

dist/doc/.git/config:
	mkdir -p dist/doc
	@URL=`git remote -v | grep origin | awk '{ printf "%s", $$2; exit }'` \
	  && cd dist/doc \
	  && git init \
	  && git config user.name Bot \
	  && git config user.email "<>" \
	  && git commit -m _ --allow-empty \
	  && git remote add origin $$URL

.PHONY: clean doc test

check: \
    dist/tmp/test_cxx11.ok \
    dist/tmp/test_iterator.ok \
    dist/tmp/test_lens.ok \
    dist/tmp/test_string.ok \
    dist/tmp/test_utility.ok

dist/tmp/test_cxx11.ok: test/cxx11.cpp calico/cxx11.hpp
	mkdir -p dist/tmp
	$(CXX) $(CXXFLAGS) -Wno-sign-conversion -o /dev/null -c test/cxx11.cpp
	touch $@

dist/tmp/test_iterator.ok: test/iterator.cpp calico/iterator.hpp
	mkdir -p dist/tmp
	$(CXX) $(CXXFLAGS) -o dist/tmp/test_iterator test/iterator.cpp
	dist/tmp/test_iterator
	touch $@

dist/tmp/test_lens.ok: test/lens.cpp calico/lens.hpp
	mkdir -p dist/tmp
	$(CXX) $(CXXFLAGS) -o dist/tmp/test_lens test/lens.cpp
	dist/tmp/test_lens
	touch $@

# note: order of libsnprintf.a vs string.cpp matters!
dist/tmp/test_string.ok: test/string.cpp calico/string.hpp \
                         dist/tmp/libsnprintf.a
	mkdir -p dist/tmp
	$(CXX) $(CXXFLAGS) -o dist/tmp/test_string dist/tmp/libsnprintf.a \
	                      test/string.cpp
	dist/tmp/test_string
	touch $@

dist/tmp/test_utility.ok: test/utility.cpp calico/utility.hpp
	mkdir -p dist/tmp
	$(CXX) $(CXXFLAGS) -o dist/tmp/test_utility test/utility.cpp
	dist/tmp/test_utility
	touch $@

dist/tmp/libsnprintf.a:
	mkdir -p dist/tmp/snprintf
	cd dist/tmp/snprintf \
	  && ../../../tools/download $(SNPRINTF_URL) | tar -xz \
	  && cd $(SNPRINTF_NAME) \
	  && $(CC) -O2 -c snprintf.c \
	  && $(AR) cru ../../libsnprintf.a snprintf.o
	rm -r dist/tmp/snprintf
