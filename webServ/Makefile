NAME	:= webServ
CC		:= c++
CFLAGS	:= -Wall -Wextra -Werror -std=c++98
IFLAGS	:= -Iincludes/config -Iincludes/request -Iincludes/server -Iincludes/utils

SRC     := 	srcs/main.cpp \
			srcs/config/Config.cpp \
			srcs/utils/writeError.cpp \
			srcs/utils/trim.cpp \
			srcs/utils/httpStatus.cpp \
			srcs/utils/strToSizeT.cpp \
			srcs/request/Request.cpp \
			srcs/server/Server.cpp \
			srcs/server/WebServer.cpp \
			srcs/server/Client.cpp

OBJ_DIR := obj
OBJS    := $(SRC:%.cpp=$(OBJ_DIR)/%.o)

# Règles

all: $(NAME)

$(NAME): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(NAME)

debug: $(OBJS)
	$(CC) $(CFLAGS) -g3 $(OBJS) -o $(NAME)

# Compile les .cpp en .o dans obj/, en créant les sous-dossiers si nécessaire
$(OBJ_DIR)/%.o: %.cpp
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $(IFLAGS) -c $< -o $@

clean:
	@rm -rf $(OBJ_DIR)

fclean: clean
	@rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re debug
