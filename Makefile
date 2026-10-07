NAME := codexion

CC := cc
CFLAGS := -I./include -Wall -Wextra -Werror -pthread -MMD -MP

SRC := \
	src/cmp.c \
	src/coder_routine_utils.c \
	src/coder_routine.c \
	src/coder.c \
	src/context.c \
	src/dongle.c \
	src/get.c \
	src/log_state.c \
	src/main.c \
	src/monitor.c \
	src/pqueue_ops.c \
	src/pqueue.c \
	src/setup.c \
	src/time_utils.c \
	src/traceback.c
OBJ := $(SRC:src/%.c=obj/%.o)
DEP := $(OBJ:.o=.d)


# Commands
# -----------------------------------------------------------------------------
all: $(NAME)

clean:
	rm -rf obj

fclean: clean
	rm -f $(NAME)

re: fclean all


# Test commands
# -----------------------------------------------------------------------------
norm:
	norminette


# Phonies
# -----------------------------------------------------------------------------
.PHONY: all clean fclean re norm


# Files
# -----------------------------------------------------------------------------
obj/%.o: src/%.c | obj/
	$(CC) $(CFLAGS) -c $< -o $@

$(NAME): $(OBJ)
	$(CC) $(CFLAGS) $(OBJ) -o $@

obj/:
	mkdir -p $@

-include $(DEP)
