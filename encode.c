#include <stdio.h>
#include<string.h>
#include "encode.h"
#include "types.h"
#include "common.h"

/* Function Definitions */

/* Get image size
 * Input: Image file ptr
 * Output: width * height * bytes per pixel (3 in our case)
 * Description: In BMP Image, width is stored in offset 18,
 * and height after that. size is 4 bytes
 */
uint get_image_size_for_bmp(FILE *fptr_image)
{
    uint width, height;
    // Seek to 18th byte
    fseek(fptr_image, 18, SEEK_SET);

    // Read the width (an int)
    fread(&width, sizeof(int), 1, fptr_image);
    printf("width = %u\n", width);

    // Read the height (an int)
    fread(&height, sizeof(int), 1, fptr_image);
    printf("height = %u\n", height);

    // Return image capacity
    return width * height * 3;
}

uint get_file_size(FILE *fptr)
{
    fseek(fptr, 0, SEEK_END);
    return ftell(fptr);// Find the size of secret file data
}

/*
 * Get File pointers for i/p and o/p files
 * Inputs: Src Image file, Secret file and
 * Stego Image file
 * Output: FILE pointer for above files
 * Return Value: e_success or e_failure, on file errors
 */

Status read_and_validate_encode_args(char *argv[], EncodeInfo *encInfo)
{
    //step1 -> check source file name having .bmp present or not
            // no -> return e_failure
    if (strstr(argv[2], ".bmp") != NULL)
    {
        // yes -> store source file name into encInfo->src_image_fname
        encInfo->src_image_fname = argv[2];
    }
    else
    {
        return e_failure;
    }

    //step2 -> check secret file having extn or not
    if (strchr(argv[3], '.') != NULL)
    {
        // yes -> store secret file name into encInfo->src_image_fname
        encInfo->secret_fname = argv[3];
    }
    else
    {
       // no -> return e_failure 
        return e_failure;
    }

    if (argv[4] != NULL)
    {
        // check .bmp extension present or not
        if (strstr(argv[4], ".bmp") != NULL)
        {
            // store stego image file name
            encInfo->stego_image_fname = argv[4];
        }
        else
        {
            printf("ERROR: Output file should be .bmp\n");
            return e_failure;
        }
    }
    else
    {
        // no -> store default name to encInfo->stego_image_fname = "stego.bmp";
        encInfo->stego_image_fname = "stego.bmp";
    }

    //step4 ->return e_success
    return e_success;
}
    // step3 -> check optional file is passed or not
            // yes -> check the file having .bmp or not
                    // no -> return e_failure
                    //yes -> store the file name into encInfo->stego_image_fname
            // no -> store default name to encInfo->stego_image_fname = "stego.bmp";
    //step4 -> return e_success

Status open_files(EncodeInfo *encInfo)
{
    // Src Image file
    encInfo->src_image_fptr = fopen(encInfo->src_image_fname, "rb");
    // Do Error handling
    if (encInfo->src_image_fptr == NULL)
    {
        perror("fopen");
        fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->src_image_fname);

        return e_failure;
    }

    // Secret file
    encInfo->fptr_secret = fopen(encInfo->secret_fname, "rb");
    // Do Error handling
    if (encInfo->fptr_secret == NULL)
    {
        perror("fopen");
        fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->secret_fname);

        return e_failure;
    }

    // Stego Image file
    encInfo->dest_image_fptr = fopen(encInfo->stego_image_fname, "wb");
    // Do Error handling
    if (encInfo->dest_image_fptr == NULL)
    {
        perror("fopen");
        fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->stego_image_fname);

        return e_failure;
    }

    // No failure return e_success
    return e_success;
}

Status check_capacity(EncodeInfo *encInfo)
{
    // step1 -> encInfo->image_capacity =get_image_size_for_bmp(source_file_pointer)
    encInfo->image_capacity = get_image_size_for_bmp(encInfo->src_image_fptr);

    // step2 -> find secret file size encInfo -> size_secret_file = get_file_size(secret file pointer)
    encInfo->size_secret_file = get_file_size(encInfo->fptr_secret);

    // step3 -> compare encInfo->image_capacity > 16 + 32 + 32 + 32 + 54 + (encInfo -> size_secret_file * 8)
    if (encInfo->image_capacity > (16 + 32 + 32 + 32 + 54 +(encInfo->size_secret_file * 8)))
    {
        //yes -> return e_success
        return e_success;
    }
    else
    {
        // no -> return e_failure
        return e_failure;
    }
}

Status copy_bmp_header(FILE *fptr_src_image, FILE *fptr_dest_image)
{
    char str[54];

    // Setting pointer to point to 0th position
    fseek(fptr_src_image, 0, SEEK_SET);

    // Reading 54 bytes from beautiful.bmp
    fread(str, 54, 1, fptr_src_image);

    // Writing 54 bytes to str
    fwrite(str, 54, 1, fptr_dest_image);
    return e_success;
}

Status encode_magic_string(const char *magic_string, EncodeInfo *encInfo)
{
    int i;
    char buffer[8];

    // Encode each character of magic string
    for (i = 0; magic_string[i] != '\0'; i++)
    {
        // encode one character (8 bits)
        fread(buffer,1,8,encInfo->src_image_fptr);
        encode_byte_to_lsb(magic_string[i],buffer);
        fwrite(buffer,1,8,encInfo->dest_image_fptr);
    }

    return e_success;
}

