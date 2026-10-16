# Chi bien dich lai file thay doi va cac file phu thuoc header do.
SHELL := cmd.exe
.SHELLFLAGS := /d /c
CXX := C:/msys64/ucrt64/bin/g++.exe
CXXFLAGS := -std=c++17 -g -O0 -Wall -Wextra -Wpedantic -MMD -MP

SOURCES := main.cpp $(wildcard models/*.cpp enums/*.cpp utils/*.cpp repositories/*.cpp services/*.cpp views/*.cpp)
OBJECTS := $(patsubst %.cpp,.build/%.o,$(SOURCES))
DEPENDENCIES := $(OBJECTS:.o=.d)
OBJECT_DIRS := $(sort $(dir $(OBJECTS)))

.PHONY: all
.DELETE_ON_ERROR:
all: StudentDocumentSystem.exe

StudentDocumentSystem.exe: $(OBJECTS) ../.vscode/build.mk
	$(CXX) $(OBJECTS) -lcrypto -o "$@"

$(OBJECTS): ../.vscode/build.mk

.build/%.o: %.cpp | $(OBJECT_DIRS)
	$(CXX) $(CXXFLAGS) -c "$<" -o "$@"

$(OBJECT_DIRS):
	if not exist "$(subst /,\,$@)" mkdir "$(subst /,\,$@)"

-include $(DEPENDENCIES)
