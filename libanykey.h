#ifndef LIBANYKEY_H
#define LIBANYKEY_H

#ifdef __cplusplus
extern "C" {
#endif

typedef struct anykey anykey;

anykey*anykey_delete(anykey*anykey);
anykey*anykey_create(void);
int anykey_canget(anykey*anykey);
char* anykey_getkey(anykey*anykey);

#ifdef __cplusplus
}
#endif

#endif
