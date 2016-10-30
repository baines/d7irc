UI   := $(patsubst ui/%.ui,build/%_ui.h,$(wildcard ui/*.ui))
SRCS := $(wildcard src/*.cpp) src/d7irc_qt.cpp
HDRS := $(wildcard src/*.h)
OBJS := $(patsubst src/%.cpp,build/%.o,$(SRCS))

CFLAGS := -std=c++14 -Isrc -Ibuild -fPIC -g\
 $(shell pkg-config --cflags Qt5Core Qt5Gui Qt5Widgets)

d7irc: $(OBJS)
	$(CXX) $(CFLAGS) $^ -o $@ -lQt5Core -lQt5Gui -lQt5Widgets -lircclient

build/%.o: src/%.cpp $(UI)
	$(CXX) $(CFLAGS) -c $< -o $@

build/%_ui.h: ui/%.ui
	uic $< -o $@

src/%_qt.cpp: src/%_qt.h
	moc $< -o $@

clean:
	$(RM) build/*.h build/*.o d7irc

.PHONY: clean
