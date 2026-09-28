/*Project Title:Image Steganography using LSB Technique
Name: S. Aravindan
Batch: ECEP25036A
Date: 25-02-2026
Steganography is a method of hiding confidential information inside 
another medium such as an image, audio, or video so 
that the presence of the data is not visible to others.
In this project:
Secret text file data is embedded into a BMP image.
Decoding retrieves the hidden data back from the image.
LSB technique is used for encoding and decoding.
*/
#include <stdio.h>
#include <string.h>
#include "encode.h"
#include "decode.h"
#include "types.h"

/* Function prototype */
OperationType check_operation_type(char *symbol);

int main(int argc, char *argv[])
{
    EncodeInfo encInfo;
    DecodeInfo decInfo;

    //step1 -> check_operation_type(argv[1])
    OperationType ret = check_operation_type(argv[1]);

    //step2 -> check the return value == e_encode
    if(ret == e_encode)
    {
        //declare structure variable EncodeInfo encInfo

        //--> read_and_validate_encode_args(pass command line arg, &encInfo) == e_success or e_failure
        if (read_and_validate_encode_args(argv, &encInfo) == e_success)
        {
            printf("Read and validation successful\n");

            // e_failure -> print error msg and stop the program
            // e_success -> next step.
            // call do_encoding(&encInfo);
            //e_failure -> print error msg and stop the program

            if(do_encoding(&encInfo) == e_success)
            {
                printf("Encoding successful\n");
            }
            else
            {
                printf("Error: Encoding failed\n");
            }
        }
        else
        {
            printf("Error: Invalid encode arguments\n");
            return 1;
        }
    }

    else if(ret==e_decode)
    {

        if (read_and_validate_decode_args(argv, &decInfo) == e_success)
        {
            printf("Read and validation successful\n");

            // e_failure -> print error msg and stop the program
            // e_success -> next step.
            // call do_encoding(&encInfo);
            //e_failure -> print error msg and stop the program

        if(do_decoding(decInfo.fptr_stego_image,decInfo.fptr_output) == e_success)
            {
                printf("Decoding successful\n");
            }
            else
            {
                printf("Error: Decoding failed\n");
            }
        }
        else
        {
            printf("Error: Invalid decode arguments\n");
            return 1;
        }
    }//step3 -> return value == e_decode
            // --
    //step3 -> return value == e_unsupported
            // --> print invalid arg
            // -e -> encode
            // -d  -> decode

    else if(ret == e_decode)
    {
        printf("Decoding not implemented\n");
    }
    
    else
    {
        printf("Unsupported operation\n");
    }

    return 0;
}


/* Function Definitions */

OperationType check_operation_type(char *symbol)
{
    if(strcmp(symbol,"-e")==0)// if it is -e return e_encode
    {
        return e_encode;
    }
    else if(strcmp(symbol,"-d")==0)//else if it is -d return e_decode
    {
        return e_decode;
    }
    else// else return e_unsuported
    {
        return e_unsupported;
    }
}






