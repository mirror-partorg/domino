

#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <sys/types.h>		/* for stupid systems */

#include "../../config/config.h"
#include "../../misc/define.h"
#include <dolib.h>

typedef unsigned char md5byte;
typedef unsigned int UWORD32;

static void MD5Transform(UWORD32 buf[4], UWORD32 const in[16]);

struct MD5Context {
	UWORD32 buf[4];
	UWORD32 bytes[2];
	UWORD32 in[16];
};

static void byteSwap(UWORD32 *buf, unsigned words)
{
        const UWORD32 byteOrderTest = 0x1;
        if (((char *)&byteOrderTest)[0] == 0) {
            md5byte *p = (md5byte *)buf;

            do {
                *buf++ = (UWORD32)((unsigned)p[3] << 8 | p[2]) << 16 |
                    ((unsigned)p[1] << 8 | p[0]);
                p += 4;
            } while (--words);
        }
}

/*
 * Start MD5 accumulation.  Set bit count to 0 and buffer to mysterious
 * initialization constants.
 */
static void  MD5Init(struct MD5Context *ctx)
{
	ctx->buf[0] = 0x67452301;
	ctx->buf[1] = 0xefcdab89;
	ctx->buf[2] = 0x98badcfe;
	ctx->buf[3] = 0x10325476;

	ctx->bytes[0] = 0;
	ctx->bytes[1] = 0;
}

/*
 * Update context to reflect the concatenation of another buffer full
 * of bytes.
 */
static void MD5Update(struct MD5Context *ctx, md5byte const *buf, unsigned len)
{
	UWORD32 t;

	/* Update byte count */

	t = ctx->bytes[0];
	if ((ctx->bytes[0] = t + len) < t)
		ctx->bytes[1]++;	/* Carry from low to high */

	t = 64 - (t & 0x3f);	/* Space available in ctx->in (at least 1) */
	if (t > len) {
		memcpy((md5byte *)ctx->in + 64 - t, buf, len);
		return;
	}
	/* First chunk is an odd size */
	memcpy((md5byte *)ctx->in + 64 - t, buf, t);
	byteSwap(ctx->in, 16);
	MD5Transform(ctx->buf, ctx->in);
	buf += t;
	len -= t;

	/* Process data in 64-byte chunks */
	while (len >= 64) {
		memcpy(ctx->in, buf, 64);
		byteSwap(ctx->in, 16);
		MD5Transform(ctx->buf, ctx->in);
		buf += 64;
		len -= 64;
	}

	/* Handle any remaining bytes of data. */
	memcpy(ctx->in, buf, len);
}

/*
 * Final wrapup - pad to 64-byte boundary with the bit pattern
 * 1 0* (64-bit count of bits processed, MSB-first)
 */
static void MD5Final(struct MD5Context *ctx, md5byte *digest)
{
	int count = ctx->bytes[0] & 0x3f;	/* Number of bytes in ctx->in */
	md5byte *p = (md5byte *)ctx->in + count;

	/* Set the first char of padding to 0x80.  There is always room. */
	*p++ = 0x80;

	/* Bytes of padding needed to make 56 bytes (-8..55) */
	count = 56 - 1 - count;

	if (count < 0) {	/* Padding forces an extra block */
		memset(p, 0, count + 8);
		byteSwap(ctx->in, 16);
		MD5Transform(ctx->buf, ctx->in);
		p = (md5byte *)ctx->in;
		count = 56;
	}
	memset(p, 0, count);
	byteSwap(ctx->in, 14);

	/* Append length in bits and transform */
	ctx->in[14] = ctx->bytes[0] << 3;
	ctx->in[15] = ctx->bytes[1] << 3 | ctx->bytes[0] >> 29;
	MD5Transform(ctx->buf, ctx->in);

	byteSwap(ctx->buf, 4);
	memcpy(digest, ctx->buf, 16);
	memset(ctx, 0, sizeof(ctx));	/* In case it's sensitive */
}

