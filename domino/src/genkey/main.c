#include <stdio.h>
#include <stdlib.h>
#include <gcrypt.h>
#include <errno.h>

static void usage(void)
{
	printf("usage: genkey <filekey>\n");
    exit(1);
}

int main(int argc, char **argv)
{
    if (argc != 2)
        usage();
    unsigned char *key;
    size_t len;
    len = gcry_cipher_get_algo_keylen(GCRY_CIPHER_AES);
    key = malloc(len);
    gcry_randomize(key, len, GCRY_STRONG_RANDOM);
    char *keystr;
    keystr = malloc(len*2 + 1);
    char *ch;
    ch = keystr;
    int i;
    for (i = 0; i < len; i++ ) {
        sprintf(ch, "%2.2x", key[i]);
        ch +=2;
    }
    *ch = '\0';
    FILE *fp;
    if ((fp = fopen(argv[1], "w")) == NULL)  {
        fprintf(stderr, "Error create file (%s): %s", argv[1], strerror(errno));
        return 0;
    }
    fprintf(fp, "-----BEGIN AES PRIVATE KEY-----\n"
            "%s\n"
            "-----END AES PRIVATE KEY-----\n", keystr);
    fclose(fp);
    return 0;
}
