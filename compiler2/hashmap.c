#include "hashmap.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

/* 哈希函数 */
static unsigned long hash(const char* str) {
    unsigned long hash = 5381;
    int c;
    
    while ((c = *str++)) {
        hash = ((hash << 5) + hash) + c; /* hash * 33 + c */
    }
    
    return hash;
}

/* 创建哈希表 */
HashMap* hashmap_create(size_t size) {
    HashMap* map = malloc(sizeof(HashMap));
    if (!map) return NULL;
    
    map->buckets = calloc(size, sizeof(HashMapNode*));
    if (!map->buckets) {
        free(map);
        return NULL;
    }
    
    map->size = size;
    map->count = 0;
    return map;
}

/* 销毁哈希表 */
void hashmap_destroy(HashMap* map) {
    if (!map) return;
    
    for (size_t i = 0; i < map->size; i++) {
        HashMapNode* node = map->buckets[i];
        while (node) {
            HashMapNode* next = node->next;
            free(node->key);
            free(node);
            node = next;
        }
    }
    
    free(map->buckets);
    free(map);
}

/* 插入键值对 */
void hashmap_put(HashMap* map, const char* key, void* value) {
    if (!map || !key) return;
    
    unsigned long index = hash(key) % map->size;
    HashMapNode* node = map->buckets[index];
    
    /* 检查是否已存在 */
    while (node) {
        if (strcmp(node->key, key) == 0) {
            node->value = value;
            return;
        }
        node = node->next;
    }
    
    /* 创建新节点 */
    node = malloc(sizeof(HashMapNode));
    if (!node) return;
    
    node->key = strdup(key);
    node->value = value;
    node->next = map->buckets[index];
    map->buckets[index] = node;
    map->count++;
}

/* 获取值 */
void* hashmap_get(HashMap* map, const char* key) {
    if (!map || !key) return NULL;
    
    unsigned long index = hash(key) % map->size;
    HashMapNode* node = map->buckets[index];
    
    while (node) {
        if (strcmp(node->key, key) == 0) {
            return node->value;
        }
        node = node->next;
    }
    
    return NULL;
}

/* 检查键是否存在 */
int hashmap_contains(HashMap* map, const char* key) {
    return hashmap_get(map, key) != NULL;
}

/* 删除键值对 */
void hashmap_remove(HashMap* map, const char* key) {
    if (!map || !key) return;
    
    unsigned long index = hash(key) % map->size;
    HashMapNode* node = map->buckets[index];
    HashMapNode* prev = NULL;
    
    while (node) {
        if (strcmp(node->key, key) == 0) {
            if (prev) {
                prev->next = node->next;
            } else {
                map->buckets[index] = node->next;
            }
            
            free(node->key);
            free(node);
            map->count--;
            return;
        }
        
        prev = node;
        node = node->next;
    }
}

/* 获取元素数量 */
size_t hashmap_count(HashMap* map) {
    return map ? map->count : 0;
}