#ifndef ASM_MD5

/* The four core functions - F1 is optimized somewhat */

/* #define F1(x, y, z) (x & y | ~x & z) */
#define F1(x, y, z) (z ^ (x & (y ^ z)))
#define F2(x, y, z) F1(z, x, y)
#define F3(x, y, z) (x ^ y ^ z)
#define F4(x, y, z) (y ^ (x | ~z))

/* This is the central step in the MD5 algorithm. */
#define MD5STEP(f,w,x,y,z,in,s) \
	 (w += f(x,y,z) + in, w = (w<<s | w>>(32-s)) + x)

/*
 * The core of the MD5 algorithm, this alters an existing MD5 hash to
 * reflect the addition of 16 longwords of new data.  MD5Update blocks
 * the data and converts bytes into longwords for this routine.
 */
static void MD5Transform(UWORD32 buf[4], UWORD32 const in[16])
{
	register UWORD32 a, b, c, d;

	a = buf[0];
	b = buf[1];
	c = buf[2];
	d = buf[3];

	MD5STEP(F1, a, b, c, d, in[0] + 0xd76aa478, 7);
	MD5STEP(F1, d, a, b, c, in[1] + 0xe8c7b756, 12);
	MD5STEP(F1, c, d, a, b, in[2] + 0x242070db, 17);
	MD5STEP(F1, b, c, d, a, in[3] + 0xc1bdceee, 22);
	MD5STEP(F1, a, b, c, d, in[4] + 0xf57c0faf, 7);
	MD5STEP(F1, d, a, b, c, in[5] + 0x4787c62a, 12);
	MD5STEP(F1, c, d, a, b, in[6] + 0xa8304613, 17);
	MD5STEP(F1, b, c, d, a, in[7] + 0xfd469501, 22);
	MD5STEP(F1, a, b, c, d, in[8] + 0x698098d8, 7);
	MD5STEP(F1, d, a, b, c, in[9] + 0x8b44f7af, 12);
	MD5STEP(F1, c, d, a, b, in[10] + 0xffff5bb1, 17);
	MD5STEP(F1, b, c, d, a, in[11] + 0x895cd7be, 22);
	MD5STEP(F1, a, b, c, d, in[12] + 0x6b901122, 7);
	MD5STEP(F1, d, a, b, c, in[13] + 0xfd987193, 12);
	MD5STEP(F1, c, d, a, b, in[14] + 0xa679438e, 17);
	MD5STEP(F1, b, c, d, a, in[15] + 0x49b40821, 22);

	MD5STEP(F2, a, b, c, d, in[1] + 0xf61e2562, 5);
	MD5STEP(F2, d, a, b, c, in[6] + 0xc040b340, 9);
	MD5STEP(F2, c, d, a, b, in[11] + 0x265e5a51, 14);
	MD5STEP(F2, b, c, d, a, in[0] + 0xe9b6c7aa, 20);
	MD5STEP(F2, a, b, c, d, in[5] + 0xd62f105d, 5);
	MD5STEP(F2, d, a, b, c, in[10] + 0x02441453, 9);
	MD5STEP(F2, c, d, a, b, in[15] + 0xd8a1e681, 14);
	MD5STEP(F2, b, c, d, a, in[4] + 0xe7d3fbc8, 20);
	MD5STEP(F2, a, b, c, d, in[9] + 0x21e1cde6, 5);
	MD5STEP(F2, d, a, b, c, in[14] + 0xc33707d6, 9);
	MD5STEP(F2, c, d, a, b, in[3] + 0xf4d50d87, 14);
	MD5STEP(F2, b, c, d, a, in[8] + 0x455a14ed, 20);
	MD5STEP(F2, a, b, c, d, in[13] + 0xa9e3e905, 5);
	MD5STEP(F2, d, a, b, c, in[2] + 0xfcefa3f8, 9);
	MD5STEP(F2, c, d, a, b, in[7] + 0x676f02d9, 14);
	MD5STEP(F2, b, c, d, a, in[12] + 0x8d2a4c8a, 20);

	MD5STEP(F3, a, b, c, d, in[5] + 0xfffa3942, 4);
	MD5STEP(F3, d, a, b, c, in[8] + 0x8771f681, 11);
	MD5STEP(F3, c, d, a, b, in[11] + 0x6d9d6122, 16);
	MD5STEP(F3, b, c, d, a, in[14] + 0xfde5380c, 23);
	MD5STEP(F3, a, b, c, d, in[1] + 0xa4beea44, 4);
	MD5STEP(F3, d, a, b, c, in[4] + 0x4bdecfa9, 11);
	MD5STEP(F3, c, d, a, b, in[7] + 0xf6bb4b60, 16);
	MD5STEP(F3, b, c, d, a, in[10] + 0xbebfbc70, 23);
	MD5STEP(F3, a, b, c, d, in[13] + 0x289b7ec6, 4);
	MD5STEP(F3, d, a, b, c, in[0] + 0xeaa127fa, 11);
	MD5STEP(F3, c, d, a, b, in[3] + 0xd4ef3085, 16);
	MD5STEP(F3, b, c, d, a, in[6] + 0x04881d05, 23);
	MD5STEP(F3, a, b, c, d, in[9] + 0xd9d4d039, 4);
	MD5STEP(F3, d, a, b, c, in[12] + 0xe6db99e5, 11);
	MD5STEP(F3, c, d, a, b, in[15] + 0x1fa27cf8, 16);
	MD5STEP(F3, b, c, d, a, in[2] + 0xc4ac5665, 23);

	MD5STEP(F4, a, b, c, d, in[0] + 0xf4292244, 6);
	MD5STEP(F4, d, a, b, c, in[7] + 0x432aff97, 10);
	MD5STEP(F4, c, d, a, b, in[14] + 0xab9423a7, 15);
	MD5STEP(F4, b, c, d, a, in[5] + 0xfc93a039, 21);
	MD5STEP(F4, a, b, c, d, in[12] + 0x655b59c3, 6);
	MD5STEP(F4, d, a, b, c, in[3] + 0x8f0ccc92, 10);
	MD5STEP(F4, c, d, a, b, in[10] + 0xffeff47d, 15);
	MD5STEP(F4, b, c, d, a, in[1] + 0x85845dd1, 21);
	MD5STEP(F4, a, b, c, d, in[8] + 0x6fa87e4f, 6);
	MD5STEP(F4, d, a, b, c, in[15] + 0xfe2ce6e0, 10);
	MD5STEP(F4, c, d, a, b, in[6] + 0xa3014314, 15);
	MD5STEP(F4, b, c, d, a, in[13] + 0x4e0811a1, 21);
	MD5STEP(F4, a, b, c, d, in[4] + 0xf7537e82, 6);
	MD5STEP(F4, d, a, b, c, in[11] + 0xbd3af235, 10);
	MD5STEP(F4, c, d, a, b, in[2] + 0x2ad7d2bb, 15);
	MD5STEP(F4, b, c, d, a, in[9] + 0xeb86d391, 21);

	buf[0] += a;
	buf[1] += b;
	buf[2] += c;
	buf[3] += d;
}

