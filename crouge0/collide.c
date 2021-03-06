#include "collide.h"

void collision_effect(Actor actor, Actor subject, G g) {
    // TODO LAST
    // collide by roles
    // run_action(actor, subject);
    ActionFunc f = event_get(ActionCollide, actor->role, subject->role, g);
    if (f) {
        f(ActionCollide, actor, subject, g);
    } else {
        _no_action(ActionCollide, actor, subject, g);
    }

    if (actor->role == RolePlayer) {
        /* debuglog(g, "collide from player"); */
    } else {
        /* debuglog(g, "collide from monster"); */
    }
}

/* return collided actor */
CharPoint collision_get_charpoint(Actor actor, int dx, int dy, GList *cp) {
    GList *node = cp;
    int x = actor->x + dx;
    int y = actor->y + dy;

    while (node) {
        CharPoint subject = node->data;
        // also checks for self collide
        if ((x == subject->x) && (y == subject->y) && ((CharPoint) actor != subject)) {
            return subject;
        }
        node = g_list_next(node);
    }
    return NULL;
}

/* True if can move */
bool collisions_player_move(Actor player, int dx, int dy, G g) {
    if ((dx == 0) && (dy == 0)) return false;

    Actor subject = (Actor) collision_get_charpoint(player, dx, dy, g->gmap->actors);
    bool result = false;
    if (!subject) {
        result = actor_move_hv(player, g->gmap, dx, dy);
    } else {
        collision_effect(player, subject, g);
    }

    Item item = items_get(g->gmap->items, player->x, player->y);
    if (item && player->role == RolePlayer) {
        char buf[BUFSIZE];
        char *description = item_descript(item);
        snprintf(buf, BUFSIZE, "You see %s %d:%d", description, dx, dy);
        free(description);
        debuglog(g, buf);
    }

    /* with objects */
    Object object = (Object) charpoint_at(g->gmap->objects, player->x, player->y);
    if (object) {
        debuglog(g, "collide object");
        switch (object->type) {
            case ObjectTypeTeleport: 
                debuglog(g, "Teleport!");
                break;
            default:  break;
        }
    }

    return result;
}

void tmap_actor_drop_item(G g, Actor actor) {
    int len = g_list_length(actor->items);
    if (!len) return;
    int index = rand() % len;
    /* Item item = item_clone(g_list_nth_data(actor->items, index)); */
    Item item = g_list_nth_data(actor->items, index);

    item->x = actor->x;
    item->y = actor->y;
    tmap_add_item(g->gmap, item);
    item_detach(&actor->items, item);
}

void collide_action_player_monster(Action action, Actor actor, Actor subject, G g) {
    UNUSED(actor);
    UNUSED(subject);
    UNUSED(action);
    char msg[100];

    int dmg = actor_calc_damage(actor, subject);
    snprintf(msg, 99, "%s hit %s by %dpt. remain HP:%d", actor->name, subject->name, dmg, subject->stat_hp);
    debuglog(g, msg);
    /* mechanic */
    g->last_target = subject;
    subject->stat_hp -= dmg;
    if (subject->stat_hp <= 0) {
        long gain = actor_defeat(actor, subject);
        snprintf(msg, 99, "%s kill %s and gain %ld exp", actor->name, subject->name, gain);
        debuglog(g, msg);
        g->last_target = NULL;
        //actor_kill(subject, g); TODO
        subject->status = StatusDead;
        g->gmap->actors = g_list_remove(g->gmap->actors, subject);
        tmap_actor_drop_item(g, subject);
        actor_free(subject); // strange behavior of g_list_remove (its free data)
    }

}

void collide_action_monster_player(Action action, Actor actor, Actor subject, G g) {
    UNUSED(actor);
    UNUSED(subject);
    UNUSED(action);
    char msg[100];
    /* debuglog(g, "! monster hit player"); */

    int dmg = actor_calc_damage(actor, subject);
    subject->stat_hp -= dmg;

    snprintf(msg, 99, "%s hit you by %dpt", actor->name, dmg);
    debuglog(g, msg);

    if (subject->stat_hp <= 0) {
        snprintf(msg, 99, "%s kill you.", actor->name);
        debuglog(g, msg);
        subject->status = StatusDead;
    }
}
