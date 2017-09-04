#include "app.h"
char version[] = "0.0.1";

enum States {State_run, State_end, State_wmap, State_cursor,  NUM_STATES};
enum Events {Event_draw, Event_key,  NUM_EVENTS};
/* GLOBAL */
State state;

void process_input(G g) {
    ss_handle(state, Event_draw, g);
	int ch = getch();
    int status = 0;
	while(ch != 'q' && !status) {
        g->key = ch;
        status = ss_handle(state, Event_key, g);
        ss_handle(state, Event_draw, g);
        debug_draw(g);
		ch = getch();
	}
}

int key_run(void* data) {
    G g = data;
    char key = g->key;
    cc_printi(key, cd_green);
    switch (key) {
        case 'w':
            ss_setstate(state, State_wmap);
            break;
        case 'c':
            ss_setstate(state, State_cursor);
            break;
    }
    return 0;
}

void debug_draw(G g) {
    char buf[1024];
    // LOG
    char log_text[1024];
    log_text[0] = '\0';
    /* debuglog(g, "abc"); */
    g_debug("hi");
    g_log(NULL, G_LOG_LEVEL_DEBUG, "MSG");
    for (int i = 0; i < (g->log_len > 8 ? 8 : g->log_len); i++) {
        strcat(log_text, g_list_nth_data(g->log, i));
        strcat(log_text, "\n");
    }

    snprintf(buf, 1024, 
            "Debug: \t"
            "Key: %d\n" 
            "Log:\n%s\n" 
            "Viewport Left: %d\n" 
            "Viewport cx:cy = %d:%d\t" 
            "Viewport left:top = %d:%d\n" 
            "Player x:y = %d:%d [HP: %d  ]\t" 
            "Target [HP: %d  ]\n" 
        , g->key, log_text, 
        g->view->display_left, g->view->cx, g->view->cy,
        viewport_left(g->view), viewport_top(g->view),
        g->player->x, g->player->y, g->player->stat_hp,
        (g->last_target ? g->last_target->stat_hp : 0)
        );
    cc_printxy(buf, cn_white, 0, 20);
}

int draw(void* data) {
    UNUSED(data);
    clear();
    cc_printxy(
            "Key bindings:\n\
        w world map\n\
        h this help\n\
        c cursor mode\n", 
            cn_white, 0, 0);
    return 0;
}

// TODO mechanic Module?
void move_all_actors_rand(G g);

void move_all_actors_rand(G g) {
    GList *actor_node = g->gmap->actors;
    while (actor_node) {
        Actor actor = actor_node->data;
        // get point where actor want go
        Point next_point = actor_get_move_point(actor, g->gmap);
        // check for collide and go
        collisions_player_move(actor, next_point.x, next_point.y, g);
        actor_node = g_list_next(actor_node);
    }
}

int proc_all_actors_status(G g);
int proc_all_actors_status(G g) {
    GList *actor_node = g->gmap->actors;
    while (actor_node) {
        Actor actor = actor_node->data;
        actor_node = g_list_next(actor_node);
        if (actor->status == StatusDead) {
            // 
        }
    }
    if (g->player->status == StatusDead) {
        debuglog(g, "end");
        return 1;
    }
    return 0;
}

int wmap_key(void* data) {
    G g = data;
    char key = g->key;
    switch (key) {
        case 'h':
            ss_setstate(state, State_run);
            break;
    }
    return 0;
}

int wmap_draw(void* data) {
    G g = data;
    clear();
    cc_putxy('M', cb_yellow, 3, 2);
    print_wmap(g->wmap);
    return 0;
}

int cursor_key(void* data) {
    G g = data;
    char key = g->key;
    switch (key) {
        case 'r':
            ss_setstate(state, State_run);
            break;
        case 'j':
            g->cursor.y++;
            if (collisions_player_move(g->player, 0, 1, g)) {
                viewport_move_down(g->view, g->gmap);
            }
            break;
        case 'k':
            g->cursor.y--;
            if (collisions_player_move(g->player, 0, -1, g)) {
                viewport_move_up(g->view, g->gmap);
            }
            break;
        case 'h':
            g->cursor.x--;
            if (collisions_player_move(g->player, -1, 0, g)) {
                viewport_move_left(g->view, g->gmap);
            }
            break;
        case 'l':
            g->cursor.x++;
            if (collisions_player_move(g->player, 1, 0, g)) {
                viewport_move_right(g->view, g->gmap);
            }
            break;
        case 'y':
            if (collisions_player_move(g->player, -1, -1, g)) {
                viewport_move_leftup(g->view, g->gmap);
            }
            break;
        case 'u':
            if (collisions_player_move(g->player, 1, -1, g)) {
                viewport_move_rightup(g->view, g->gmap);
            }
            break;
        case 'm':
            if (collisions_player_move(g->player, 1, 1, g)) {
                viewport_move_rightdown(g->view, g->gmap);
            }
            break;
        case 'n':
            if (collisions_player_move(g->player, -1, 1, g)) {
                viewport_move_leftdown(g->view, g->gmap);
            }
            break;
    }
    move_all_actors_rand(g);
    return proc_all_actors_status(g);
}

int cursor_draw(void* data) {
    G g = data;

    clear();
    draw_map(g->gmap, g->view);
    /* cc_putxy('@', cn_yellow, */ 
    /*         min(g->view->cx, g->view->width / 2) + g->view->display_left, */ 
    /*         min(g->view->cy, g->view->height / 2) + g->view->display_top); */
    draw_actors(g->gmap->actors, g->view);
    draw_actor_self(g->player, g->view);
    return 0;
}

void state_init() {
    state = ss_make_state(NUM_STATES, NUM_EVENTS);
    ss_sethander(state, State_run, Event_draw, draw);
    ss_sethander(state, State_run, Event_key, key_run);
    ss_sethander(state, State_wmap, Event_draw, wmap_draw);
    ss_sethander(state, State_wmap, Event_key, wmap_key);
    ss_sethander(state, State_cursor, Event_draw, cursor_draw);
    ss_sethander(state, State_cursor, Event_key, cursor_key);
    ss_setstate(state, State_run);
}

void start() {
    G g = new_g();
    curses_init();
    state_init();
    Actor a = make_actor('o', 2, 2);
    add_actor(g, a);
    a->behavior = BehaviorSimpleAttacker;
    //a->behavior = BehaviorSimpleDirect;
    //a->behavior = BehaviorRandom;
    //a->behavior = BehaviorStand;
    a->color = cb_blue;
    a->directed.x = 0;
    a->directed.y = 0;
    for (int i = 0; i < 20; i++) {
        a = make_actor('d', i, 5);
        add_actor(g, a);
        a->color = cb_red;
        a->behavior = BehaviorSimpleAttacker;
    }
    /* for (int i = 0; i < 20; i++) { */
    /*     a = make_actor('s', i, 5); */
    /*     a->color = cb_yellow; */
    /*     add_actor(g, a); */
    /* } */

    start_events();
    event_register(ActionCollide, RolePlayer, RoleMonster, collide_action_player_monster);
    event_register(ActionCollide, RoleMonster, RolePlayer, collide_action_monster_player);
    process_input(g);
    ss_free_state(state);
    curses_end();
    end_events();
    free_g(g);
}
