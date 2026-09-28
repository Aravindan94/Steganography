#ifndef DECODE_H
#define DECODE_H

#include <stdio.h>
#include "types.h"
#include "decode.h"

/*
 * Structure to store information required for
 * decoding secret file from stego Image
 */

typedef struct _DecodeInfo
{
    //Stego Image Info
    char *stego_image_fname;
    FILE *fptr_stego_image;

    //Output Secret File Info
    char *output_fname;
    FILE *fptr_output;

    //Decoded data info
    int extn_size;
    char extn_secret_file[10];
    int size_secret_file;

} DecodeInfo;


//Decoding function prototype
Status read_and_validate_decode_args(char *argv[], DecodeInfo *decInfo);


//Perform decoding
Status do_decoding(FILE *fptr_stego_image, FILE *fptr_output);

//Decode magic string
Status decode_magic_string(FILE *fptr_src);

//Decode extension size
Status decode_secret_file_extn_size(FILE *fptr_src, int *extn_size);

//Decode extension
Status decode_secret_file_extn(FILE *fptr_src, char *extn, int size);

//Decode secret file size
Status decode_secret_file_size(FILE *fptr_src, int *file_size);

//Decode secret file data
Status decode_secret_file_data(FILE *fptr_src,
                               FILE *fptr_output,
                               int file_size);

//Decode a byte from LSB 
Status decode_byte_from_lsb(char *image_buffer, char *data);

//Decode size from LSB
Status decode_size_from_lsb(char *image_buffer, int *size);

#endif
