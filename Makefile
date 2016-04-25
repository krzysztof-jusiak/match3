TGT:=match3
CXXFLAGS:=-O2 -std=c++14 -I src -I libs/msm-lite/include -Ilibs/di/include -Ilibs/range-v3/include #-Wall -Wextra -Wno-c99-extensions
CXXFLAGS_EMSCRIPTEN:=-Wwarn-absolute-paths --emrun -s USE_SDL=2 -s USE_SDL_IMAGE=2 -s USE_SDL_TTF=2
CXXFLAGS_APP:=-I/usr/local/include/SDL2
LINKFLAGS_EMSCRIPTEN:=--preload-file data --use-preload-plugins
LINKFLAGS_APP:=-lSDL2 -lSDL2_image -lSDL2_ttf

.PHONY: all style pph web web_run app app_run release clean test

all: app app_run

style:
	find src test -iname "*.hpp" -or -iname "*.cpp" | xargs clang-format -i

pph:
	$(CXX) $(CXXFLAGS) $(CXXFLAGS_APP) src/fwd.hpp

web:
	em++ $(CXXFLAGS) $(CXXFLAGS_EMSCRIPTEN) $(LINKFLAGS_EMSCRIPTEN) src/main.cpp -o index.html --shell-file data/template.html

web_run:
	emrun --port 8080 index.html

app:
	$(CXX) $(CXXFLAGS) $(CXXFLAGS_APP) $(LINKFLAGS_APP) src/main.cpp -o $(TGT)

app_run:
	./$(TGT)

release: clean web
	git stash save && TMP=`mktemp -d /tmp/match.XXXX` && mv index.* $$TMP && git checkout gh-pages && mv $$TMP/index.* . && git add index.* && git commit -m "release" && git push origin +HEAD:gh-pages && git checkout master && rm -rf $$TMP && git stash pop

clean:
	rm -f index.* $(TGT) *.out

test:
	$(CXX) $(CXXFLAGS) -include test/common/test.hpp test/functional_tests.cpp -o functional_tests.out && ./functional_tests.out
