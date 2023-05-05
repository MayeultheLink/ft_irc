NAME		=	ircserv

SRCS		=	srcs/main.cpp \
			srcs/Server.cpp \
			srcs/ClientInfo.cpp
#			srcs/Channel.cpp \

CC		=	c++
CFLAGS		=	-Wall -Wextra -Werror -I./incs -std=c++98
OBJS		=	${SRCS:.cpp=.o}
DEPS		=	${OBJS:.o=.d}

all		:	${NAME}

-include $(DEPS)

$(NAME)		:	$(OBJS)
			${CC} ${CFLAGS} ${OBJS} -o ${NAME}

%.o:%.cpp		
			$(CC) -MMD $(CFLAGS) -c $< -o $@

clean		:
			rm -f ${OBJS} ${DEPS}

fclean		:
			rm -f ${OBJS} ${DEPS} ${NAME}

re		:	fclean all

.PHONY		:	all clean fclean re
