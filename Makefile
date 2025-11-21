LIBS = -lSDL2 -lGL -lGLU
OWN_PATHS = -I.
HEADERS = $(OWN_PATHS) -I/usr/include/SDL2
DEFINES = -DUSE_SDL
CFLAGS = -std=c++11 -Wall -O3 -ggdb
NAME = opengw
OBJDIR = obj

ALL_SRC = $(wildcard *.cpp)
SRC_FILES = $(notdir $(ALL_SRC))
OBJ_FILES = $(SRC_FILES:.cpp=.o)

OBJS = $(addprefix $(OBJDIR)/, $(OBJ_FILES))
DEPS = $(OBJS:.o=.d)

CLANG_ANALYZE   = #--analyze
CLANG_THREAD    = #-fsanitize=thread
CLANG_ADDRESS   = #-fsanitize=address
CLANG_MEMORY    = #-fsanitize=memory
CLANG_UNDEFINED = #-fsanitize=undefined

CLANG_FLAGS= $(CLANG_ANALYZE) $(CLANG_THREAD) $(CLANG_ADDRESS) $(CLANG_MEMORY) $(CLANG_UNDEFINED)

COMPILER = g++
#COMPILER = clang++

all: $(NAME)
	./$(NAME)

# dependencies
$(OBJDIR)/%.d : %.cpp | $(OBJDIR)
	$(COMPILER) -MM -MP -MT $(@:.d=.o) -o $@ $< $(CFLAGS) $(DEFINES) $(HEADERS)

# compiling
$(OBJDIR)/%.o : %.cpp
	$(COMPILER) -o $@ -c $< $(CFLAGS) $(CLANG_FLAGS) $(DEFINES) $(HEADERS)

clean:
	rm $(OBJDIR)/*

$(NAME): $(OBJS)
	$(COMPILER) -o $@ $(OBJS) $(LIBS) $(CLANG_FLAGS)

$(OBJDIR):
	mkdir -p $@

# Load .d files
ifneq ($(MAKECMDGOALS),clean)
-include $(DEPS)
endif
