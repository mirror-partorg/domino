#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <zlib.h>
#include "cencode.h"

#define SBER_PREFIX "@02@"

void print_usage()
{
    printf("usage: sberconv <input xml filename> <output base64 filename>"
         "\n"
         "Copyright (C) 2008-2019 Youra Richesse\n"
         "sberconv is free software and comes with ABSOLUTELY NO WARRANTY\n"
         "under the terms of the GNU General Public License\n"
         "\n"
         "Report bugs to sberconv Support Center:\n"
         "  it256@maria-ra.ru\n");
    exit(1);
}

int main(int argc, char *argv[])
{
    if (argc < 3)
        print_usage();

    const char *filename_xml, *filename_base64; 
    FILE *fp;
    char buff[1024], *data;
    size_t data_size;


    filename_xml = argv[1];
    filename_base64 = argv[2];

    if ( (fp = fopen(filename_xml, "rb")) == NULL )  {
        fprintf(stderr, "Error opening input file (%s): %s\n", filename_xml, strerror(errno));
        return 2;
    }
    fseek(fp, 0L, SEEK_END);
    data_size = ftell(fp);
    fseek(fp, 0L, SEEK_SET);

    if ( (data = (char*)malloc(data_size)) == NULL ) {
        fprintf(stderr, "Not enough memory\n");
        return 2;
    }
    //if ( fread(data, data_size, 1, fp) != 1 ) {
    //    fprintf(stderr, "Error read from file (%s): %s\n", filename_xml, strerror(errno));
    //    return 2;
    //}
    // sad but true delete 0d
    data_size = 0;
    while ( !feof(fp) )  {
	    int symc;
        if ( (symc = fgetc(fp)) == EOF ) break;
        if ( symc != 13 )
        	data[data_size++] = symc;
    }
    fclose(fp);
    uLongf size_f;
    char *buf;
    size_f = data_size * 1.1 + 12;
    if ( (buf = (char*)malloc(size_f)) == NULL) {
        fprintf(stderr, "could not allocate memory for data\n");
        return 2;
    }
    int error = compress2(buf, &size_f, data, data_size, 6);
    if ( error != Z_OK ) {
        switch (error) {
        case Z_MEM_ERROR:
        	fprintf(stderr, "ZLib: not enough memory\n");
            break;
        case Z_BUF_ERROR:
            fprintf(stderr, "ZLib: not enough room in the output buffer\n");
            break;
        case Z_STREAM_ERROR:
            fprintf(stderr, "ZLib: input level parameter is invalid\n");
            break;
        default :
            fprintf(stderr, "ZLib: undefined zlib error %d\n", error);
        }
        return 3;
    }
    if ( (fp = fopen(filename_base64, "wb")) == NULL )  {
        fprintf(stderr, "Error opening for write file (%s): %s\n", filename_base64, strerror(errno));
        return 2;
    }
    size_t n;
    char *out;
    size_t size;

    if ( (out = (char*)malloc(size_f*4+strlen(SBER_PREFIX))) == NULL) {
        fprintf(stderr, "could not allocate memory for data\n");
        return 2;
    }
    memcpy(out,SBER_PREFIX,strlen(SBER_PREFIX));
 	base64_encodestate encode;
    base64_init_encodestate(&encode,0);
    size = base64_encode_block(buf, size_f, out + strlen(SBER_PREFIX), &encode);
    size += base64_encode_blockend(out + strlen(SBER_PREFIX) + size, &encode);
    if ( (n = fwrite(out, 1, size+strlen(SBER_PREFIX), fp) ) != size+strlen(SBER_PREFIX)) {
        fprintf(stderr, "Error writing file (%s): %s\n", filename_base64, strerror(errno));
        return 2;
    }
    fclose(fp);
    return 0;
}