static char* MD5StrHash(const char *buf, unsigned char hash[49])
{
    unsigned char digest[16];
    struct MD5Context context;
    MD5Init(&context);
    MD5Update(&context, (md5byte*)buf, strlen(buf));
    MD5Final(&context, digest);
    char *ch;
    ch = (char*) hash;
    int i;
    for (i = 0; i < 16; i++ ) {
        sprintf(ch, "%2.2x", digest[i]);
        ch +=2;
    }
    *ch = '\0';
    return (char*) hash;
}
#endif

static char *pw_make (const char *clear, unsigned char hash[128])
{
    return MD5StrHash(clear, hash);
}

static int pw_check(const char *clear, const char* hash)
{
    char pw[128];
    MD5StrHash(clear, (unsigned char*)pw);
    return !strcmp(pw, hash);
}

static char *passwd_name = NULL;

//static char *passwd_name_default()
//{
//   if ( passwd_name ) 
//      return  passwd_name;
//   passwd_name=do_malloc(strlen(DOMINO_PATH)+strlen(DOMINO_PASSWD_FILE)+5);
//   sprintf(passwd_name, "%s/%s", DOMINO_PATH, DOMINO_PASSWD_FILE);
//   return passwd_name;
//}

DO_EXPORT void do_set_passwd_filename(const char *filename)
{
    if ( passwd_name )
       do_free(passwd_name);

    passwd_name = do_malloc(strlen(filename) + 1);
    strcpy(passwd_name, filename);
}
DO_EXPORT int do_authenticate(const char *name, const char *password, int *userlevel)
{
    int len = strlen(name);
    *userlevel = -1;
    FILE *fp;
    char buff[1024], *ch, *head, hash[128];;
    if ( !passwd_name ) {
        passwd_name = do_get_config_filename(DOMINO_PASSWD_LOCAL_FILE,DOMINO_PASSWD_FILE);
    }
    
    if ((fp = fopen(passwd_name, "r")) == NULL)  {
        do_log(LOG_ERR, "Error opening passwd file (%s): %s", passwd_name, strerror(errno));
        return 0;
    }
    while (!feof(fp))  {
        if (fgets(buff, 1023, fp) == NULL) break;
        ch = buff;
        for (; *ch != '\0' && *ch !=':' && *ch != '\n'; ch++);
        if ((*ch == ':') && (len == ch - buff) && !strncmp(name, buff, ch - buff)) {
            head = ++ch;
            for (; *ch != '\0' && *ch !=':' && *ch != '\n'; ch++);
            if (*ch != ':') continue;
            *ch = '\0';
            *userlevel = atoi(head);
            head = ++ch;
            for (; *ch != '\0' && *ch !=':' && *ch != '\n'; ch++);
            *ch = '\0';
            strcpy(hash, head);
            if (!pw_check(password, hash))
                *userlevel = -1;
            break;
        }
    }
    fclose(fp);
    if (*userlevel < 0)
        return 0;
    return *userlevel >=0;

}

