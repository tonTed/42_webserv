# Name of the final executable
NAME = webserv

# Paths configurations
SRCDIR := src
OBJDIR := obj

# Create the list of directories
DIRS = $(shell find $(SRCDIR) -type d | sed 's/$(SRCDIR)\///g' | sed -n '1!p')
SRCDIRS	= $(foreach dir, $(DIRS), $(addprefix $(SRCDIR)/, $(dir)))
OBJDIRS = $(foreach dir, $(DIRS), $(addprefix $(OBJDIR)/, $(dir)))

# Create a list of *.c sources in DIRS
SRCS = $(wildcard $(SRCDIR)/*.cpp)
SRCS += $(foreach dir, $(SRCDIRS), $(wildcard $(dir)/*.cpp))

SRC_TEST = $(wildcard test/*.cpp)
SRC_TEST += $(SRCS)
SRC_TEST := $(filter-out $(SRCDIR)/main.cpp, $(SRC_TEST))

# Define objects for all sources
OBJS = $(patsubst $(SRCDIR)/%.cpp, $(OBJDIR)/%.o, $(SRCS))

TESTER_DIRS	= 	YoupiBanane \
				YoupiBanane/nop \
				YoupiBanane/Yeah

TESTER_FILES =	YoupiBanane/youpi.bad_extension \
				YoupiBanane/youpi.bla \
				YoupiBanane/nop/other.pouic \
				YoupiBanane/nop/youpi.bad_extension \
				YoupiBanane/Yeah/not_happy.bad_extension

TESTER_TOOLS = 	tester \
				cgi_tester

# Name the compiler & flags
CC = c++
#TODO PUT BACK FLAG WITH C++98
#CFLAGS = -Wall -Wextra -Werror -std=c++98
CFLAGS = -Wall -Wextra -Werror
CFLAGS += -I. -MMD

# rules for compile
$(OBJDIR)/%.o: $(SRCDIR)/%.cpp
	$(HIDE)$(CC) $(CFLAGS) -c $< -o $@

all			: buildrepo $(NAME)
		$(HIDE) mkdir -p data/files

$(NAME)		: $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(NAME)
	@printf $(GREEN)"[$@] program created\n"$(RESET)

clean		:
	$(HIDE)rm -rf $(OBJDIR)
	@printf $(YELLOW)"[$(NAME)] objects removed\n"$(RESET)

fclean		: clean
	$(HIDE)rm -f $(NAME)
	$(HIDE)rm -rf $(TESTER_TOOLS)
	$(HIDE)rm -rf $(TESTER_DIRS)

re			: fclean all

utest		: buildrepo
	$(HIDE) mkdir -p data/files
	$(CC) $(CFLAGS) $(SRC_TEST) -o utest
	@printf $(GREEN)"[$@] program created\n"$(RESET)
	./utest
	rm -f utest

tester:
	$(HIDE) mkdir -p $(TESTER_DIRS)
	$(HIDE) touch $(TESTER_FILES)
	$(HIDE) cp test/tester_tools/tester tester
	$(HIDE) cp test/tester_tools/cgi_tester cgi_tester
	./tester http://localhost:8080

print		:
	@#echo $(SRCS)
	@echo $(SRC_TEST)

run			: all
	./$(NAME)

rrun		: re run

mock		: all
	./$(NAME) -m

pdf			:
	@open subject/webserv.pdf

# Create  repo of objects!
buildrepo	:
	$(HIDE)$(call make-repo)

.PHONY		: all clean fclean re print run buildrepo $(NAME) tester

define make-repo
	$(HIDE)mkdir -p $(OBJDIR)
	$(HIDE)for dir in $(DIRS); \
   	do \
	mkdir -p $(OBJDIR)/$$dir; \
   	done
endef


# Color
BLACK := "\e[0;30m"
RED := "\e[0;31m"
GREEN := "\e[0;32m"
YELLOW := "\e[0;33m"
BLUE := "\e[0;34m"
MAGENTA := "\e[0;35m"
CYAN := "\e[0;36m"
RESET :="\e[0m"

# Decide whether the commands will be shwon or not
VERBOSE = FALSE

ifeq ($(VERBOSE),TRUE)
    HIDE =
else
    HIDE = @
endif

# Recompilation with modified headers
-include $(OBJS:)
