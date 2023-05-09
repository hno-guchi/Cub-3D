NAME = cub3D
CC = cc
CFLAGS = -Wall -Wextra -Werror
LIBS = -Lmlx -lmlx -framework OpenGL -framework AppKit
SOURCES_DIR = ./srcs
LIBFT_DIR = ./srcs/libft
MAPS_DIR = $(SOURCES_DIR)/maps
RAYCASTING_DIR = $(SOURCES_DIR)/raycasting
WINDOW_DIR = $(SOURCES_DIR)/window

SOURCES = $(SOURCES_DIR)/main.c\
          $(MAPS_DIR)/read_map.c\
          $(MAPS_DIR)/get_map_data.c\
          $(MAPS_DIR)/check_map_wall.c\
          $(WINDOW_DIR)/close_window.c\
          $(WINDOW_DIR)/new_window.c\
          $(SOURCES_DIR)/free.c\
          $(SOURCES_DIR)/init.c\
          $(SOURCES_DIR)/print_error.c\
          $(GNL_DIR)/get_next_line.c\
          $(GNL_DIR)/get_next_line_utils.c\

GNL_DIR = ./srcs/get_next_line
GNL = $(GNL_DIR)/get_next_line.a

MINILIBX_DIR = ./mlx
MINILIBX	 = $(MINILIBX_DIR)/libmlx.a

INCLUDE = -I include -I $(LIBFT_DIR) -I $(GNL_DIR) -I $(MINILIBX_DIR)

# makeがファイルを検索する際に使用するディレクトリのリストを指定するための変数
VPATH = $(SOURCES_DIR):$(MAPS_DIR):$(RAYCASTING_DIR):$(GNL_DIR) \
		$(WINDOW_DIR)

OBJS_DIR = objs
# addprefixの構成[$(addprefix prefix, list)]
# prefix: 追加する接頭辞
# list: 接頭辞を追加する文字列のリスト
# notdir 関数は、引数で与えられたパスのうち、ディレクトリ部分を取り除き、ファイル名のみを返します。
OBJS = $(addprefix $(OBJS_DIR)/, $(notdir $(SOURCES:.c=.o)))

$(OBJS_DIR)/%.o: %.c
	mkdir -p $(OBJS_DIR)
	$(CC) $(CFLAGS) $(INCLUDE) -c $< -o $@

LIBFT = -L$(LIBFT_DIR) -lft

all: $(NAME)

$(MINILIBX_DIR) :
	make -C $(MINILIBX_DIR)

$(NAME) : $(OBJS)
	$(MAKE) -C $(LIBFT_DIR)
	$(CC) $(CFLAGS) $(OBJS) $(INCLUDE) $(LIBFT) $(LIBS) -o $(NAME)


clean:
	rm -rf $(OBJS_DIR)
	$(MAKE) -C $(LIBFT_DIR) clean

fclean : clean
	rm -f $(NAME)
	$(MAKE) -C $(MINILIBX_DIR) clean
	$(MAKE) -C $(LIBFT_DIR) fclean

re: fclean all