DO_EXPORT int do_useradd(const char *name, const char *password, int userlevel)
{

    unsigned char hash[128];
    pw_make(password, hash);

    FILE *fp, *fr;
    int exist;
    char buff[1024];
    exist = 1;
    if ( !passwd_name )
        passwd_name = do_get_config_filename(DOMINO_PASSWD_LOCAL_FILE,DOMINO_PASSWD_FILE);
    
    if ((fp = fopen(passwd_name, "r")) == NULL)  {
        if (errno != 2) {
            do_log(LOG_ERR, "Error opening passwd file (%s): %s\n", passwd_name, strerror(errno));
            return 0;
        }
        exist = 0;
    }
    if (exist) {
        char *newname, *ch;
        newname = (char*) malloc (strlen(passwd_name) + 2);
        sprintf(newname, "%s+", passwd_name);
        if ((fr = fopen(newname, "w")) == NULL)  {
            do_log(LOG_ERR, "Error create passwd file (%s): %s\n", newname, strerror(errno));
            fclose(fp);
            return 0;
        }
        exist = 0;
        while (!feof(fp))  {
            if (fgets(buff, 1023, fp) == NULL) break;
            ch = buff;
            for (; *ch != '\0' && *ch !=':' && *ch != '\n'; ch++);
            if ((strlen(name) == ch - buff ) && !strncmp(name, buff, ch - buff)) {
                sprintf(buff, "%s:%d:%s\n", name, userlevel, hash);
                exist = 1;
            }
            if (fprintf(fr, "%s", buff) <= 0) {
                do_log(LOG_ERR, "Error write passwd file (%s): %s\n", newname, strerror(errno));
                fclose(fp);
                fclose(fr);
                return 0;
            }
        }
        fclose(fp);
        if (!exist) {
            sprintf(buff, "%s:%d:%s\n", name, userlevel, hash);
            if (fprintf(fr, "%s", buff) <= 0) {
                do_log(LOG_ERR, "Error write passwd file (%s): %s\n", newname, strerror(errno));
                fclose(fr);
                return 0;
            }
        }
        fclose(fr);
        sprintf(newname, "%s-", passwd_name);
        remove(newname);
        if (rename (passwd_name, newname) == -1 ) {
             do_log(LOG_ERR, "Error rename file (%s->%s): %s\n", passwd_name, newname, strerror(errno));
             return 0;
        }
        sprintf(newname, "%s+", passwd_name);
        if (rename (newname, passwd_name) == -1 ) {
            do_log(LOG_ERR, "Error rename file (%s->%s): %s\n", newname, passwd_name, strerror(errno));
            return 0;
        }
    }
    else {
        if ((fr = fopen(passwd_name, "w")) == NULL)  {
            do_log(LOG_ERR, "Error create passwd file (%s): %s\n", passwd_name, strerror(errno));
            return 0;
        }
        sprintf(buff, "%s:%d:%s\n", name, userlevel, hash);
        if (fprintf(fr, "%s", buff) <= 0) {
            do_log(LOG_ERR, "Error write passwd file (%s): %s\n", passwd_name, strerror(errno));
            fclose(fr);
            return 0;
        }
        fclose(fr);
    }
    return 1;
}

