#include "inventory.h"

extern State state;

int inventory_key(void* data) {
    G g = data;
    char key = g->key;

    switch (key) {
        case 'i':
            ss_setstate(state, State_cursor);
            break;
        case '?':
            break;
        default:
            break;
    }
    return 0;
}

int inventory_draw(void* data) {
    G g = data;
    UNUSED(g);

    clear();
    cc_printxy("Inventory:", cn_white, 0, 0);
    char buf[BUFSIZE];
    GList *it = g->player->items;
    int y = 0;
    while (it) {
        Item item = it->data;
        // wtf with % sign?
        char *descriptoin = item_descript(item);
        snprintf(buf, BUFSIZE, "%c) %c - %s", (y + 'a'), item->c, descriptoin);
        free(descriptoin);
    debuglog(g, buf);
        cc_printxy(buf, cn_white, 2, ++y);
        it = g_list_next(it);
    }

    return 0;
}
