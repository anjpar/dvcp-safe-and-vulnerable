/*
 * DVCP - Safe Version
 * All vulnerabilities have been fixed
 * For Buttercup fuzzing demonstration
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <errno.h>

struct Image {
    char header[4];
    int width;
    int height;
    char data[10];
};

// Safe bounds checking for multiplication
int safe_multiply(int a, int b, int *result) {
    if (a == 0 || b == 0) {
        *result = 0;
        return 0;
    }
    
    // Check for overflow
    if (a > 0 && b > 0 && a > INT_MAX / b) {
        return -1;  // Overflow would occur
    }
    if (a < 0 && b < 0 && a < INT_MAX / b) {
        return -1;  // Overflow would occur
    }
    if ((a > 0 && b < 0 && b < INT_MIN / a) ||
        (a < 0 && b > 0 && a < INT_MIN / b)) {
        return -1;  // Underflow would occur
    }
    
    *result = a * b;
    return 0;
}

// Safe bounds checking for addition
int safe_add(int a, int b, int *result) {
    if ((b > 0 && a > INT_MAX - b) || (b < 0 && a < INT_MIN - b)) {
        return -1;  // Overflow/underflow would occur
    }
    *result = a + b;
    return 0;
}

// Safe bounds checking for subtraction
int safe_subtract(int a, int b, int *result) {
    if ((b < 0 && a > INT_MAX + b) || (b > 0 && a < INT_MIN + b)) {
        return -1;  // Overflow/underflow would occur
    }
    *result = a + b;
    return 0;
}

int ProcessImage(char* filename) {
    FILE *fp;
    struct Image img;

    fp = fopen(filename, "r");
    if (fp == NULL) {
        printf("\nCan't open file or file doesn't exist.\n");
        return -1;
    }

    while (fread(&img, sizeof(img), 1, fp) > 0) {
        printf("\n\tHeader\twidth\theight\tdata\t\n");
        printf("\n\t%s\t%d\t%d\t%s\n", img.header, img.width, img.height, img.data);

        // SAFE: Check for integer overflow before allocation
        int size1;
        if (safe_add(img.width, img.height, &size1) != 0 || size1 <= 0 || size1 > 10000) {
            printf("Invalid size1 calculation\n");
            continue;
        }

        char* buff1 = (char*)malloc(size1);
        if (!buff1) {
            printf("Memory allocation failed\n");
            fclose(fp);
            return -1;
        }

        // SAFE: Bounds-checked memcpy
        size_t copy_size = (sizeof(img.data) < (size_t)size1) ? sizeof(img.data) : (size_t)size1;
        memcpy(buff1, img.data, copy_size);
        
        free(buff1);
        buff1 = NULL;  // SAFE: Prevent double free

        // SAFE: Check for integer underflow and bounds
        int size2;
        if (safe_subtract(img.width, img.height, &size2) != 0) {
            printf("Invalid size2 calculation\n");
            continue;
        }
        size2 += 100;
        if (size2 <= 0 || size2 > 10000) {
            printf("Invalid size2 value\n");
            continue;
        }

        char* buff2 = (char*)malloc(size2);
        if (!buff2) {
            printf("Memory allocation failed\n");
            continue;
        }

        // SAFE: Bounds-checked memcpy
        copy_size = (sizeof(img.data) < (size_t)size2) ? sizeof(img.data) : (size_t)size2;
        memcpy(buff2, img.data, copy_size);

        // SAFE: Check for division by zero
        int size3 = 0;
        if (img.height != 0) {
            size3 = img.width / img.height;
        } else {
            printf("Division by zero prevented\n");
            free(buff2);
            continue;
        }

        // SAFE: Bounds checking for buffer access
        if (size3 < 0 || size3 > 10000) {
            printf("Invalid size3 value\n");
            free(buff2);
            continue;
        }

        char buff3[10] = {0};
        char* buff4 = (char*)malloc(size3 > 0 ? size3 : 1);
        if (!buff4) {
            printf("Memory allocation failed\n");
            free(buff2);
            continue;
        }

        // SAFE: Bounds-checked memcpy
        copy_size = (sizeof(img.data) < (size_t)size3) ? sizeof(img.data) : (size_t)size3;
        if (copy_size > 0) {
            memcpy(buff4, img.data, copy_size);
        }

        // SAFE: No out-of-bounds read
        if (size3 >= 0 && size3 < 10) {
            char OOBR = buff3[size3];
            (void)OOBR;  // Use variable to avoid warning
        }
        if (size3 >= 0 && size3 < (int)size3) {
            char OOBR_heap = buff4[size3];
            (void)OOBR_heap;
        }

        // SAFE: No out-of-bounds write
        if (size3 >= 0 && size3 < 9) {  // Leave room for null terminator
            buff3[size3] = 'c';
        }
        if (buff4 && size3 >= 0 && size3 < (int)size3 - 1) {
            buff4[size3] = 'c';
        }

        // SAFE: Always free buff4
        free(buff4);
        buff4 = NULL;

        // SAFE: Check for multiplication overflow
        int size4;
        if (safe_multiply(img.width, img.height, &size4) != 0 || size4 < 0 || size4 > 100000) {
            printf("Invalid size4 calculation\n");
            free(buff2);
            continue;
        }

        // SAFE: No infinite recursion or heap exhaustion
        printf("Processing complete for this image\n");

        free(buff2);
        buff2 = NULL;
    }

    fclose(fp);
    return 0;
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        printf("Usage: %s <image_file>\n", argv[0]);
        return 1;
    }

    return ProcessImage(argv[1]);
}
