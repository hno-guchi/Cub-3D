/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: susasaki <susasaki@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/04/28 18:50:28 by susasaki          #+#    #+#             */
/*   Updated: 2023/06/24 15:31:05 by susasaki         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# include "../include/cub3d.h"

void init_map(t_map *map)
{
    map->map_data = NULL;
    map->height = 0;
    map->width = 0;
}

void init(t_info *info,t_map *map, t_vars *vars)
{
    info->map = map;
    info->vars = vars;
    init_map(info->map);
}