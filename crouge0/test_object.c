#include "object.h"
#include "config_parser.h"
#include <stdio.h>
#include <assert.h>

char *tst_str = "1:2:_cn_blue:$:hello stash:ObjectTypeStash:\n";
int object_fields_count = 10;

void test_create() {
    Object o = object_new(1, 2, 'z');
    object_print(o);
    object_free(o);
}

void test_serialization() {
    Object o = object_new(1, 2, 'z');
    char *x = object_serialize(o);
    printf("dump:%s\n", x);
    free(x);
    object_free(o);

    char *test = strdup(tst_str);
    Strings s = parse_dsv_line(test, object_fields_count);
    Object z = object_deserialize(s);
    object_print(z);
    object_free(z);
    free(test);
    free_dsv_strings(s);

    Objects objs = NULL;
    Object obj = object_new(1, 2, 'z');
    object_add(&objs, obj);
    obj = object_new(1, 2, 'a');
    object_add(&objs, obj);
    objects_save("/tmp/objs", objs);
    objects_free(&objs);

    objs = objects_load("/tmp/objs");
    GList *it = objs;
    while (it) {
        object_print(it->data);
        it = g_list_next(it);
    }
    objects_free(&objs);
}

int main() {
    test_create();
    test_serialization();
}
