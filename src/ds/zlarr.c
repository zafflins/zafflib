#include <zl/zlds.h>

typedef struct ArrayHeader {
    u32 used;
    u32 slots;
    u64 stride;
} ZL_ArrayHeader;

ptr CreateArray(u32 slots, u32 stride) {
    if (!slots || !stride) return NULL;

    ZL_ArrayHeader* h = AllocMemory(sizeof(ZL_ArrayHeader) + (slots * stride));
    if (!h) return NULL;

    SetMemory(sizeof(ZL_ArrayHeader) + (slots * stride), 0, h);
    h->stride = stride;
    h->slots = slots;

    return (ptr)((u8*)h + sizeof(ZL_ArrayHeader));
}

ptr ResizeArray(u32 slots, ptr arr) {
    if (!slots) return arr;

    ZL_ArrayHeader* h = ((ZL_ArrayHeader*)((u8*)arr - sizeof(ZL_ArrayHeader)));
    ptr new = CreateArray(slots, h->stride);
    if (!new) return arr;

    ZL_ArrayHeader* nh = ((ZL_ArrayHeader*)((u8*)new - sizeof(ZL_ArrayHeader)));
    nh->used = h->used;

    WriteMemory(h->used * h->stride, arr, new);
    DestroyArray(arr);
    return new;
}

void DestroyArray(ptr arr) {
    if (!arr) return;
    FreeMemory((ptr)((u8*)arr - sizeof(ZL_ArrayHeader)));
}

ZL_ArrayHeader ZL_DescribeArray(ptr arr) {
    if (!arr) return (ZL_ArrayHeader){0};
    return *(ZL_ArrayHeader*)((u8*)arr - sizeof(ZL_ArrayHeader));
}


u32 ArrayUsed(ptr arr) {
    if (!arr) return 0;
    return ((ZL_ArrayHeader*)((u8*)arr - sizeof(ZL_ArrayHeader)))->used;
}

u32 ArraySlots(ptr arr) {
    if (!arr) return 0;
    return ((ZL_ArrayHeader*)((u8*)arr - sizeof(ZL_ArrayHeader)))->slots;
}

u8 ArrayFull(ptr arr) {
    if (!arr) return 0;
    ZL_ArrayHeader *h = ((ZL_ArrayHeader*)((u8*)arr - sizeof(ZL_ArrayHeader)));
    return h->used == h->slots;
}


void SetArray(u32 slot, ptr value, ptr arr) {
    if (!arr) return;

    ZL_ArrayHeader* head = (ZL_ArrayHeader*)((u8*)arr - sizeof(ZL_ArrayHeader));
    if (slot >= head->slots) return;
    if (slot >= head->used) {
        if (head->used + 1 > head->slots) return;
        head->used++;
    } WriteMemory(head->stride, value, (ptr)((u8*)arr + (slot * head->stride)));
}

void ClearArray(ptr arr) {
    if (!arr) return;
    ZL_ArrayHeader* head = (ZL_ArrayHeader*)((u8*)arr - sizeof(ZL_ArrayHeader));
    SetMemory(head->stride * head->slots, 0, arr);
    head->used = 0;
}

ptr GetArray(u32 slot, ptr arr) {
    if (!arr) return NULL;

    ZL_ArrayHeader* head = (ZL_ArrayHeader*)((u8*)arr - sizeof(ZL_ArrayHeader));
    if (slot > head->used) return NULL;

    return (ptr)((u8*)arr + (slot * head->stride));
}

ptr SliceArray(u32 src, u32 end, ptr arr) {
    if (src == end || !arr) return NULL;

    ZL_ArrayHeader* head = (ZL_ArrayHeader*)((u8*)arr - sizeof(ZL_ArrayHeader));
    if (src >= head->slots || end >= head->slots) return NULL;
    if (end < src) SWAP(u32, src, end);

    ptr out = CreateArray(end - src, head->stride);
    if (!out) return NULL;

    WriteMemory(head->stride * (end - src), (u8*)arr + (src * head->stride), out);
    return out;
}

void RemArray(u32 slot, ptr arr) {
    if (!arr) return;

    ZL_ArrayHeader* head = (ZL_ArrayHeader*)((u8*)arr - sizeof(ZL_ArrayHeader));
    if (!head->used || slot >= head->used) return;

    SetMemory(head->stride, 0, (ptr)((u8*)arr + (slot * head->stride)));
}

void PushArray(ptr value, ptr arr) {
    if (!arr) return;

    ZL_ArrayHeader* head = (ZL_ArrayHeader*)((u8*)arr - sizeof(ZL_ArrayHeader));
    if (head->used + 1 > head->slots) return;

    WriteMemory(head->stride, value, (ptr)((u8*)arr + (head->used++ * head->stride)));
}

void PullArray(u32 slot, ptr value, ptr arr) {
    if (!arr) return;

    ZL_ArrayHeader* head = (ZL_ArrayHeader*)((u8*)arr - sizeof(ZL_ArrayHeader));
    if (!head->used || slot >= head->used) return;

    ptr last = (ptr)((u8*)arr + (head->used * head->stride));
    ptr pull = (ptr)((u8*)arr + (slot * head->stride));
    ptr put = (ptr)((u8*)pull + head->stride);
    u32 move = head->used-- - (slot + 1);

    WriteMemory(head->stride, pull, value);
    WriteMemory(move * head->stride, put, pull);
    SetMemory(head->stride, 0, last);
}

void PopArray(ptr value, ptr arr) {
    if (!arr) return;

    ZL_ArrayHeader* head = (ZL_ArrayHeader*)((u8*)arr - sizeof(ZL_ArrayHeader));
    if (!head->used) return;

    ptr end = (ptr)((u8*)arr + ((--head->used) * head->stride));
    u64 stride = head->stride;

    WriteMemory(stride, end, value);
    SetMemory(stride, 0, end);
}

void PopSwapArray(u32 slot, ptr arr) {
    if (!arr) return;

    ZL_ArrayHeader* head = (ZL_ArrayHeader*)((u8*)arr - sizeof(ZL_ArrayHeader));
    if (!head->used || slot >= head->used) return;

    ptr end = (ptr)((u8*)arr + ((--head->used) * head->stride));
    ptr pop = (ptr)((u8*)arr + (slot * head->stride));
    u64 stride = head->stride;

    WriteMemory(stride, end, pop);
    SetMemory(stride, 0, end);
}
