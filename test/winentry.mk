TMP=..\dist\tmp^\
CXXFLAGS=/nologo /EHsc /Fe$(TMP)test.exe /Fo$(TMP) /GL /I.. /Ox /W4
LIBS=user32.lib
LDFLAGS=/link /ENTRY:mainCRTStartup

main: $(TMP)test.exe

$(TMP)test.exe: ..\winentry\winentry.h ..\winentry\winentry.c winentry_test.cpp
	-md $(TMP)
	$(CXX) $(CXXFLAGS) $(LIBS) ..\winentry\winentry.c winentry_test.cpp \
	  $(LDFLAGS)

clean:
	-del /s /q $(TMP)winentry.obj $(TMP)winentry_test.obj $(TMP)test.exe
