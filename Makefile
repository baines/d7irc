UI   := $(patsubst data/%.ui,build/%_ui.h,$(wildcard data/*.ui))
MOC  := $(patsubst src/%_qt.h,src/%_qt.cpp,$(wildcard src/*_qt.h))
QRC  := $(patsubst data/%.qrc,src/%_qrc.cpp,$(wildcard data/*.qrc))
SRCS := $(wildcard src/*.cpp) $(MOC) $(QRC)
HDRS := $(wildcard src/*.h)
OBJS := $(patsubst src/%.cpp,build/%.o,$(SRCS))

CFLAGS := -std=c++14 -Isrc -Ibuild -fPIC -g -Ibuild/libircclient-1.9/include\
 $(shell pkg-config --cflags Qt5Core Qt5Gui Qt5Widgets Qt5Network)

samurairc: $(OBJS)
	$(CXX) $(CFLAGS) $^ -o $@ -lQt5Core -lQt5Gui -lQt5Widgets -lQt5Network build/libircclient.a -lssl -lcrypto

build/%.o: src/%.cpp $(UI) $(HDRS)
	$(CXX) $(CFLAGS) -c $< -o $@

build/%_ui.h: data/%.ui
	uic $< -o $@

src/%_qt.cpp: src/%_qt.h
	moc $< -o $@

src/%_qrc.cpp: data/%.qrc
	rcc $< -o $@

clean:
	$(RM) build/*.h build/*.o samurairc $(wildcard src/*_qt.cpp) $(wildcard src/*_qrc.cpp)

.PHONY: clean

.SECONDARY:
