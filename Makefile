CXXFLAGS=-std=c++11 -pedantic -Wall -Wsign-conversion -I.
CXXF=$(CXX) $(CXXFLAGS)
DESTDIR=/usr

REMOTE=git@github.com:Rufflewind/calico.git

SNPRINTF_NAME=snprintf_2.2
SNPRINTF_URL=http://www.ijs.si/software/snprintf/$(SNPRINTF_NAME).tar.gz

all:

clean:
	rm -fr dist

install: all
	install -d $(DESTDIR)/include/calico
	install -m644 -t $(DESTDIR)/include/calico calico/*

uninstall:
	rm -fr $(DESTDIR)/include/calico

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
	cd dist/doc \
	  && git init \
	  && git config user.name Bot \
	  && git config user.email "<>" \
	  && git commit -m _ --allow-empty \
	  && git remote add origin "$(REMOTE)"

.PHONY: clean doc test

check: \
    dist/tmp/libsnprintf.a \
    dist/tmp/test_cxx11 \
    dist/tmp/test_iterator \
    dist/tmp/test_lens \
    dist/tmp/test_string \
    dist/tmp/test_utility

dist/tmp/test_cxx11: test/cxx11.cpp calico/cxx11.hpp
	mkdir -p dist/tmp
	$(CXX) $(CXXFLAGS) -Wno-sign-conversion -o $@ -c test/cxx11.cpp

dist/tmp/test_iterator: test/iterator.cpp calico/iterator.hpp
	mkdir -p dist/tmp
	$(CXX) $(CXXFLAGS) -o $@ test/iterator.cpp \
	        && dist/tmp/test_iterator

dist/tmp/test_lens: test/lens.cpp calico/lens.hpp
	mkdir -p dist/tmp
	$(CXX) $(CXXFLAGS) -o $@ test/lens.cpp \
	        && dist/tmp/test_lens

dist/tmp/test_string: test/string.cpp calico/string.hpp dist/tmp/libsnprintf.a
	mkdir -p dist/tmp
	$(CXX) $(CXXFLAGS) -o $@ dist/tmp/libsnprintf.a test/string.cpp \
	        && dist/tmp/test_string

dist/tmp/test_utility: test/utility.cpp calico/utility.hpp
	mkdir -p dist/tmp
	$(CXX) $(CXXFLAGS) -o $@ test/utility.cpp \
	        && dist/tmp/test_utility

dist/tmp/libsnprintf.a:
	mkdir -p dist/tmp/snprintf
	cd dist/tmp/snprintf \
	  && ../../../tools/download $(SNPRINTF_URL) | tar -xz \
	  && cd $(SNPRINTF_NAME) \
	  && $(CC) -O2 -c snprintf.c \
	  && $(AR) cru ../../libsnprintf.a snprintf.o
	rm -r dist/tmp/snprintf
