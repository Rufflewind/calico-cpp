DIST=dist
DOC=$(DIST)/doc
TMP=$(DIST)/tmp
CXX03=--std=c++03
CXX11=--std=c++11
DL_NAME=snprintf_2.2
DL_URL=http://www.ijs.si/software/snprintf/$(DL_NAME).tar.gz
CXXFLAGS=$(CXX11) -Wall -I.
CCFLAGS_EXT=-O3 # For external libraries
CXXF=$(CXX) $(CXXFLAGS)

all: test doc

clean:
	rm -fr $(DIST)

test: \
    $(TMP)/libsnprintf.a \
    $(TMP)/test_cxx11 \
    $(TMP)/test_iterator \
    $(TMP)/test_lens \
    $(TMP)/test_string \
    $(TMP)/test_utility

$(TMP)/test_cxx11: test/cxx11.cpp calico/cxx11.hpp
	mkdir -p $(TMP)
	$(CXXF) -o $(TMP)/test_cxx11.o -c test/cxx11.cpp

$(TMP)/test_iterator: test/iterator.cpp calico/iterator.hpp
	mkdir -p $(TMP)
	$(CXXF) -o $(TMP)/test_iterator test/iterator.cpp \
	        && $(TMP)/test_iterator

$(TMP)/test_lens: test/lens.cpp calico/lens.hpp
	mkdir -p $(TMP)
	$(CXXF) -o $(TMP)/test_lens test/lens.cpp \
	        && $(TMP)/test_lens

$(TMP)/test_string: test/string.cpp calico/string.hpp $(TMP)/libsnprintf.a
	mkdir -p $(TMP)
	$(CXXF) -o $(TMP)/test_string $(TMP)/libsnprintf.a test/string.cpp \
	        && $(TMP)/test_string

$(TMP)/test_utility: test/utility.cpp calico/utility.hpp
	mkdir -p $(TMP)
	$(CXXF) -o $(TMP)/test_utility test/utility.cpp \
	        && $(TMP)/test_utility

$(TMP)/libsnprintf.a:
	mkdir -p $(TMP)/snprintf
	cd $(TMP)/snprintf/ \
	  && wget -O - $(DL_URL) | tar -xz \
	  && cd $(DL_NAME) \
	  && $(CC) $(CCFLAGS_EXT) -c snprintf.c \
	  && $(AR) cru ../../libsnprintf.a snprintf.o
	rm -r $(TMP)/snprintf/

doc: $(DOC)/.git/config
	doxygen
	cd $(DOC) \
	  && git add -u \
	  && git add . \
	  && git commit --amend -m Autogenerated \
	       --author="Autogenerated <noreply@dev.null>" \
	  && git push origin gh-pages --force

$(DOC)/.git/config:
	mkdir -p $(DOC)
	cd $(DOC) \
	  && git init \
	  && git remote add -t gh-pages -f origin \
	       git@github.com:Rufflewind/calico.git \
	  && git checkout gh-pages

.PHONY: clean doc test
