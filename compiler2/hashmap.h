#ifndef HASHMAP_H
#define HASHMAP_H

#include <stddef.h>

/* 哈希表节点 */
typedef struct HashMapNode {
    char* key;
    void* value;
    struct HashMapNode* next;
} HashMapNode;

/* 哈希表 */
typedef struct {
    HashMapNode** buckets;
    size_t size;
    size_t count;
} HashMap;

/* 函数声明 */
HashMap* hashmap_create(size_t size);
void hashmap_destroy(HashMap* map);
void hashmap_put(HashMap* map, const char* key, void* value);
void* hashmap_get(HashMap* map, const char* key);
int hashmap_contains(HashMap* map, const char* key);
void hashmap_remove(HashMap* map, const char* key);
size_t hashmap_count(HashMap* map);

#endif