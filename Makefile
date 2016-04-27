#
# Copyright (c) 2016 Krzysztof Jusiak (krzysztof at jusiak dot net)
#
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)
#
TGT:=match3
VALGRIND:=valgrind --leak-check=full --error-exitcode=1
CXXFLAGS:=-std=c++14 -I src -I libs/msm-lite/include -Ilibs/di/include -Ilibs/range-v3/include #-Wall -Wextra -Wno-c99-extensions
CXXFLAGS_EMSCRIPTEN:=-Wwarn-absolute-paths --emrun -s USE_SDL=2 -s USE_SDL_IMAGE=2 -s USE_SDL_TTF=2
CXXFLAGS_APP:=-I/usr/local/include/SDL2
LINKFLAGS_EMSCRIPTEN:=--preload-file data --use-preload-plugins
LINKFLAGS_APP:=-lSDL2 -lSDL2_image -lSDL2_ttf

.PHONY: all style pph web web_run app app_run release clean test

all: app app_run

style:
	find src test -iname "*.hpp" -or -iname "*.cpp" | xargs clang-format -i

pph:
	$(CXX) $(CXXFLAGS) $(CXXFLAGS_APP) -O2 src/fwd.hpp

web:
	em++ $(CXXFLAGS) $(CXXFLAGS_EMSCRIPTEN) -O2 $(LINKFLAGS_EMSCRIPTEN) src/main.cpp -o index.html --shell-file data/template.html

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

test: test_unit_tests test_integration_tests test_functional_tests

test_%:
	$(CXX) $(CXXFLAGS) -g -include test/common/test.hpp test/$*.cpp -o $*.out && $($(MEMCHECK)) ./$*.out

test_unit_tests:
	$(CXX) $(CXXFLAGS) -g -include test/common/test.hpp test/unit_tests/actions.cpp -o unit_tests_actions.out && $($(MEMCHECK)) ./unit_tests_actions.out
	$(CXX) $(CXXFLAGS) -g -include test/common/test.hpp test/unit_tests/guards.cpp -o unit_tests_guards.out && $($(MEMCHECK)) ./unit_tests_guards.out
	$(CXX) $(CXXFLAGS) -g -include test/common/test.hpp test/unit_tests/logic.cpp -o unit_tests_logic.out && $($(MEMCHECK)) ./unit_tests_logic.out
