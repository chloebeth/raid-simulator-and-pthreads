#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#define LENGTH 9  // maximum length of files


void init() {
    // create the parity file by applying the xor operator 
    // to all 4 text files 
    char f1_txt[LENGTH];
    char f2_txt[LENGTH];
    char f3_txt[LENGTH];
    char f4_txt[LENGTH];
    char output[LENGTH];
    int i;

    FILE *F1 = fopen("F1.txt", "r");
    FILE *F2 = fopen("F2.txt", "r");
    FILE *F3 = fopen("F3.txt", "r");
    FILE *F4 = fopen("F4.txt", "r");
    FILE *parity = fopen("parity.txt", "w");

    // read from all files
    fread(f1_txt, sizeof(f1_txt), LENGTH, F1);
    fread(f2_txt, sizeof(f2_txt), LENGTH, F2);
    fread(f3_txt, sizeof(f3_txt), LENGTH, F3);
    fread(f4_txt, sizeof(f4_txt), LENGTH, F4);

    for (i=0; i<strlen(f1_txt); i++) {
        char tmp = f1_txt[i] ^ f2_txt[i] ^ f3_txt[i] ^ f4_txt[i];
        output[i] = tmp;
    }

    fwrite(output, 1, sizeof(output), parity); 

    fclose(F1);
    fclose(F2);
    fclose(F3);
    fclose(F4);
    fclose(parity);
}

void write_file(int n, int offset, char *text) {
    FILE *file;
    char update[LENGTH];
    char curr[LENGTH];

    // check for which file to write to
    if (n==1) {
        file = fopen("F1.txt", "r+");
    } else if (n==2) {
        file = fopen("F2.txt", "r+");
    } else if (n==3) {
        file = fopen("F3.txt", "r+");
    } else if (n==4) {
        file = fopen("F4.txt", "r+");
    }

    // check if the offset and new text length are within the range 
    if (offset+strlen(text) > LENGTH) {
        printf("Error. Offset + New text exceed the limit.\n");
    } else {
        // write new text
        fread(curr, sizeof(curr), LENGTH, file);
        for (int j=0; j<offset; j++) {
            update[j] = curr[j];
        }

        for (int i=offset; i<strlen(text)+1; i++) {
            update[i] = text[i-offset];
        }

        for (int k=strlen(text)+offset; k<LENGTH; k++) {
            update[k] = curr[k];
        }
        fclose(file);

        if (n==1) {
            file = fopen("F1.txt", "w");
        } else if (n==2) {
            file = fopen("F2.txt", "w");
        } else if (n==3) {
            file = fopen("F3.txt", "w");
        } else if (n==4) {
            file = fopen("F4.txt", "w");
        }

        fwrite(update, 1, strlen(update), file); 
        printf("New file text: %s\n", update);
        fclose(file);

        // update parity file
        init();
    }
}
 