DO_EXPORT int do_userdel(const char *name, const char *password, int userlevel)
{

    FILE *fp, *fr;
    char buff[1024];
    if ( !passwd_name )
        passwd_name = do_get_config_filename(DOMINO_PASSWD_LOCAL_FILE,DOMINO_PASSWD_FILE);

    if ((fp = fopen(passwd_name, "r")) == NULL)  {
        if (errno != 2) {
            do_log(LOG_ERR, "Error opening passwd file (%s): %s\n", passwd_name, strerror(errno));
            return 0;
        }
        return 1;
    }
    char *newname, *ch;
    newname = (char*) malloc (strlen(passwd_name) + 2);
    sprintf(newname, "%s+", passwd_name);
    if ((fr = fopen(newname, "w")) == NULL)  {
        do_log(LOG_ERR, "Error create passwd file (%s): %s\n", newname, strerror(errno));
        fclose(fp);
        return 0;
    }
    while (!feof(fp))  {
        if (fgets(buff, 1023, fp) == NULL) break;
        ch = buff;
        for (; *ch != '\0' && *ch !=':' && *ch != '\n'; ch++);
        if ((strlen(name) == ch - buff ) && !strncmp(name, buff, ch - buff));
        else
        if (fprintf(fr, "%s", buff) <= 0) {
            do_log(LOG_ERR, "Error write passwd file (%s): %s\n", newname, strerror(errno));
            fclose(fp);
            fclose(fr);
            return 0;
        }
    }
    fclose(fr);
    fclose(fp);
    sprintf(newname, "%s-", passwd_name);
    remove(newname);
    if (rename (passwd_name, newname) == -1 ) {
         do_log(LOG_ERR, "Error rename file (%s->%s): %s\n", passwd_name, newname, strerror(errno));
         return 0;
    }
    sprintf(newname, "%s+", passwd_name);
    if (rename (newname, passwd_name) == -1 ) {
         do_log(LOG_ERR, "Error rename file (%s->%s): %s\n", newname, passwd_name, strerror(errno));
         return 0;
    }
    return 1;
}
