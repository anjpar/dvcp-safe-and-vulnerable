/*
 * DVCP - Vulnerable Version
 * Contains multiple vulnerabilities for fuzzing demonstration
 * Based on hardik05/Damn_Vulnerable_C_Program
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct Image {
    char header[4];
    int width;
    int height;
    char data[10];
};

void stack_operation() {
    char buff[0x1000];
    (void)buff;
    while(1) {
        stack_operation();  // Infinite recursion - stack exhaustion
    }
}

int ProcessImage(char* filename) {
    FILE *fp;
    struct Image img;

    fp = fopen(filename, "r");
    if (fp == NULL) {
        printf("\nCan't open file or file doesn't exist.\n");
        exit(0);
    }

    while (fread(&img, sizeof(img), 1, fp) > 0) {
        printf("\n\tHeader\twidth\theight\tdata\t\n");
        printf("\n\t%s\t%d\t%d\t%s\n", img.header, img.width, img.height, img.data);

        // VULNERABILITY: Integer overflow
        // 0x7FFFFFFF + 1 = 0 (or negative)
        // Will cause very large/small memory allocation
        int size1 = img.width + img.height;
        char* buff1 = (char*)malloc(size1);

        // VULNERABILITY: Heap buffer overflow
        memcpy(buff1, img.data, sizeof(img.data));
        
        free(buff1);
        
        // VULNERABILITY: Double free
        if (size1 % 2 == 0) {
            free(buff1);
        } else {
            // VULNERABILITY: Use after free
            if (size1 % 3 == 0) {
                buff1[0] = 'a';
            }
        }

        // VULNERABILITY: Integer underflow
        // 0 - 1 = -1 (negative, will cause huge malloc)
        int size2 = img.width - img.height + 100;
        char* buff2 = (char*)malloc(size2);

        // VULNERABILITY: Heap buffer overflow
        memcpy(buff2, img.data, sizeof(img.data));

        // VULNERABILITY: Divide by zero
        int size3 = img.width / img.height;

        char buff3[10];
        char* buff4 = (char*)malloc(size3);
        memcpy(buff4, img.data, sizeof(img.data));

        // VULNERABILITY: Out-of-bounds read (OOBR) - stack
        char OOBR = buff3[size3];
        (void)OOBR;
        
        // VULNERABILITY: Out-of-bounds read (OOBR) - heap
        char OOBR_heap = buff4[size3];
        (void)OOBR_heap;

        // VULNERABILITY: Out-of-bounds write (OOBW) - stack
        buff3[size3] = 'c';
        
        // VULNERABILITY: Out-of-bounds write (OOBW) - heap
        buff4[size3] = 'c';

        if (size3 > 10) {
            // VULNERABILITY: Memory leak
            buff4 = 0;
        } else {
            free(buff4);
        }

        int size4 = img.width * img.height;
        if (size4 % 2 == 0) {
            // VULNERABILITY: Stack exhaustion
            stack_operation();
        } else {
            // VULNERABILITY: Heap exhaustion
            char *buff5;
            do {
                buff5 = (char*)malloc(size4);
            } while(buff5);
        }
        
        free(buff2);
    }

    fclose(fp);
    return 0;
}

#ifndef FUZZING_BUILD
int main(int argc, char* argv[]) {
    if (argc != 2) {
        printf("Usage: %s <image_file>\n", argv[0]);
        return 1;
    }

    return ProcessImage(argv[1]);
}
#endif
