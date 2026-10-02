#include <zl/zlio.h>

u64 StrLen(char* str) {
    if (!str) return 0;
    u64 l = 0;
    while(*str) {
        str++;
        l++;
    } return l;
}

u8 ChrInStr(char c, char* str) {
    if (!str) return 0;
    while (*str) {
        if (c == *str) return 1;
        str++;
    } return 0;
}

u8 StrInStr(char* sub, char* str) {
    if (!str || !sub) return 0;
    u64 len = StrLen(sub);
    FOR_I(0, StrLen(str), 1) {
        if (StrIsStr(len, sub, str + i)) {
            return 1;
        }
    } return 0;
}

u8 StrIsStr(u64 len, char* a, char* b) {
    if (!len || !a || !b) return 0;
    FOR_I(0, len, 1) {
        if (a[i] != b[i]) return 0;
    } return 1;
}

u8 PrefixInStr(char* str, char* prefix) {
    if (!str || !prefix) return 0;
    while (*prefix) {
        if (*str++ != *prefix++) return 0;
    } return 1;
}
