#ifndef __TOOLS_H__
#define __TOOLS_H__

static inline unsigned char gen8(int *a) {
    unsigned char res = 0;
    for (int i = 0; i < 8; i++) {
        if (a[i] == 1) {
            res += 1 << i;
        }
    }
    return res;
}

static inline unsigned short gen16(int *a) {
    unsigned short res = 0;
    for (int i = 0; i < 16; i++) {
        if (a[i] == 1) {
            res += 1 << i;
        }
    }
    return res;
}

static inline unsigned int gen32(int *a) {
    unsigned int res = 0;
    for (int i = 0; i < 32; i++) {
        if (a[i] == 1) {
            res += 1 << i;
        }
    }
    return res;
}

#endif /*__DESC_H__*/
