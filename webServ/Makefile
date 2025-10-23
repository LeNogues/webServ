NAME	:= webServ
CC		:= c++
CFLAGS	:= -Wall -Wextra -Werror -std=c++98 -g3
IFLAGS	:= -Iincludes/config -Iincludes/request -Iincludes/server -Iincludes/utils

SRC     := 	srcs/main.cpp \
			srcs/config/Config.cpp \
			srcs/httpGen/httpStatus.cpp \
			srcs/httpGen/generateErrorPage.cpp \
			srcs/httpGen/generateAutoIndex.cpp \
			srcs/request/Request.cpp \
			srcs/server/Client.cpp \
			srcs/server/Server.cpp \
			srcs/server/WebServer.cpp \
			srcs/server/handleFunction/handleConnection.cpp \
			srcs/server/handleFunction/handleRequest.cpp \
			srcs/server/handleFunction/handleWriteRead.cpp \
			srcs/server/handleFunction/switchState.cpp \
			srcs/utils/converters.cpp \
			srcs/utils/trim.cpp \
			srcs/utils/writeError.cpp \
			srcs/utils/parsePath.cpp \
			srcs/utils/setNonBlocking.cpp

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
