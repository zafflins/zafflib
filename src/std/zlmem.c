#include <zl/zlstd.h>
#include <stdlib.h>
#include <string.h>


typedef struct ZL_MemoryHeader {
    u64 offset;
    u64 size;
} MemoryHeader;

static ptr AlignForward(u8 align, ptr memory) {
    u64 a = align - 1;
    u64 m = (u64)memory;
    return (ptr)((m + a) & ~a);
}

ptr AllocMemory(u64 size) {
    if (!size) return NULL;

    ptr r = malloc(size + sizeof(MemoryHeader) + ZL_ALIGN);
    if (!r) return NULL;

    SetMemory(size + sizeof(MemoryHeader) + ZL_ALIGN, 0, r);
    ptr a = AlignForward(ZL_ALIGN, (u8*)r + sizeof(MemoryHeader));
    MemoryHeader* h = (MemoryHeader*)((u8*)a - sizeof(MemoryHeader));

    h->offset = (u64)a - (u64)r;
    h->size = size;
    return a;
}

Result FreeMemory(ptr memory) {
    if (!memory) return ZL_ERROR;
    MemoryHeader* h = (MemoryHeader*)((u8*)memory - sizeof(MemoryHeader));
    free((ptr)((u8*)memory - h->offset));
    return ZL_OK;
}

ptr ReallocMemory(u64 size, ptr memory) {
    if (!size || !memory) return memory;

    MemoryHeader* oldH = (MemoryHeader*)((u8*)memory - sizeof(MemoryHeader));
    u64 oldOffset = oldH->offset;
    u64 oldSize = oldH->size;

    MemoryHeader* newH = realloc((ptr)((u8*)memory - oldH->offset), size + sizeof(MemoryHeader) + ZL_ALIGN);
    if (!newH) return NULL;

    ptr a = AlignForward(ZL_ALIGN, (ptr)((u8*)newH + sizeof(MemoryHeader)));
    newH->offset = (u64)a - (u64)newH;
    newH->size = size;

    // preserve data after realloc (bytes preceeding `a` are lost if offset changes)
    if (oldOffset != newH->offset) {
        MoveMemory((oldSize < size) ? oldSize : size, (ptr)((u8*)newH + oldOffset), a);
    } return a;
}

Result SetMemory(u64 size, i32 value, ptr memory) {
    if (!size || !memory) return ZL_ERROR;
    memset(memory, value, size);
    return ZL_OK;
}

Result ReadMemory(u64 size, ptr src, ptr dest) {
    if (!size || !src || !dest || src == dest) return ZL_ERROR;
    memcpy(dest, src, size);
    return ZL_OK;
}

Result MoveMemory(u64 size, ptr src, ptr dest) {
    if (!size || !src || !dest || src == dest) return ZL_ERROR;
    memmove(dest, src, size);
    return ZL_OK;
}

Result WriteMemory(u64 size, ptr src, ptr dest) {
    if (!size || !src || !dest || src == dest) return ZL_ERROR;
    memcpy(dest, src, size);
    return ZL_OK;
}

Result CompareMemory(u64 size, ptr mem1, ptr mem2) {
    if (!size || !mem1 || !mem2 || mem1 == mem2) return ZL_ERROR;
    return (!memcmp(mem1, mem2, size)) ? ZL_OK : ZL_ERROR;
}








Result DestroyArena(Arena* arena) {
    if (!arena || !arena->data) return ZL_ERROR;
    FreeMemory(arena->data);
    arena->data = NULL;
    arena->offset = 0;
    arena->marker = 0;
    arena->total = 0;
    return ZL_OK;
}

void ResetArena(u8 marker, Arena* arena) {
    if (!arena || !arena->data) return;
    if (marker) arena->marker = 0;
    arena->offset = arena->marker;
    SetMemory(arena->total - arena->marker, 0, (ptr)((u8*)arena->data + arena->offset));
}

void MarkArena(Arena* arena) {
    if (!arena || !arena->data) return;
    arena->marker = arena->offset;
}

void UnmarkArena(Arena* arena) {
    if (!arena || !arena->data) return;
    arena->marker = 0;
}

Arena CreateArena(u64 size) {
    Arena a = {0};
    if (!size) return a;

    ptr data = AllocMemory(size);
    if (!data) return a;

    a.data = data;
    a.total = size;
    return a;
}

ptr ArenaAllocMemory(u64 size, Arena* arena) {
    if (!arena || !size || size > arena->total || arena->offset + size > arena->total) return NULL;
    ptr m = (ptr)((u8*)arena->data + arena->offset);
    arena->offset += size;
    return m;
}
