#include <zl/zlio.h>


FileBuffer CreateFile(u64 size) {
    FileBuffer fb = {0};
    if (!size) return fb;

    fb.data = AllocMemory(size);
    if (!fb.data) return fb;

    fb.size = size;
    return fb;
}

void CloseFile(FileBuffer* fb) {
    if (!fb || !fb->data) return;
    FreeMemory(fb->data);
    *fb = (FileBuffer){0};
}

void ZLWriteFile(u64 size, ptr data, FileBuffer* fb) {
    if (!size || !fb || !fb->data) return;
    if (((u64)fb->cursor + size) - (u64)fb->data > fb->size) return;

    WriteMemory(size, data, fb->cursor);
    fb->cursor += size;
}

void ReadFile(u64 size, ptr data, FileBuffer* fb) {
    if (!size || !fb || !fb->data) return;
    if (((u64)fb->cursor + size) - (u64)fb->data > fb->size) return;

    WriteMemory(size, fb->cursor, data);
    fb->cursor += size;
}

void SaveFile(char* path, FileBuffer* fb) {
    if (!path || !fb || !fb->data) return;

    FILE* io = fopen((const char*)path, "wb");
    if (!io) return;

    fwrite(fb->data, 1, (u64)fb->cursor - (u64)fb->data, io);
    fclose(io);
}

FileBuffer LoadFile(char* path) {
    FileBuffer fb = {0};
    if (!path) return fb;

    ptr f = fopen(path, "rb");
    if (!f) return fb;

    fseek(f, 0, SEEK_END);
    fb.size = ftell(f);
    fseek(f, 0, SEEK_SET);

    ptr d = AllocMemory(fb.size + 1);
    if (!d) return fb;

    fread(d, 1, fb.size, f);
    fclose(f);

    fb.data = d;
    fb.cursor = d;
    fb.data[fb.size] = '\0';

    return fb;
}

u64 SumFileLines(FileBuffer* fb) {
    if (!fb || !fb->data) return 0;
    u64 n = 0;
    char* c = fb->data;
    while(*c) {
        if (*c == '\n') n++;
        c++;
    } return n;
}
