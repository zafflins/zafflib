#include <zl/zlds.h>

typedef struct Pool {
    u32 next;
    u32 dead;
    u32 slots;
    u32 stride;

    ptr data;
    u32* ver;
    u32* free;
} Pool;

static inline u64 _NewHandle(u32 ver, u32 index) {
    return ((u64)ver << 32) | index;
}

static inline u32 _HandleVer(u64 handle) {
    return (u32)(handle >> 32);
}

static inline u32 _HandleIdx(u64 handle) {
    return (u32)(handle & 0xFFFFFFFF);
}

ptr CreatePool(u32 slots, u32 stride) {
    if (!slots || !stride) return NULL;

    Pool* p = AllocMemory(sizeof(Pool) + ((slots * sizeof(u32)) * 2) + (slots * stride));
    if (!p) return NULL;

    p->slots = slots;
    p->stride = stride;
    p->ver = (u32*)((u8*)p + sizeof(Pool));
    p->free = (u32*)((u8*)p->ver + slots * sizeof(u32));
    p->data = (u8*)p->free + slots * sizeof(u32);

    FOR_I(0, slots, 1) p->ver[i] = 1;
    return p;
}

u64 PoolAllocMemory(ptr data, ptr pool) {
    if (!pool) return ZL_INVALID_HANDLE;

    Pool* p = (Pool*)pool;
    if (!p->dead && p->next >=  p->slots) return ZL_INVALID_HANDLE;

    u32 idx = 0;
    if (p->dead) {
        idx = p->free[--p->dead];
    } else {
        idx = p->next++;
    } u32 ver = p->ver[idx];

    if (data) {
        ptr dst = (u8*)p->data + idx * p->stride;
        WriteMemory(p->stride, data, dst);
    } return _NewHandle(ver, idx);
}

void PoolFreeMemory(u64 handle, ptr pool) {
    if (!handle || handle == ZL_INVALID_HANDLE || !pool) return;

    Pool* p = (Pool*)pool;
    u32 idx = _HandleIdx(handle);
    u32 ver = _HandleVer(handle);
    if (idx >= p->next || ver != p->ver[idx]) return;

    p->free[p->dead++] = idx;
    p->ver[idx]++;

    ptr dst = (u8*)p->data + idx * p->stride;
    SetMemory(p->stride, 0, dst);
}

void DestroyPool(ptr pool) {
    FreeMemory(pool);
}


void SetPoolData(u64 handle, ptr data, ptr pool) {
    if (!handle || handle == ZL_INVALID_HANDLE || !pool) return;

    Pool* p = (Pool*)pool;
    u32 idx = _HandleIdx(handle);
    u32 ver = _HandleVer(handle);
    if (idx >= p->next || ver != p->ver[idx]) return;

    ptr dst = (u8*)p->data + idx * p->stride;
    if (!data) SetMemory(p->stride, 0, dst);
    else WriteMemory(p->stride, data, dst);
}

u32 GetPoolVersion(u64 handle, ptr pool) {
    return _HandleVer(handle);
}

u32 GetPoolIndex(u64 handle, ptr pool) {
    return _HandleIdx(handle);
}

ptr GetPoolData(u64 handle, ptr pool) {
    if (!handle || handle == ZL_INVALID_HANDLE || !pool) return NULL;

    Pool* p = (Pool*)pool;
    u32 idx = _HandleIdx(handle);
    u32 ver = _HandleVer(handle);
    if (idx >= p->next || ver != p->ver[idx]) return NULL;

    return (u8*)p->data + idx * p->stride;
}
