#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

int main(void) {
    char line[100]; //Buffer for user input
    char *endptr;
    long num;

    printf("ml' nob:\n");

    //Read one line of input
    if (fgets(line, sizeof(line), stdin) == NULL) {
        printf("Neh mi'\n");
        return EXIT_SUCCESS;
    }

    //Remove trailing newline if present 
    line[strcspn(line, "\n")] = '\0';

    //Skip leading whitespace
    char *ptr = line;
    while (isspace((unsigned char)*ptr)) ptr++;

    //If line is empty after trimming -> invalid
    if(*line == '\0') {
        printf("Neh mi'\n");
        return EXIT_SUCCESS;
    }

    //Try to convert to integer
    num = strtol(line, &endptr, 10);

    //Check if entire input was a valid integer (no extra junk)
    while (isspace((unsigned char)*endptr)) endptr++;
    if (*endptr != '\0') {
        printf("Neh mi'\n");
        return EXIT_SUCCESS;
    }

    //Check range
    if (num < 0 || num > 8) {
        printf("Qih mi' %ld\n", num);
        return EXIT_SUCCESS;
    }

    //Valid integer in range
    printf("Qapla'\n");

    switch (num) {
        case 0:
            printf("noH QapmeH wo' Qaw'lu'chugh yay chavbe'lu' 'ej wo' choqmeH may' DoHlu'chugh lujbe'lu'.\n");
            break;
        case 1:
            printf("bortaS bIr jablu'DI' reH QaQqu' nay'.\n");
            break;
        case 2:
            printf("Qu' buSHa'chugh SuvwI', batlhHa' vangchugh, qoj matlhHa'chugh, pagh ghaH SuvwI''e'.\n");
            break;
        case 3:
            printf("bISeH'eghlaH'be'chugh latlh Dara'laH'be'.\n");
            break;
        case 4:
            printf("qaStaHvIS wa' ram loS SaD Hugh SIjlaH qetbogh loD.\n");
            break;
        case 5:
            printf("Suvlu'taHvIS yapbe' HoS neH.\n");
            break;
        case 6:
            printf("Ha'DIbaH DaSop 'e' DaHechbe'chugh yIHoHQo'.\n");
            break;
        case 7:
            printf("Heghlu'meH QaQ jajvam.\n");
            break;
        case 8:
            printf("leghlaHchu'be'chugh mIn lo'laHbe' taj jej.\n");
            break;
    }
    return EXIT_SUCCESS;
}