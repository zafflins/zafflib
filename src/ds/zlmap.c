#include <string.h>
#include <zl/zlds.h>
#include <zl/zlio.h>


typedef struct MapEntry {
    char* key;
    ptr value;
} MapEntry;

typedef struct MapHeader {
    u32 used;
    u32 slots;
    u32 stride;
} MapHeader;

static inline u32 HashKey(const char* k) {
	if (!k) return ((1ULL << 32) - 1);
	u32 h = 2166136261u; 	// FNV-1a offset basis
	while (*k) {
		h ^= (char)*k++;	// XOR with current character
		h *= 16777619;		// multiply by FNV-1a prime
	}; return h;
}

static inline u32 ProbeSet(char* key, ptr map, u32 max) {
    u32 s = HashKey(key) % max;
    u32 h = s;
    FOR_I(0, max, 1) {
        MapEntry* e = &((MapEntry*)map)[s];
        if (!e->key || (e->key && StrIsStr(StrLen(key), key, e->key))) return s;

        s = (s + 1) % max;
        if (s == h) return 0xFFFFFFFF;
    } return 0xFFFFFFFF;
}

static inline u32 ProbeGet(char* key, ptr map, u32 max) {
    u32 s = HashKey(key) % max;
    u32 h = s;
    FOR_I(0, max, 1) {
        MapEntry* e = &((MapEntry*)map)[s];
        if (e->key && StrIsStr(StrLen(key), key, e->key)) return s;

        s = (s + 1) % max;
        if (s == h) return 0xFFFFFFFF;
    } return 0xFFFFFFFF;
}


ptr CreateMap(u32 slots, u32 stride) {
    if (!slots || !stride) return NULL;

    MapHeader* h = AllocMemory(sizeof(MapHeader) + (slots * sizeof(MapEntry)));
    if (!h) return NULL;

    h->slots = slots;
    h->stride = stride;
    return  ((u8*)h + sizeof(MapHeader));
}

ptr ResizeMap(u32 slots, ptr map) {
    if (!slots || !map) return map;

    MapHeader* h = (MapHeader*)((u8*)map - sizeof(MapHeader));
    ptr new = CreateMap(slots, h->stride);
    if (!new) return map;

    FOR_I(0, h->slots, 1) {
        MapEntry* entry = &((MapEntry*)map)[i];
        if (entry->key) SetMap(entry->key, entry->value, new);
    } return new;
}

void DestroyMap(ptr map) {
    if (!map) return;
    MapHeader* h = (MapHeader*)((u8*)map - sizeof(MapHeader));
    FOR_I(0, 1, h->slots) FreeMemory(((MapEntry*)map)[i].value);
    FreeMemory(h);
}

void SetMap(char* key, ptr value, ptr map) {
    if (!key || !value || !map) return;

    MapHeader* h = (MapHeader*)((u8*)map - sizeof(MapHeader));

    u32 slot = ProbeSet(key, map, h->slots);
    if (slot >= h->slots) return;

    MapEntry* entry = &((MapEntry*)map)[slot];
    if (!entry->key) {
        entry->key = AllocMemory(StrLen(key));
        entry->value = AllocMemory(h->stride);
        if (!entry->key || !entry->value) {
            FreeMemory(entry->value);
            FreeMemory(entry->key);
            *entry = (MapEntry){0};
            return;
        }
    }

    h->used += (slot >= h->used && h->used + 1 < h->slots) ? 1 : 0;
    WriteMemory(h->stride, value, entry->value);
    WriteMemory(StrLen(key), key, entry->key);
}

ptr GetMap(char* key, ptr map) {
    if (!key || !map) return NULL;

    MapHeader* h = (MapHeader*)((u8*)map - sizeof(MapHeader));

    u32 slot = ProbeGet(key, map, h->slots);
    if (slot >= h->slots) return NULL;

    return ((MapEntry*)map)[slot].value;
}

void RemMap(char* key, ptr map) {
    if (!key || !map) return;

    MapHeader* h = (MapHeader*)((u8*)map - sizeof(MapHeader));

    u32 slot = ProbeGet(key, map, h->slots);
    if (slot >= h->slots) return;

    MapEntry* entry = &((MapEntry*)map)[slot];
    FreeMemory(entry->value);
    FreeMemory(entry->key);
    *entry = (MapEntry){0};
}


void IterMapValues(void (*fn)(ptr), ptr map) {
    if (!fn || !map) return;
    MapHeader* h = (MapHeader*)((u8*)map - sizeof(MapHeader));
    FOR_I(0, h->slots, 1) {
        MapEntry* e = &((MapEntry*)map)[i];
        if (!e->key) continue;
        printf("ITER KEY: %s\n", e->key);
        fn(e->value);
        printf("ITERATED KEY: %s\n", e->key);
    }
}

void IterMapKeys(void (*fn)(char*), ptr map) {
    if (!fn || !map) return;
    MapHeader* h = (MapHeader*)((u8*)map - sizeof(MapHeader));
    FOR_I(0, h->slots, 1) {
        MapEntry* e = &((MapEntry*)map)[i];
        if (!e->key) continue;
        fn(e->key);
    }
}
