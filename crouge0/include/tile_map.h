#pragma once

#include "lib/ccurses.h"
#include "util.h"
#include "actor.h"
#include "item.h"
#include "object.h"
#include "lightmap.h"
#include "config_parser.h"
#include <stdbool.h>
#include <glib.h>
#include <libgen.h>

typedef struct Tile {
    Color color;
    char c;
    bool passable;
} Tile;

typedef struct MapTile {
    Tile tile;
    int x;
    int y;
} MapTile;

typedef struct TileMap {
    Tile *tiles;
    int width;
    int height;
    LightMap lightmap;
    GList *actors;
    Items items;
    Objects objects;
    /* location fields TODO make it sep struct with cast to basic */
    char *tiles_file;
    char *items_file;
    char *actors_file;
    char *objects_file;
} *TileMap;

typedef struct WorldMap {
    TileMap map;
    WorldMapRegionDescriptor *metadata;
} *WorldMap;

typedef struct Viewport {
    int width;
    int height;
    int cx;
    int cy;
    int display_left;
    int display_top;
} Viewport;

// TODO Viewport module
void viewport_move_left(Viewport *v, TileMap map);
void viewport_move_right(Viewport *v, TileMap map);
void viewport_move_up(Viewport *v, TileMap map);
void viewport_move_down(Viewport *v, TileMap map);
void viewport_move_leftup(Viewport *v, TileMap map);
void viewport_move_rightup(Viewport *v, TileMap map);
void viewport_move_leftdown(Viewport *v, TileMap map);
void viewport_move_rightdown(Viewport *v, TileMap map);
int viewport_top(Viewport *v);
int viewport_left(Viewport *v);
int viewport_bottom(Viewport *v);
int viewport_right(Viewport *v);
bool is_passable(TileMap map, int x, int y);

typedef void (*TileFunc)(Tile *tile, int x, int y);
typedef void (*TileDataFunc)(Tile *tile, int x, int y, void *data);

TileMap make_tile_map(int width, int height);
TileMap load_tile_map(string filename);
TileMap load_global_tmap();
void free_tile_map(TileMap map);
void free_global_map(TileMap map);

void print_tile_map(TileMap map);
string tilemap_to2d(TileMap map);
Tile *tile_at(TileMap map, int x, int y);
void draw_map(TileMap map, Viewport *v);
void foreach_tile(TileMap map, TileFunc f);
void foreach_tile_set(TileMap map, TileDataFunc f, void *data, char tile_char);

struct Actor* map_get_player(TileMap map);
void tmap_add_item(TileMap map, Item item);
char *actor_take_from(Actor actor, TileMap map, int x, int y);
void copy_map2tiles(TileMap map, char *line, int len, int offset);