void rebuild() {
    char f1_txt[LENGTH];
    char f2_txt[LENGTH];
    char f3_txt[LENGTH];
    char f4_txt[LENGTH];
    char p_txt[LENGTH];
    char output[LENGTH];

    int size = 0;

    FILE *F1;
    FILE *F2;
    FILE *F3;
    FILE *F4;
    FILE *parity;

    int i;
    int miss = 0; // 1(F1), 2(F2), 3(F3), 4(F4), 5(parity) 
    struct stat file_stat;

    // check for which file is missing
    if (stat("F1.txt", &file_stat) != 0) {
        F1 = fopen("F1.txt", "w");
        F2 = fopen("F2.txt", "r");
        F3 = fopen("F3.txt", "r");
        F4 = fopen("F4.txt", "r");
        parity = fopen("parity.txt", "rb");
        miss = 1;
        fread(f2_txt, sizeof(f2_txt), LENGTH, F2);
        fread(f3_txt, sizeof(f3_txt), LENGTH, F3);
        fread(f4_txt, sizeof(f4_txt), LENGTH, F4);
        fread(p_txt, sizeof(p_txt), LENGTH, parity);
        size = sizeof(f2_txt);
    } 
    
    if (stat("F2.txt", &file_stat) != 0) {
        F1 = fopen("F1.txt", "r");
        F2 = fopen("F2.txt", "w");
        F3 = fopen("F3.txt", "r");
        F4 = fopen("F4.txt", "r");
        parity = fopen("parity.txt", "rb");
        miss = 2;
        fread(f1_txt, sizeof(f1_txt), LENGTH, F1);
        fread(f3_txt, sizeof(f3_txt), LENGTH, F3);
        fread(f4_txt, sizeof(f4_txt), LENGTH, F4);
        fread(p_txt, sizeof(p_txt), LENGTH, parity);
        size = sizeof(f1_txt);
    } 
    
    if (stat("F3.txt", &file_stat) != 0) {
        F1 = fopen("F1.txt", "r");
        F2 = fopen("F2.txt", "r");
        F3 = fopen("F3.txt", "w");
        F4 = fopen("F4.txt", "r");
        parity = fopen("parity.txt", "rb");
        miss = 3;
        fread(f2_txt, sizeof(f2_txt), LENGTH, F2);
        fread(f1_txt, sizeof(f1_txt), LENGTH, F1);
        fread(f4_txt, sizeof(f4_txt), LENGTH, F4);
        fread(p_txt, sizeof(p_txt), LENGTH, parity);
        size = sizeof(f2_txt);
    }
    
    if (stat("F4.txt", &file_stat) != 0) {
        F1 = fopen("F1.txt", "r");
        F2 = fopen("F2.txt", "r");
        F3 = fopen("F3.txt", "r");
        F4 = fopen("F4.txt", "w");
        parity = fopen("parity.txt", "rb");
        miss = 4;
        fread(f2_txt, sizeof(f2_txt), LENGTH, F2);
        fread(f3_txt, sizeof(f3_txt), LENGTH, F3);
        fread(f1_txt, sizeof(f1_txt), LENGTH, F1);
        fread(p_txt, sizeof(p_txt), LENGTH, parity);
        size = sizeof(f1_txt);
    } 

    if (stat("parity.txt", &file_stat) != 0) {
        miss = 5;
    }

    for (i=0; i<size; i++) {
        if (miss==1) {
            char tmp = p_txt[i] ^ f2_txt[i] ^ f3_txt[i] ^ f4_txt[i];
            output[i] = tmp;
        } else if (miss==2) {
            char tmp = f1_txt[i] ^ p_txt[i] ^ f3_txt[i] ^ f4_txt[i];
            output[i] = tmp;
        } else if (miss==3) {
            char tmp = f1_txt[i] ^ f2_txt[i] ^ p_txt[i] ^ f4_txt[i];
            output[i] = tmp;
        } else if (miss==4) {
            char tmp = f1_txt[i] ^ f2_txt[i] ^ f3_txt[i] ^ p_txt[i];
            output[i] = tmp;
        }
    }

    if (miss==1) {
        fwrite(output, 1, strlen(output)-1, F1); 
    } else if (miss==2) {
        fwrite(output, 1, strlen(output)-1, F2); 
    } else if (miss==3) {
        fwrite(output, 1, strlen(output)-1, F3); 
    } else if (miss==4) {
        fwrite(output, 1, strlen(output)-1, F4); 
    } else if (miss==5) {
        init();
    }


    printf("Repaired: ");
    int j = 0;
    while (j < 8) {
        printf("%c", output[j]);
        j++;
    }
    printf("\n");
    fclose(F1);
    fclose(F2);
    fclose(F3);
    fclose(F4);
    fclose(parity);
}

int main(int argc, char **argv) {
    int mode;           // 0 for init, 1 for write, 2 for rebuild
    int n;              // which file to write to
    int offset;         // offset of which byte to write at 
    char *text;

    if (!(argc > 2)) {
        printf("Please specify an option.\n");
        printf("RAID-Sim -I\n");
        printf("RAID-Sim -W n offset 'text'\n");
        printf("RAID-Sim -R\n");
    } else if (strcmp(argv[1], "RAID-Sim")) {
        printf("Invalid command. Please use 'RAID-Sim -command.'\n");
    } else {
        if (!strcmp(argv[2], "-I")) {
            mode = 0;
            printf("Init\n");
        } else if (!strcmp(argv[2], "-W")) {
            mode = 1;
            n = atoi(argv[3]);
            offset = atoi(argv[4]);
            text = argv[5];
            printf("Write\n");
        } else if (!strcmp(argv[2], "-R")) {
            mode = 2;
            printf("Repair\n");
        } else {
            printf("Please use the command 'RAID-Sim -I', RAID-Sim -W n offset 'text'', or 'RAID-Sim -R'\n");
        }
    }

    if (mode==0) {
        init();
    } else if (mode==1) {
        write_file(n, offset, text);
    } else if (mode==2) {
        rebuild();
    }

    return 0;
}
