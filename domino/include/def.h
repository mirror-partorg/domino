#ifndef _DEF_H
#define _DEF_H

#ifndef PACK_STRUCT
#ifdef _LINUX
#define PACK_STRUCT __attribute__((packed))
#else
#define PACK_STRUCT __attribute__((gcc_struct, packed))
#endif
#endif

#endif
