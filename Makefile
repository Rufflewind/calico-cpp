DOC=dist/doc
TMP=dist/tmp
CXX03=--std=c++03
CXX11=--std=c++11
DL_NAME=snprintf_2.2
DL_URL=http://www.ijs.si/software/snprintf/$(DL_NAME).tar.gz
CXXFLAGS=$(CXX11) -Wall -I.
CCFLAGS_EXT=-O3 # For external libraries

all: test doc

clean:
	rm -r dist

test: \
  $(TMP)/libsnprintf.a \
  test/cxx11.cpp \
  test/iterator.cpp \
  test/lens.cpp \
  test/string.cpp
	mkdir -p $(TMP)
	$(CXX) $(CXXFLAGS) -o /dev/null -c test/cxx11.cpp
	$(CXX) $(CXXFLAGS) -o $(TMP)/test test/iterator.cpp \
	  && $(TMP)/test
	$(CXX) $(CXXFLAGS) -o $(TMP)/test \
	     $(TMP)/libsnprintf.a test/string.cpp \
	  && $(TMP)/test
	$(CXX) $(CXXFLAGS) -o $(TMP)/test test/lens.cpp \
	  && $(TMP)/test

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