Status encode_secret_file_extn(const char *file_extn, EncodeInfo *encInfo)
{
    int i;
    char buffer[8];

    // Encode each character of file extension
    for(i = 0; file_extn[i] != '\0'; i++)
    {
        fread(buffer,1,8,encInfo->src_image_fptr);
        encode_byte_to_lsb(file_extn[i],buffer);
        fwrite(buffer,1,8,encInfo->dest_image_fptr);
    }
    return e_success;
}

Status encode_secret_file_data(EncodeInfo *encInfo)
{
    char secret_byte;
    char buffer[8];

    // Move secret file pointer to beginning
    fseek(encInfo->fptr_secret, 0, SEEK_SET);

    // Read secret file byte by byte
    while (fread(&secret_byte, sizeof(char), 1,
                 encInfo->fptr_secret) == 1)
    {
        // Encode one byte into image
        fread(buffer,1,8,encInfo->src_image_fptr);
        encode_byte_to_lsb(secret_byte,buffer);
        fwrite(buffer,1,8,encInfo->dest_image_fptr);
    }

    return e_success;
}


Status copy_remaining_img_data(FILE *fptr_src, FILE *fptr_dest)
{
     char buffer;

    // Copy until end of source image file
    while (fread(&buffer, sizeof(char), 1, fptr_src) == 1)
    {
        if (fwrite(&buffer, sizeof(char), 1, fptr_dest) != 1)
        {
            return e_failure;
        }
    }

    return e_success;
}

Status encode_byte_to_lsb(char data, char *image_buffer)
{
    int i;

    for (i = 0; i < 8; i++)
    {
        // Clear LSB of image byte
        image_buffer[i] = image_buffer[i] & 0xFE;

        // Take bit from data and set as LSB
        image_buffer[i] |= ((data >> (7 - i)) & 1);
    }

    return e_success;
}
Status encode_size_to_lsb(int size, char *imageBuffer)
{
    int i;

    for (i = 0; i < 32; i++)
    {
        // Clear LSB of image byte
        imageBuffer[i] &= 0xFE;

        // Set LSB with corresponding bit of size
        imageBuffer[i] |= ((size >> (31 - i)) & 1);
    }

    return e_success;
}
Status do_encoding(EncodeInfo *encInfo)
{
     //Step 1 -> Open required files
    if (open_files(encInfo) == e_failure)
    {
        printf("ERROR: Unable to open files\n");
        return e_failure;
    }
    printf("INFO: Files opened successfully\n");


    //Step 2 -> Check image capacity
    if (check_capacity(encInfo) == e_failure)
    {
        printf("ERROR: Insufficient image capacity\n");
        return e_failure;
    }
    printf("INFO: Image has enough capacity\n");


    //Step 3 -> Copy BMP header 
    if (copy_bmp_header(encInfo->src_image_fptr,
                        encInfo->dest_image_fptr) == e_failure)
    {
        printf("ERROR: Failed to copy BMP header\n");
        return e_failure;
    }
    printf("INFO: BMP header copied successfully\n");


    //Step 4 -> Encode magic string
    if (encode_magic_string(MAGIC_STRING, encInfo) == e_failure)
    {
        printf("ERROR: Magic string encoding failed\n");
        return e_failure;
    }
    printf("INFO: Magic string encoded\n");


    //Step 5 -> Encode extension size
    if (encode_secret_file_extn_size(strlen(encInfo->extn_secret_file),
                                     encInfo) == e_failure)
    {
        printf("ERROR: Extension size encoding failed\n");
        return e_failure;
    }
    printf("INFO: Extension size encoded\n");


    //Step 6 -> Encode extension
    if (encode_secret_file_extn(encInfo->extn_secret_file,
                                encInfo) == e_failure)
    {
        printf("ERROR: Extension encoding failed\n");
        return e_failure;
    }
    printf("INFO: Extension encoded\n");


    //Step 7 -> Encode secret file size
    if (encode_secret_file_size(encInfo->size_secret_file,
                                encInfo) == e_failure)
    {
        printf("ERROR: Secret file size encoding failed\n");
        return e_failure;
    }
    printf("INFO: Secret file size encoded\n");


    //Step 8 -> Encode secret file data
    if (encode_secret_file_data(encInfo) == e_failure)
    {
        printf("ERROR: Secret file data encoding failed\n");
        return e_failure;
    }
    printf("INFO: Secret file data encoded\n");


    //Step 9 -> Copy remaining image data
    if (copy_remaining_img_data(encInfo->src_image_fptr,
                                encInfo->dest_image_fptr) == e_failure)
    {
        printf("ERROR: Failed to copy remaining image data\n");
        return e_failure;
    }
    printf("INFO: Remaining image data copied\n");


    printf("INFO: Encoding completed successfully\n");
    return e_success;
}
Status encode_secret_file_extn_size(int size, EncodeInfo *encInfo)
{
    char image_buffer[32];

    // Read 32 bytes from source image
    fread(image_buffer, sizeof(char), 32,
          encInfo->src_image_fptr);

    // Encode size into LSB
    encode_size_to_lsb(size, image_buffer);

    // Write modified bytes to destination image
    fwrite(image_buffer, sizeof(char), 32,
           encInfo->dest_image_fptr);

    return e_success;
}
Status encode_secret_file_size(long file_size, EncodeInfo *encInfo)
{
    char image_buffer[32];

    // Read 32 bytes from source image
    fread(image_buffer, sizeof(char), 32,
          encInfo->src_image_fptr);

    // Encode file size into LSB
    encode_size_to_lsb(file_size, image_buffer);

    // Write modified bytes to destination image
    fwrite(image_buffer, sizeof(char), 32,
           encInfo->dest_image_fptr);

    return e_success;
}

