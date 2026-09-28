#include <stdio.h>
#include <string.h>
#include "decode.h"
#include "types.h"
#include "common.h"


Status read_and_validate_decode_args(char *argv[], DecodeInfo *decInfo)
{
    //Step 1 : Check stego image file present or not 
    if (argv[2] == NULL)
    {
        printf("Error: Stego image file not provided\n");
        return e_failure;
    }

    //Step 2 : Check file extension (.bmp) 
    if (strstr(argv[2], ".bmp") != NULL)
    {
        decInfo->stego_image_fname = argv[2];
        printf("INFO: Stego image file found\n");
    }
    else
    {
        printf("Error: Stego image must be .bmp file\n");
        return e_failure;
    }

    // Step 3 : Check output file name 
    if (argv[3] != NULL)
    {
        decInfo->output_fname = argv[3];
        printf("INFO: Output file name set as %s\n", decInfo->output_fname);
    }
    else
    {
        decInfo->output_fname = "decoded.txt";
    }

    //Step 4 : Open stego image file
    decInfo->fptr_stego_image =
            fopen(decInfo->stego_image_fname, "r");

    if (decInfo->fptr_stego_image == NULL)
    {
        printf("Error: Unable to open stego image\n");
        return e_failure;
    }
    printf("INFO: Stego image opened successfully\n");

    //Step 5 : Open output file
    decInfo->fptr_output =
            fopen(decInfo->output_fname, "w");

    if (decInfo->fptr_output == NULL)
    {
        printf("Error: Unable to create output file\n");
        return e_failure;
    }
    return e_success;
    printf("INFO: Output file created successfully\n");
}



//Decode a byte from LSB
Status decode_byte_from_lsb(char *image_buffer, char *data)
{
    int i;
    *data = 0;

    for(i = 0; i < 8; i++)
    {
        *data = *data << 1;
        *data |= (image_buffer[i] & 1);
    }

    return e_success;
}

//Decode size from LSB (32 bits)
Status decode_size_from_lsb(char *image_buffer, int *size)
{
    int i;
    *size = 0;

    for(i = 0; i < 32; i++)
    {
        *size = *size << 1;
        *size |= (image_buffer[i] & 1);
    }

    return e_success;
}

//Decode magic string
Status decode_magic_string(FILE *fptr_src)
{
    char buffer[8];
    char data;
    int i;

    for(i = 0; MAGIC_STRING[i] != '\0'; i++)
    {
        fread(buffer,1,8,fptr_src);
        decode_byte_from_lsb(buffer,&data);

        if(data != MAGIC_STRING[i])
        {
            return e_failure;
        }
    }

    return e_success;
}

//Decode extension size
Status decode_secret_file_extn_size(FILE *fptr_src, int *extn_size)
{
    char buffer[32];

    fread(buffer,1,32,fptr_src);
    decode_size_from_lsb(buffer,extn_size);

    return e_success;
}

//Decode extension
Status decode_secret_file_extn(FILE *fptr_src, char *extn, int size)
{
    char buffer[8];
    char data;
    int i;

    for(i = 0; i < size; i++)
    {
        fread(buffer,1,8,fptr_src);
        decode_byte_from_lsb(buffer,&data);
        extn[i] = data;
    }

    extn[size] = '\0';
    return e_success;
}

// Decode secret file size
Status decode_secret_file_size(FILE *fptr_src, int *file_size)
{
    char buffer[32];

    fread(buffer,1,32,fptr_src);
    decode_size_from_lsb(buffer,file_size);

    return e_success;
}

//Decode secret file data
Status decode_secret_file_data(FILE *fptr_src,
                               FILE *fptr_output,
                               int file_size)
{
    char buffer[8];
    char data;
    int i;

    for(i = 0; i < file_size; i++)
    {
        fread(buffer,1,8,fptr_src);
        decode_byte_from_lsb(buffer,&data);
        fwrite(&data,1,1,fptr_output);
    }

    return e_success;
}

//Main decoding function
Status do_decoding(FILE *fptr_stego_image, FILE *fptr_output)
{
    int extn_size;
    int secret_file_size;
    char extn[10];

    //Skip BMP header
    fseek(fptr_stego_image,54,SEEK_SET);
    printf("INFO: BMP header skipped\n");
    //Decode magic string
    if(decode_magic_string(fptr_stego_image) == e_failure)
    {
        printf("ERROR: Magic string mismatch\n");
        return e_failure;
    }
    printf("INFO: Magic string decoded successfully\n");

    //Decode extension size
    decode_secret_file_extn_size(fptr_stego_image,&extn_size);
    printf("INFO: Extension size decoded: %d\n", extn_size);

    //Decode extension
    decode_secret_file_extn(fptr_stego_image,extn,extn_size);
    printf("INFO: Extension decoded: %s\n", extn);

    //Decode secret file size
    decode_secret_file_size(fptr_stego_image,&secret_file_size);
    printf("INFO: Secret file size decoded: %d bytes\n", secret_file_size);
    //Decode secret file data
    decode_secret_file_data(fptr_stego_image,
                            fptr_output,
                            secret_file_size);
    printf("INFO: Secret data decoded successfully\n");

    printf("INFO: Decoding completed successfully\n");

    return e_success;
}
