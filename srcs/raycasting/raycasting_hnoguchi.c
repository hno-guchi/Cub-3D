/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   raycasting_hnoguchi.c                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: susasaki <susasaki@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/14 14:33:56 by hnoguchi          #+#    #+#             */
/*   Updated: 2023/07/06 16:03:48 by susasaki         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cub3d.h"

// TODO: destruct_raycasting.c
void	destroy_window(t_vars *vars)
{
	if (vars->mlx != NULL && vars->win != NULL)
	{
		mlx_destroy_window(vars->mlx, vars->win);
	}
}

// TODO: destruct_raycasting.c
void	destroy_textures(t_vars *vars)
{
	int	i;

	i = 0;
	if (vars->mlx == NULL)
	{
		return ;
	}
	while (i < TEXTURE_LIST_SIZE)
	{
		if (vars->texture_list[i].data.img != NULL)
		{
			mlx_destroy_image(vars->mlx, vars->texture_list[i].data.img);
		}
		i += 1;
	}
}

// TODO: destruct_raycasting.c
void	destruct_raycasting(t_info *info)
{
	destroy_window(info->vars);
	destroy_textures(info->vars);
	free(info->vars->mlx);
}

// TODO: texture_mlx_pixel_put_line.c
void	texture_mlx_pixel_put_line(t_vars *vars, int x_axis, int y_axis[2],
		unsigned int color)
{
	int				y;
	unsigned int	*dst;

	y = y_axis[0];
	while (y <= y_axis[1])
	{
		dst = vars->data->addr + (y * WIN_WIDTH + x_axis);
		*dst = color;
		y += 1;
	}
}

// TODO: get_nearest_axis.c
static bool	is_hit_wall(char **map, t_ray *ray)
{
	if ('0' < map[ray->x_map][ray->y_map]
		&& map[ray->x_map][ray->y_map] <= '9')
	{
		return (true);
	}
	return (false);
}

// TODO: get_nearest_axis.c
static int	calculate_step_x_direction(t_ray *ray, t_vars *vars)
{
	if (ray->x_dir < 0)
	{
		ray->x_side_dist = (vars->x_pos - ray->x_map)
			* ray->x_delta_dist;
		return (-1);
	}
	ray->x_side_dist = (ray->x_map + 1.0
			- vars->x_pos) * ray->x_delta_dist;
	return (1);
}

// TODO: get_nearest_axis.c
static int	calculate_step_y_direction(t_ray *ray, t_vars *vars)
{
	if (ray->y_dir < 0)
	{
		ray->y_side_dist = (vars->y_pos - ray->y_map)
			* ray->y_delta_dist;
		return (-1);
	}
	ray->y_side_dist = (ray->y_map + 1.0
			- vars->y_pos) * ray->y_delta_dist;
	return (1);
}

// TODO: get_nearest_axis.c
// perform DDA
int	get_nearest_axis(t_ray *ray, t_info *info)
{
	int	step_x;
	int	step_y;
	int	axis;

	step_x = calculate_step_x_direction(ray, info->vars);
	step_y = calculate_step_y_direction(ray, info->vars);
	while (1)
	{
		if (ray->x_side_dist < ray->y_side_dist)
		{
			ray->x_side_dist += ray->x_delta_dist;
			ray->x_map += step_x;
			axis = X_AXIS;
		}
		else
		{
			ray->y_side_dist += ray->y_delta_dist;
			ray->y_map += step_y;
			axis = Y_AXIS;
		}
		if (is_hit_wall(info->map->map_data, ray))
			break ;
	}
	return (axis);
}

// TODO: try_draw_texture_floor_and_ceiling.c
void	set_draw_background(t_draw_background *draw, t_vars *vars,
		int y_coord, float vertical_position_camera)
{
	float	ray_dir_left[2];
	float	ray_dir_right[2];
	float	row_dist;

	ray_dir_left[X_AXIS] = vars->x_dir - vars->x_cam_plane;
	ray_dir_left[Y_AXIS] = vars->y_dir - vars->y_cam_plane;
	ray_dir_right[X_AXIS] = vars->x_dir + vars->x_cam_plane;
	ray_dir_right[Y_AXIS] = vars->y_dir + vars->y_cam_plane;
	row_dist = vertical_position_camera / y_coord;
	draw->x_span = row_dist * (ray_dir_right[X_AXIS]
			- ray_dir_left[X_AXIS]) / WIN_WIDTH;
	draw->y_span = row_dist * (ray_dir_right[Y_AXIS]
			- ray_dir_left[Y_AXIS]) / WIN_WIDTH;
	draw->x_coord = vars->x_pos + row_dist
		* ray_dir_left[X_AXIS];
	draw->y_coord = vars->y_pos + row_dist
		* ray_dir_left[Y_AXIS];
}

// TODO: try_draw_texture_floor_and_ceiling.c
int	decide_texture_floor(int cell[2])
{
	int	checker_board_pattern;

	checker_board_pattern = (int)((cell[X_AXIS] + cell[Y_AXIS]) & 1);
	if (checker_board_pattern == 0)
	{
		return (FLOOR_1);
	}
	return (FLOOR_2);
}

// TODO: try_draw_texture_floor_and_ceiling.c
void	put_texture_floor(t_draw_background *draw, t_vars *vars,
		int coordinate_screen[2], int cell[2])
{
	int				coordinate_texture[2];
	int				floor_texture;
	unsigned int	color;

	floor_texture = decide_texture_floor(cell);
	coordinate_texture[X_AXIS] = (int)(vars->texture_list[floor_texture].width
			* (draw->x_coord
				- cell[X_AXIS])) & (vars->texture_list[floor_texture].width
			- 1);
	coordinate_texture[Y_AXIS] = (int)(vars->texture_list[floor_texture].height
			* (draw->y_coord
				- cell[Y_AXIS])) & (vars->texture_list[floor_texture].height
			- 1);
	color = *(vars->texture_list[floor_texture].data.addr
			+ vars->texture_list[floor_texture].width
			* coordinate_texture[Y_AXIS] + coordinate_texture[X_AXIS]);
	color = (color >> 1) & 8355711;
	vars->data->addr[(WIN_WIDTH * coordinate_screen[Y_AXIS])
		+ coordinate_screen[X_AXIS]] = color;
}

// TODO: try_draw_texture_floor_and_ceiling.c
void	put_texture_ceiling(t_draw_background *draw, t_vars *vars,
		int coordinate_screen[2], int cell[2])
{
	int				coordinate_texture[2];
	unsigned int	color;

	coordinate_texture[X_AXIS] = (int)(vars->texture_list[CEILING].width
			* (draw->x_coord
				- cell[X_AXIS])) & (vars->texture_list[CEILING].width - 1);
	coordinate_texture[Y_AXIS] = (int)(vars->texture_list[CEILING].height
			* (draw->y_coord
				- cell[Y_AXIS])) & (vars->texture_list[CEILING].height - 1);
	color = *(vars->texture_list[CEILING].data.addr
			+ vars->texture_list[CEILING].height * coordinate_texture[Y_AXIS]
			+ coordinate_texture[X_AXIS]);
	color = (color >> 1) & 8355711;
	vars->data->addr[(WIN_WIDTH * (WIN_HEIGHT
				- coordinate_screen[Y_AXIS]))
		+ coordinate_screen[X_AXIS]] = color;
}

// TODO: try_draw_texture_floor_and_ceiling.c
int	draw_texture_floor(t_vars *vars)
{
	int					coordinate_screen[2];
	int					cell[2];
	t_draw_background	draw;

	coordinate_screen[Y_AXIS] = (WIN_HEIGHT / 2) - 1;
	while (coordinate_screen[Y_AXIS] < WIN_HEIGHT)
	{
		set_draw_background(&draw, vars,
			coordinate_screen[Y_AXIS] - (WIN_HEIGHT / 2), 0.5 * WIN_HEIGHT);
		coordinate_screen[X_AXIS] = 0;
		while (coordinate_screen[X_AXIS] < WIN_WIDTH)
		{
			cell[X_AXIS] = (int)draw.x_coord;
			cell[Y_AXIS] = (int)draw.y_coord;
			put_texture_floor(&draw, vars, coordinate_screen, cell);
			coordinate_screen[X_AXIS] += 1;
			draw.x_coord += draw.x_span;
			draw.y_coord += draw.y_span;
		}
		coordinate_screen[Y_AXIS] += 1;
	}
	return (0);
}

// TODO: try_draw_texture_floor_and_ceiling.c
int	draw_texture_ceiling(t_vars *vars)
{
	int					coordinate_screen[2];
	int					cell[2];
	t_draw_background	draw;

	coordinate_screen[Y_AXIS] = (WIN_HEIGHT / 2) - 1;
	while (coordinate_screen[Y_AXIS] < WIN_HEIGHT)
	{
		set_draw_background(&draw, vars, coordinate_screen[Y_AXIS]
			- (WIN_HEIGHT / 2), 0.5 * WIN_HEIGHT);
		coordinate_screen[X_AXIS] = 0;
		while (coordinate_screen[X_AXIS] < WIN_WIDTH)
		{
			cell[X_AXIS] = (int)draw.x_coord;
			cell[Y_AXIS] = (int)draw.y_coord;
			put_texture_ceiling(&draw, vars, coordinate_screen, cell);
			coordinate_screen[X_AXIS] += 1;
			draw.x_coord += draw.x_span;
			draw.y_coord += draw.y_span;
		}
		coordinate_screen[Y_AXIS] += 1;
	}
	return (0);
}

// TODO: try_draw_texture_floor_and_ceiling.c
void	try_draw_texture_floor_and_ceiling(t_vars *vars)
{
	if (vars->texture_list[FLOOR_1].data.img != NULL)
	{
		draw_texture_floor(vars);
	}
	if (vars->texture_list[CEILING].data.img != NULL)
	{
		draw_texture_ceiling(vars);
	}
}

// TODO: draw_color_floor_and_ceiling.c
void	draw_color_floor_and_ceiling(t_vars *vars, unsigned int floor_col
		, unsigned int ceil_col)
{
	int	x_axis;
	int	y_axis_floor[2];
	int	y_axis_ceiling[2];

	x_axis = 0;
	y_axis_floor[0] = 0;
	y_axis_floor[1] = (WIN_HEIGHT / 2) - 1;
	y_axis_ceiling[0] = y_axis_floor[1];
	y_axis_ceiling[1] = WIN_HEIGHT - 1;
	while (x_axis < WIN_WIDTH)
	{
		texture_mlx_pixel_put_line(vars, x_axis, y_axis_floor, floor_col);
		texture_mlx_pixel_put_line(vars, x_axis, y_axis_ceiling, ceil_col);
		x_axis += 1;
	}
}
// TODO: draw_wall.c
void	set_ray_data(t_ray *ray, t_vars *vars, int x)
{
	double	x_current_camera;

	x_current_camera = 2 * x / (double)WIN_WIDTH - 1;
	ray->x_dir = vars->x_dir + (vars->x_cam_plane
			* x_current_camera);
	ray->y_dir = vars->y_dir + (vars->y_cam_plane
			* x_current_camera);
	ray->x_map = (int)vars->x_pos;
	ray->y_map = (int)vars->y_pos;
	ray->x_side_dist = 0;
	ray->y_side_dist = 0;
	if (ray->x_dir == 0)
	{
		ray->x_delta_dist = 1e30;
	}
	else
		ray->x_delta_dist = ABS(1 / ray->x_dir);
	if (ray->y_dir == 0)
	{
		ray->y_delta_dist = 1e30;
	}
	else
		ray->y_delta_dist = ABS(1 / ray->y_dir);
}

// TODO: draw_wall.c
double	get_wall_distance(t_ray *ray, int side)
{
	if (side == X_AXIS)
	{
		return (ray->x_side_dist - ray->x_delta_dist);
	}
	return (ray->y_side_dist - ray->y_delta_dist);
}

// TODO: set_draw_texture_data.c
int	decide_draw_texture(t_ray *ray, t_vars *vars, int side)
{
	if (side == Y_AXIS)
	{
		if (ray->y_map < vars->y_pos)
		{
			return (WEST_WALL);
		}
		return (EAST_WALL);
	}
	else if (ray->x_map < vars->x_pos)
	{
		return (NORTH_WALL);
	}
	return (SOUTH_WALL);
}

// TODO: draw_wall.c
int	get_draw_start_y_coordinate(int line_height)
{
	int	start;

	start = (-line_height / 2) + (WIN_HEIGHT / 2);
	if (start < 0)
	{
		return (0);
	}
	return (start);
}

// TODO: draw_wall.c
int	get_draw_end_y_coordinate(int line_height)
{
	int	end;

	end = (line_height / 2) + (WIN_HEIGHT / 2);
	if (WIN_HEIGHT <= end)
	{
		return (WIN_HEIGHT - 1);
	}
	return (end);
}

// TODO: set_draw_texture_data.c
double	get_hit_wall_x(t_draw_wall *wall, t_ray *ray, t_vars *vars)
{
	double	wall_x;

	wall_x = 0.0;
	if (wall->side == X_AXIS)
	{
		wall_x = vars->y_pos + wall->wall_dist
			* ray->y_dir;
	}
	else
	{
		wall_x = vars->x_pos + wall->wall_dist
			* ray->x_dir;
	}
	wall_x -= floor((wall_x));
	return (wall_x);
}

// TODO: set_draw_texture_data.c
int	get_x_coordinate_texture(t_draw_texture *texture, t_draw_wall *wall,
		t_ray *ray, t_vars *vars)
{
	int	x_coord_texture;

	x_coord_texture = (int)(texture->wall_x
			* (double)vars->texture_list[texture->index].width);
	if (wall->side == 0 && 0 < ray->x_dir)
	{
		x_coord_texture = vars->texture_list[texture->index].width
			- x_coord_texture - 1;
	}
	if (wall->side == 1 && ray->y_dir < 0)
	{
		x_coord_texture = vars->texture_list[texture->index].width
			- x_coord_texture - 1;
	}
	return (x_coord_texture);
}

// TODO: draw_wall.c
void	set_draw_wall_data(t_draw_wall *wall, t_ray *ray, t_info *info)
{
	wall->side = get_nearest_axis(ray, info);
	wall->wall_dist
		= get_wall_distance(ray, wall->side);
	wall->line_height = (int)(WIN_HEIGHT
			/ wall->wall_dist);
	wall->start_y = get_draw_start_y_coordinate(wall->line_height);
	wall->end_y = get_draw_end_y_coordinate(wall->line_height);
}

// TODO: set_draw_texture_data.c
void	set_draw_texture_data(t_draw_texture *texture, t_draw_wall *wall,
		t_ray *ray, t_vars *vars)
{
	texture->index = decide_draw_texture(ray, vars, wall->side);
	texture->wall_x = get_hit_wall_x(wall, ray, vars);
	texture->x_coord = get_x_coordinate_texture(texture, wall, ray, vars);
	texture->span = (1.0 * vars->texture_list[texture->index].height)
		/ wall->line_height;
	texture->current_pos = (wall->start_y - (WIN_HEIGHT / 2)
			+ (wall->line_height / 2)) * texture->span;
}

// TODO: draw_wall.c
void	put_texture(t_draw_texture *texture, t_draw_wall *wall, t_vars *vars,
		int x_coordinate_screen)
{
	unsigned int	color;
	int				y_coordinate_screen;
	int				y_coordinate_texture;

	y_coordinate_screen = wall->start_y;
	while (y_coordinate_screen < wall->end_y)
	{
		y_coordinate_texture
			= (int)texture->current_pos
			& (vars->texture_list[texture->index].height
				- 1);
		texture->current_pos += texture->span;
		color = *(vars->texture_list[texture->index].data.addr
				+ vars->texture_list[texture->index].height
				* y_coordinate_texture + texture->x_coord);
		if (wall->side == Y_AXIS)
		{
			color = (color >> 1) & 8355711;
		}
		vars->data->addr[y_coordinate_screen * WIN_WIDTH
			+ x_coordinate_screen] = color;
		y_coordinate_screen += 1;
	}
}

// TODO: draw_wall.c
int	draw_wall(t_info *info)
{
	int				x_coordinate_screen;
	t_ray			ray;
	t_draw_wall		wall;
	t_draw_texture	texture;

	x_coordinate_screen = 0;
	while (x_coordinate_screen < WIN_WIDTH)
	{
		set_ray_data(&ray, info->vars, x_coordinate_screen);
		set_draw_wall_data(&wall, &ray, info);
		set_draw_texture_data(&texture, &wall, &ray, info->vars);
		put_texture(&texture, &wall, info->vars, x_coordinate_screen);
		x_coordinate_screen += 1;
	}
	return (0);
}

// TODO: clean_image.c
void	clean_image(t_vars *vars)
{
	int	x;
	int	y_axis[2];

	x = 0;
	y_axis[0] = 0;
	y_axis[1] = WIN_HEIGHT - 1;
	while (x < WIN_WIDTH)
	{
		texture_mlx_pixel_put_line(vars, x, y_axis, 0x00000000);
		x += 1;
	}
}

// TODO: key_action.c
void	move_forward(char **map, t_vars *vars)
{
	int		one_forward_x_pos_vec;
	int		one_forward_y_pos_vec;
	char	distination;

	one_forward_x_pos_vec = vars->x_pos
		+ (vars->x_dir * MOVE_DISTANCE);
	one_forward_y_pos_vec = vars->y_pos
		+ (vars->y_dir * MOVE_DISTANCE);
	distination
		= map[one_forward_x_pos_vec][one_forward_y_pos_vec];
	if (distination == '1')
		return ;
	else
	{
		if (0 < one_forward_x_pos_vec && 0 < (int)vars->x_pos)
		{
			vars->x_pos += vars->x_dir * MOVE_DISTANCE;
		}
		if (0 < one_forward_y_pos_vec && 0 < (int)vars->y_pos)
		{
			vars->y_pos += vars->y_dir * MOVE_DISTANCE;
		}
	}
}

// printf("\x1b[33mCrashing into a wall!!!!!\x1b[0m\n");

// TODO: key_action.c
void	move_backward(char **map, t_vars *vars)
{
	int		one_backward_x_pos_vec;
	int		one_backward_y_pos_vec;
	char	distination;

	one_backward_x_pos_vec
		= vars->x_pos - (vars->x_dir * MOVE_DISTANCE);
	one_backward_y_pos_vec
		= vars->y_pos - (vars->y_dir * MOVE_DISTANCE);
	distination = map[one_backward_x_pos_vec][one_backward_y_pos_vec];
	if (distination == '1')
		return ;
	else
	{
		if (0 < one_backward_x_pos_vec && 0 < (int)vars->x_pos)
			vars->x_pos -= vars->x_dir * MOVE_DISTANCE;
		if (0 < one_backward_y_pos_vec && 0 < (int)vars->y_pos)
			vars->y_pos -= vars->y_dir * MOVE_DISTANCE;
	}
}

// TODO: key_action.c
void	rotate_right_camera(t_vars *vars)
{
	double	x_old_dir;
	double	x_old_plane;

	x_old_dir = vars->x_dir;
	x_old_plane = vars->x_cam_plane;
	vars->x_dir = vars->x_dir * cos(-MOVE_DISTANCE)
		- vars->y_dir * sin(-MOVE_DISTANCE);
	vars->y_dir = x_old_dir * sin(-MOVE_DISTANCE)
		+ vars->y_dir * cos(-MOVE_DISTANCE);
	vars->x_cam_plane = vars->x_cam_plane * cos(-MOVE_DISTANCE)
		- vars->y_cam_plane * sin(-MOVE_DISTANCE);
	vars->y_cam_plane = x_old_plane * sin(-MOVE_DISTANCE)
		+ vars->y_cam_plane * cos(-MOVE_DISTANCE);
}

// TODO: key_action.c
void	rotate_left_camera(t_vars *vars)
{
	double	x_old_dir;
	double	x_old_plane;

	x_old_dir = vars->x_dir;
	x_old_plane = vars->x_cam_plane;
	vars->x_dir = vars->x_dir * cos(MOVE_DISTANCE)
		- vars->y_dir * sin(MOVE_DISTANCE);
	vars->y_dir = x_old_dir * sin(MOVE_DISTANCE) + vars->y_dir
		* cos(MOVE_DISTANCE);
	vars->x_cam_plane = vars->x_cam_plane * cos(MOVE_DISTANCE)
		- vars->y_cam_plane * sin(MOVE_DISTANCE);
	vars->y_cam_plane = x_old_plane * sin(MOVE_DISTANCE)
		+ vars->y_cam_plane * cos(MOVE_DISTANCE);
}

// TODO: key_action.c
int	key_action(int keycode, t_info *info)
{
	if (keycode == W_KEY || keycode == UP_KEY)
		move_forward(info->map->map_data, info->vars);
	else if (keycode == S_KEY || keycode == DOWN_KEY)
		move_backward(info->map->map_data, info->vars);
	else if (keycode == D_KEY || keycode == RIGHT_KEY)
		rotate_right_camera(info->vars);
	else if (keycode == A_KEY || keycode == LEFT_KEY)
		rotate_left_camera(info->vars);
	else if (keycode == ESC_KEY)
		close_window(info);
	else if (keycode == M_KEY)
		info->flag->map *= -1;
	else 
		return (-1);
	draw_color_floor_and_ceiling(info->vars,
		info->vars->floor_col, info->vars->ceil_col);
	try_draw_texture_floor_and_ceiling(info->vars);
	draw_wall(info);
	mlx_put_image_to_window(info->vars->mlx, info->vars->win,
		info->vars->data->img, 0, 0);
	updata_pos_map(info->vars, info, keycode);
	minimap(info, info->data);
	return (0);
}

// TODO: create_xpm_textures.c
void	exit_create_texture(t_info *info,
		char *path, t_texture_data *texture)
{
	texture->data.img
		= mlx_xpm_file_to_image(info->vars->mlx, path,
			&texture->width, &texture->height);
	if (texture->data.img == NULL)
	{
		destruct_raycasting(info);
		print_error("small texture.");
	}
	texture->data.addr
		= (unsigned int *)mlx_get_data_addr(texture->data.img,
			&texture->data.bits_per_pixel,
			&texture->data.line_length,
			&texture->data.endian);
	if (texture->width != 64 || texture->height != 64)
	{
		destruct_raycasting(info);
		print_error("small texture.");
	}
}

// TODO: create_xpm_textures.c
void	create_texture_floor(t_info *info,
		char *path, t_texture_data *texture_list)
{
	if (path == NULL)
	{
		texture_list[FLOOR_1].data.img = NULL;
		texture_list[FLOOR_1].data.addr = NULL;
		texture_list[FLOOR_2].data.img = NULL;
		texture_list[FLOOR_2].data.addr = NULL;
		return ;
	}
	exit_create_texture(info, path, &texture_list[FLOOR_1]);
	exit_create_texture(info,
		"./srcs/raycasting/xpm/bluestone.xpm", &texture_list[FLOOR_2]);
}

// TODO: create_xpm_textures.c
void	create_texture_ceiling(t_info *info,
		char *path, t_texture_data *texture)
{
	if (path == NULL)
	{
		texture->data.img = NULL;
		texture->data.addr = NULL;
		return ;
	}
	exit_create_texture(info, path, texture);
}

// TODO: create_xpm_textures.c
void	initialize_texture_list(t_texture_data *texture_list)
{
	int	i;

	i = 0;
	while (i < TEXTURE_LIST_SIZE)
	{
		texture_list[i].data.img = NULL;
		i += 1;
	}
}

// TODO: create_xpm_textures.c
void	create_xpm_textures(t_texture *texture, t_info *info)
{
	initialize_texture_list(info->vars->texture_list);
	create_texture_floor(info, texture->f_tex, info->vars->texture_list);
	create_texture_ceiling(info,
		texture->c_tex, &info->vars->texture_list[CEILING]);
	exit_create_texture(info,
		texture->so, &info->vars->texture_list[SOUTH_WALL]);
	exit_create_texture(info,
		texture->no, &info->vars->texture_list[NORTH_WALL]);
	exit_create_texture(info,
		texture->ea, &info->vars->texture_list[EAST_WALL]);
	exit_create_texture(info,
		texture->we, &info->vars->texture_list[WEST_WALL]);
}

// TODO: init_nswe_dirction.c
void	set_north_player_direction(t_vars *vars)
{
	vars->x_dir = -1.0;
	vars->y_dir = 0.0;
	vars->x_cam_plane = 0.0;
	vars->y_cam_plane = 0.66;
}

// TODO: init_nswe_dirction.c
void	set_south_player_direction(t_vars *vars)
{
	vars->x_dir = 1.0;
	vars->y_dir = 0.0;
	vars->x_cam_plane = 0.0;
	vars->y_cam_plane = -0.66;
}

// TODO: init_nswe_dirction.c
void	set_east_player_direction(t_vars *vars)
{
	vars->x_dir = 0.0;
	vars->y_dir = 1.0;
	vars->x_cam_plane = 0.66;
	vars->y_cam_plane = 0.0;
}

// TODO: init_nswe_dirction.c
void	set_west_player_direction(t_vars *vars)
{
	vars->x_dir = 0.0;
	vars->y_dir = -1.0;
	vars->x_cam_plane = -0.66;
	vars->y_cam_plane = 0.0;
}

// TODO: init_nswe_dirction.c
void	init_nswe_dirction(char player_direction, t_vars *vars)
{
	if (player_direction == NORTH)
	{
		set_north_player_direction(vars);
	}
	else if (player_direction == SOUTH)
	{
		set_south_player_direction(vars);
	}
	else if (player_direction == EAST)
	{
		set_east_player_direction(vars);
	}
	else if (player_direction == WEST)
	{
		set_west_player_direction(vars);
	}
}

// TODO: exit_mlx.c
void	*exit_mlx_init(void)
{
	void	*mlx;

	mlx = mlx_init();
	if (mlx == NULL)
	{
		print_error("Failed malloc.");
	}
	return (mlx);
}

// TODO: exit_mlx.c
void	*exit_mlx_new_window(void *mlx)
{
	void	*window;

	window = mlx_new_window(mlx, WIN_WIDTH, WIN_HEIGHT, "Cub3d");
	if (window == NULL)
	{
		print_error("Failed malloc.");
	}
	return (window);
}

// TODO: exit_mlx.c
void	*exit_mlx_new_image(void *mlx)
{
	void	*image;

	image = mlx_new_image(mlx, WIN_WIDTH, WIN_HEIGHT);
	if (image == NULL)
	{
		print_error("Failed malloc.");
	}
	return (image);
}

// TODO: initialize_vars.c
unsigned int	create_rgb(int r, int g, int b)
{
	unsigned int	rgb;

	rgb = 0;
	if (r < 0 || 255 < r)
	{
		r = 0;
	}
	if (g < 0 || 255 < g)
	{
		g = 0;
	}
	if (b < 0 || 255 < b)
	{
		b = 0;
	}
	rgb |= (r & 0xFF) << 16;
	rgb |= (g & 0xFF) << 8;
	rgb |= (b & 0xFF);
	return (rgb);
}

// TODO: initialize_vars.c
void	initialize_vars(t_info *info)
{
	info->vars->mlx = exit_mlx_init();
	info->vars->win = exit_mlx_new_window(info->vars->mlx);
	info->vars->x_pos = (double)info->map->y_player + 0.500001;
	info->vars->y_pos = (double)info->map->x_player + 0.500001;
	init_nswe_dirction
		(info->map->map_data[info->map->y_player][info->map->x_player],
		info->vars);
	info->vars->floor_col
		= create_rgb(info->texture->f_rgb->red,
			info->texture->f_rgb->green, info->texture->f_rgb->blue);
	info->vars->ceil_col
		= create_rgb(info->texture->c_rgb->red,
			info->texture->c_rgb->green, info->texture->c_rgb->blue);
	info->vars->data->img = exit_mlx_new_image(info->vars->mlx);
	info->vars->data->addr
		= (unsigned int *)mlx_get_data_addr(info->vars->data->img,
			&info->vars->data->bits_per_pixel,
			&info->vars->data->line_length,
			&info->vars->data->endian);
	clean_image(info->vars);
	create_xpm_textures(info->texture, info);
	draw_color_floor_and_ceiling(info->vars,
		info->vars->floor_col, info->vars->ceil_col);
	try_draw_texture_floor_and_ceiling(info->vars);
	draw_wall(info);
}

t_info *g_info;

int mouse_action(int mousecode, t_info *info)
{
	int keycode;

	(void)info;
	if (mousecode == R_CLICK)
	{
		keycode = D_KEY;
	}
	else if (mousecode == L_CLICK)
	{
		keycode = A_KEY;
	}
	else
		return (-1);
	return (key_action(keycode, g_info));
}

// TODO: raycasting.c
void	raycasting(t_info *info)
{
	g_info = info;
	initialize_vars(info);
	mlx_put_image_to_window(info->vars->mlx, info->vars->win,
		info->vars->data->img, 0, 0);
	minimap(info, info->data);
	mlx_key_hook(info->vars->win, key_action, info);
	mlx_mouse_hook(info->vars->win, mouse_action, info);
	mlx_hook(info->vars->win, ON_DESTROY, 1L<<ON_DESTROY, close_window, info);
	mlx_loop(info->vars->mlx);
	destruct_raycasting(info);
}
