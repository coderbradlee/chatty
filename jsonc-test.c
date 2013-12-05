#include <stdlib.h>
#include <stdio.h>
#include <json-c/json.h>

int main(int argc, const char *argv[])
{
    struct json_object* jo1 = json_object_new_array();
    struct json_object* jo2 = json_object_new_object();
    struct json_object* ro = json_object_new_int64(10);
    json_object_array_add(jo1, ro);
    json_object_object_add(jo2, "key2", jo1);
    printf("%s\n", json_object_to_json_string(jo2));
    json_object_put(jo2);

    jo2 = json_tokener_parse("{\"key\": 13}");
    printf("%s\n", json_object_to_json_string(jo2));

    json_object_put(jo2);
    return 0;
}
