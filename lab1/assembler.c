/*
    Name 1: Sidharth Nair
    UTEID 1: sn25377
*/

#include <stdio.h> /* standard input/output library */
#include <stdlib.h> /* Standard C Library */
#include <string.h> /* String operations library */
#include <ctype.h> /* Library for useful character operations */
#include <limits.h> /* Library for definitions of common variable type characteristics */

#define MAX_LINE_LENGTH 255
#define MAX_LABEL_LEN 20
#define MAX_SYMBOLS 255

enum
{
    DONE, OK, EMPTY_LINE
};

enum
{
    NOT_SIGNED, SIGNED
};

FILE* infile = NULL;
FILE* outfile = NULL;


typedef struct {
    int address;
    char label[MAX_LABEL_LEN + 1];
} TableEntry;
TableEntry symbolTable[MAX_SYMBOLS];
int size = 0;

void exitAndClose(int err) {
    fclose(infile);
    fclose(outfile);
    exit(err);
}

int toNum(char * pStr ) {
    char * t_ptr;
    char * orig_pStr;
    int t_length,k;
    int lNum, lNeg = 0;
    long int lNumLong;

    orig_pStr = pStr;
    if( *pStr == '#' )                                /* decimal */
    {
        pStr++;
        if( *pStr == '-' )                                /* dec is negative */
        {
            lNeg = 1;
            pStr++;
        }
        t_ptr = pStr;
        t_length = strlen(t_ptr);
        for(k=0;k < t_length;k++)
        {
            if (!isdigit(*t_ptr))
            {
                printf("Error: invalid decimal operand, %s\n",orig_pStr);
                exitAndClose(4);
            }
            t_ptr++;
        }
        lNum = atoi(pStr);
        if (lNeg)
            lNum = -lNum;

        return lNum;
    }
    else if( *pStr == 'x' )        /* hex     */
    {
        pStr++;
        if( *pStr == '-' )                                /* hex is negative */
        {
            lNeg = 1;
            pStr++;
        }
        t_ptr = pStr;
        t_length = strlen(t_ptr);
        for(k=0;k < t_length;k++)
        {
            if (!isxdigit(*t_ptr))
            {
                printf("Error: invalid hex operand, %s\n",orig_pStr);
                exitAndClose(4);
            }
            t_ptr++;
        }
        lNumLong = strtol(pStr, NULL, 16);    /* convert hex string into integer */
        lNum = (lNumLong > INT_MAX)? INT_MAX : lNumLong;
        if( lNeg )
            lNum = -lNum;
        return lNum;
    }
    else
    {
        printf( "Error: invalid operand, %s\n", orig_pStr);
        exitAndClose(4);  /* This has been changed from error code 3 to error code 4, see clarification 12 */
    }
}

int isOpcode(char *lPtr) {
    if ((strcmp(lPtr, "add") == 0) || (strcmp(lPtr, "and") == 0) || (strcmp(lPtr, "br") == 0) ||
        (strcmp(lPtr, "brn") == 0) || (strcmp(lPtr, "brz") == 0) || (strcmp(lPtr, "brp") == 0) ||
        (strcmp(lPtr, "brzp") == 0) || (strcmp(lPtr, "brnp") == 0) || (strcmp(lPtr, "brnz") == 0) ||
        (strcmp(lPtr, "brnzp") == 0) || (strcmp(lPtr, "jmp") == 0) || (strcmp(lPtr, "jsr") == 0) ||
        (strcmp(lPtr, "jsrr") == 0) || (strcmp(lPtr, "ldb") == 0) || (strcmp(lPtr, "ldw") == 0) ||
        (strcmp(lPtr, "lea") == 0) || (strcmp(lPtr, "not") == 0) || (strcmp(lPtr, "ret") == 0) ||
        (strcmp(lPtr, "rti") == 0) || (strcmp(lPtr, "lshf") == 0) || (strcmp(lPtr, "rshfl") == 0) ||
        (strcmp(lPtr, "rshfa") == 0) || (strcmp(lPtr, "stb") == 0) || (strcmp(lPtr, "stw") == 0) ||
        (strcmp(lPtr, "trap") == 0) || (strcmp(lPtr, "xor") == 0) || (strcmp(lPtr, "halt") == 0) ||
        (strcmp(lPtr, "getc") == 0) || (strcmp(lPtr, "out") == 0) || (strcmp(lPtr, "puts") == 0) ||
        (strcmp(lPtr, "in") == 0) || (strcmp(lPtr, "nop") == 0)) {
        return 1;
    }
    return -1;
}

int readAndParse( FILE * pInfile, char * pLine, char ** pLabel, char ** pOpcode, char ** pArg1, char ** pArg2, char ** pArg3, char ** pArg4) {
    char * lRet, * lPtr;
    int i;
    if( !fgets( pLine, MAX_LINE_LENGTH, pInfile ) )
        return( DONE );
    for( i = 0; i < strlen( pLine ); i++ )
        pLine[i] = tolower( pLine[i] );

    /* convert entire line to lowercase */
    *pLabel = *pOpcode = *pArg1 = *pArg2 = *pArg3 = *pArg4 = pLine + strlen(pLine);

    /* ignore the comments */
    lPtr = pLine;

    while( *lPtr != ';' && *lPtr != '\0' &&
           *lPtr != '\n' )
        lPtr++;

    *lPtr = '\0';
    if( !(lPtr = strtok( pLine, "\t\n ," ) ) )
        return( EMPTY_LINE );

    if( isOpcode( lPtr ) == -1 && lPtr[0] != '.' ) /* found a label */
    {
        *pLabel = lPtr;
        if( !( lPtr = strtok( NULL, "\t\n ," ) ) ) {
            *pOpcode = NULL;
            return( OK );
        }
    }

    *pOpcode = lPtr;

    if( !( lPtr = strtok( NULL, "\t\n ," ) ) ) {
        *pArg1 = NULL;
        return( OK );
    }

    *pArg1 = lPtr;

    if( !( lPtr = strtok( NULL, "\t\n ," ) ) ) {
        *pArg2 = NULL;
        return( OK );
    }

    *pArg2 = lPtr;
    if( !( lPtr = strtok( NULL, "\t\n ," ) ) ) {
        *pArg3 = NULL;
        return( OK );
    }

    *pArg3 = lPtr;

    if( !( lPtr = strtok( NULL, "\t\n ," ) ) ) {
        *pArg4 = NULL;
        return( OK );
    }

    *pArg4 = lPtr;

    return( OK );
}

int power(int x, int n) {
    int ret = 1;
    for (int i = 0; i < n; i++) {
        ret *= x;
    }
    return ret;
}

// Converts a decimal number to a signed or unsigned binary number using n bits
char* decToBinary(int dec, char *bin, int n, int isSigned) {
    if (isSigned) {
        if (dec > power(2, n - 1) - 1 || dec < -1 * power (2, n - 1)) {
            exitAndClose(3);
        }
    }
    else {
        if (dec > power(2, n) - 1 || dec < 0) {
            exitAndClose(3);
        }
    }
    int tmp = dec;
    bin[n] = '\0';
    for (int i = n - 1; i >= 0; i--) {
        if (tmp % 2 == 0) {
            bin[i] = '0';
        } else {
            bin[i] = '1';
        }
        tmp /= 2;
    }
    if (dec < 0) { // flip the bits and add one
        for (int i = 0; i < n; i++) {
            bin[i] == '0' ? bin[i]++ : bin[i]--;
        }
        for (int i = n - 1; i >= 0; i--) {
            if (bin[i] == '1') {
                bin[i]--;
            }
            else {
                bin[i]++;
                break;
            }
        }
    }
    return bin;
}

// Converts a binary string (must be n = A*4 digits long where A is any integer) to its hex representation
char* binaryInstructionToHex(char *bin, char *hex, int n) {
    hex[n/4] = '\0';
    for (int i = 0; i < n / 4; i++) {
        int x = 0;
        for (int j = 0; j < 4; j++) {
            x += power(2, 3 - j) * (bin[i * 4 + j] - '0');
        }
        if (x >= 10) {
            hex[i] = (char)('A' + x - 10);
        }
        else {
            hex[i] = (char)('0' + x);
        }
    }
    return hex;
}

int symbolLookup(char* label) {
    if (label[0] == 'x' || isOpcode(label) == 1) {
        exitAndClose(4); // Only Labels can be operands for symbol lookup
    }
    char *labelPtr = label;
    while (*labelPtr != '\0') {
        if (!isalnum(*(labelPtr++))) {
            exitAndClose(4); // Only Labels can be operands for symbol lookup
        }
    }
    for (int i = 0; i < size; i++) {
        if (strcmp(label, symbolTable[i].label) == 0) {
            return symbolTable[i].address;
        }
    }
    exitAndClose(1); // UNDEFINED LABEL
}

int checkRegisterArg(char* arg) {
    if (arg[0] != 'r' || arg[1] == '\0' || arg[1] > '7' || arg[1] < '0' || arg[2] != '\0') {
        exitAndClose(4);
    }
    return 1;
}

int main(int argc, char* argv[]) {
    /* open the source file */
    infile = fopen(argv[1], "r");
    outfile = fopen(argv[2], "w");

    if (!infile) {
        printf("Error: Cannot open file %s\n", argv[1]);
        exitAndClose(4);
    }
    if (!outfile) {
        printf("Error: Cannot open file %s\n", argv[2]);
        exitAndClose(4);
    }

    int lRet;
    int address;
    int start = -1;

    do // first pass: determine starting address and generate symbol table
    {
        char lLine[MAX_LINE_LENGTH + 1], *lLabel, *lOpcode, *lArg1, *lArg2, *lArg3, *lArg4;
        lRet = readAndParse( infile, lLine, &lLabel, &lOpcode, &lArg1, &lArg2, &lArg3, &lArg4 );
        if( lRet != DONE && lRet != EMPTY_LINE )
        {
            if (lOpcode == NULL && lLabel[0] != '\0') {
                exitAndClose(2); // LABEL ON ITS OWN
            }
            if (strcmp(lOpcode, ".orig") == 0) {
                if (!(lArg2 == NULL || lArg2[0] == ';')) {
                    exitAndClose(4);
                }
                address = toNum(lArg1);
                if (address % 2 != 0 || address > power(2, 16) - 1 || address < 0) {
                    exitAndClose(3);
                }
                start = address;
                fprintf( outfile, "0x%.4X\n", start );
            }
            else if (strcmp(lOpcode, ".end") == 0) {
                if (start == -1 || !(lArg1 == NULL || lArg1[0] == ';')) {
                    exitAndClose(4); // NO .ORIG OR .END HAS ADDITIONAL OPERANDS
                }
                break;
            }
            else if (lLabel[0] != '\0') {
                if (start == -1) {
                    exitAndClose(4); // NO .ORIG
                }
                if (strcmp(lOpcode, ".fill") != 0 && isOpcode(lOpcode) == -1) {
                    exitAndClose(2);
                }
                if (size == MAX_SYMBOLS || lOpcode[0] == '\0' || lLabel[0] == 'x' || isOpcode(lLabel) == 1) {
                    exitAndClose(4);
                }
                char *labelPtr = lLabel;
                while (*labelPtr != '\0') {
                    if (!isalnum(*(labelPtr++))) {
                        exitAndClose(4);
                    }
                }
                for (int i = 0; i < size; i++) {
                    if (strcmp(symbolTable[0].label, lLabel) == 0) {
                        exitAndClose(4); // label already exists
                    }
                }
                symbolTable[size].address = address;
                strcpy(symbolTable[size].label, lLabel);
                size++;
                address += 2;
            }
            else {
                address += 2;
            }
        }
        if (address < 0 || address > power(2, 16) - 1) {
            exitAndClose(4);
        }
    } while( lRet != DONE );

    rewind(infile);

    address = start;
    int done = 0;
    do // second pass: convert assembly instructions to machine code
    {
        if (address < 0 || address > power(2, 16) - 1) {
            exitAndClose(4);
        }
        char lLine[MAX_LINE_LENGTH + 1], *lLabel, *lOpcode, *lArg1, *lArg2, *lArg3, *lArg4;
        lRet = readAndParse( infile, lLine, &lLabel, &lOpcode, &lArg1, &lArg2, &lArg3, &lArg4 );
        if( lRet != DONE && lRet != EMPTY_LINE )
        {
            if (strcmp(lOpcode, ".orig") == 0) {
                continue;
            }
            else if (strcmp(lOpcode, ".end") == 0) {
                done = 1;
                break;
            }
            else if (strcmp(lOpcode, ".fill") == 0) {
                if (!(lArg2 == NULL || lArg2[0] == ';')) {
                    exitAndClose(4);
                }
                int constant = toNum(lArg1);
                if (constant > power(2, 16) - 1 || constant < -1 * power(2, 15)) {
                    exitAndClose(3);
                }
                if (constant < 0) {
                    char binary[17];
                    char signedHex[5];
                    binaryInstructionToHex(decToBinary(constant, binary, 16, SIGNED), signedHex, 16);
                    fprintf( outfile, "0x%s\n", signedHex);
                }
                else {
                    fprintf( outfile, "0x%.4X\n", constant);
                }
                address += 2;
            }
            else if (isOpcode(lOpcode) == 1) {
                char binary[17], hex[5], dr[4], sr1[4], sr2[4], br[4], pco9[10],
                     pco11[12], bo6[7], imm5[6], amt4[5], trapv8[9];
                binary[0] = '\0';
                hex[0] = '\0';
                if (strcmp(lOpcode, "add") == 0) {
                    if (!(lArg4 == NULL || lArg4[0] == ';')) {
                        exitAndClose(4);
                    }
                    strcat(binary, "0001");
                    checkRegisterArg(lArg1);
                    checkRegisterArg(lArg2);
                    strcat(binary, decToBinary(atoi(lArg1 + 1), dr, 3, NOT_SIGNED));
                    strcat(binary, decToBinary(atoi(lArg2 + 1), sr1, 3, NOT_SIGNED));
                    if (lArg3[0] == 'r') {
                        strcat(binary, "000");
                        checkRegisterArg(lArg3);
                        strcat(binary, decToBinary(atoi(lArg3 + 1), sr2, 3, NOT_SIGNED));
                    }
                    else {
                        strcat(binary, "1");
                        strcat(binary, decToBinary(toNum(lArg3), imm5, 5, SIGNED));
                    }
                }
                else if (strcmp(lOpcode, "and") == 0) {
                    if (!(lArg4 == NULL || lArg4[0] == ';')) {
                        exitAndClose(4);
                    }
                    strcat(binary, "0101");
                    checkRegisterArg(lArg1);
                    checkRegisterArg(lArg2);
                    strcat(binary, decToBinary(atoi(lArg1 + 1), dr, 3, NOT_SIGNED));
                    strcat(binary, decToBinary(atoi(lArg2 + 1), sr1, 3, NOT_SIGNED));
                    if (lArg3[0] == 'r') {
                        strcat(binary, "000");
                        checkRegisterArg(lArg3);
                        strcat(binary, decToBinary(atoi(lArg3 + 1), sr2, 3, NOT_SIGNED));
                    }
                    else {
                        strcat(binary, "1");
                        strcat(binary, decToBinary(toNum(lArg3), imm5, 5, SIGNED));
                    }
                }
                else if (strstr(lOpcode, "br") != NULL) {
                    if (!(lArg2 == NULL || lArg2[0] == ';')) {
                        exitAndClose(4);
                    }
                    if (isOpcode(lOpcode) == -1) {
                        exitAndClose(2);
                    }
                    strcat(binary, "0000");
                    if (strcmp(lOpcode, "br") == 0) {
                        strcat(binary, "111");
                    }
                    else {
                        if (strstr(lOpcode, "n") != NULL) {
                            strcat(binary, "1");
                        }
                        else {
                            strcat(binary, "0");
                        }
                        if (strstr(lOpcode, "z") != NULL) {
                            strcat(binary, "1");
                        }
                        else {
                            strcat(binary, "0");
                        }
                        if (strstr(lOpcode, "p") != NULL) {
                            strcat(binary, "1");
                        }
                        else {
                            strcat(binary, "0");
                        }
                    }
                    strcat(binary, decToBinary((symbolLookup(lArg1) - (address + 2)) / 2, pco9, 9, SIGNED));
                }
                else if (strcmp(lOpcode, "jmp") == 0) {
                    if (!(lArg2 == NULL || lArg2[0] == ';')) {
                        exitAndClose(4);
                    }
                    strcat(binary, "1100000");
                    checkRegisterArg(lArg1);
                    strcat(binary, decToBinary(atoi(lArg1 + 1), br, 3, NOT_SIGNED));
                    strcat(binary, "000000");
                }
                else if (strcmp(lOpcode, "ret") == 0) {
                    if (!(lArg1 == NULL || lArg1[0] == ';')) {
                        exitAndClose(4);
                    }
                    strcat(binary, "1100000111000000");
                }
                else if (strcmp(lOpcode, "jsr") == 0 || strcmp(lOpcode, "jsrr") == 0) {
                    if (!(lArg2 == NULL || lArg2[0] == ';')) {
                        exitAndClose(4);
                    }
                    strcat(binary, "0100");
                    if (strcmp(lOpcode, "jsr") == 0) {
                        if (((symbolLookup(lArg1) - (address + 2)) / 2 > power(2, 10) - 1) || ((symbolLookup(lArg1) - (address + 2)) / 2 < -1 * power(2, 10))) {
                              exitAndClose(4);
                        }
                        strcat(binary, "1");
                        strcat(binary, decToBinary((symbolLookup(lArg1) - (address + 2)) / 2, pco11, 11, SIGNED));
                    }
                    else {
                        strcat(binary, "000");
                        checkRegisterArg(lArg1);
                        strcat(binary, decToBinary(atoi(lArg1 + 1), br, 3, NOT_SIGNED));
                        strcat(binary, "000000");
                    }
                }
                else if (strcmp(lOpcode, "ldb") == 0) {
                    if (!(lArg4 == NULL || lArg4[0] == ';')) {
                        exitAndClose(4);
                    }
                    strcat(binary, "0010");
                    checkRegisterArg(lArg1);
                    checkRegisterArg(lArg2);
                    strcat(binary, decToBinary(atoi(lArg1 + 1), dr, 3, NOT_SIGNED));
                    strcat(binary, decToBinary(atoi(lArg2 + 1), br, 3, NOT_SIGNED));
                    strcat(binary, decToBinary(toNum(lArg3), bo6, 6, SIGNED));
                }
                else if (strcmp(lOpcode, "ldw") == 0) {
                    if (!(lArg4 == NULL || lArg4[0] == ';')) {
                        exitAndClose(4);
                    }
                    strcat(binary, "0110");
                    checkRegisterArg(lArg1);
                    checkRegisterArg(lArg2);
                    strcat(binary, decToBinary(atoi(lArg1 + 1), dr, 3, NOT_SIGNED));
                    strcat(binary, decToBinary(atoi(lArg2 + 1), br, 3, NOT_SIGNED));
                    strcat(binary, decToBinary(toNum(lArg3), bo6, 6, SIGNED));
                }
                else if (strcmp(lOpcode, "lea") == 0) {
                    if (!(lArg3 == NULL || lArg3[0] == ';')) {
                        exitAndClose(4);
                    }
                    if (((symbolLookup(lArg2) - (address + 2)) / 2 > power(2, 8) - 1) || ((symbolLookup(lArg2) - (address + 2)) / 2 < -1 * power(2, 8))) {
                        exitAndClose(4);
                    }
                    strcat(binary, "1110");
                    checkRegisterArg(lArg1);
                    strcat(binary, decToBinary(atoi(lArg1 + 1), dr, 3, NOT_SIGNED));
                    strcat(binary, decToBinary((symbolLookup(lArg2) - (address + 2)) / 2, pco9, 9, SIGNED));
                }
                else if (strcmp(lOpcode, "rti") == 0) {
                    if (!(lArg1 == NULL || lArg1[0] == ';')) {
                        exitAndClose(4);
                    }
                    strcat(binary, "1000000000000000");
                }
                else if (strcmp(lOpcode, "lshf") == 0) {
                    if (!(lArg4 == NULL || lArg4[0] == ';')) {
                        exitAndClose(4);
                    }
                    strcat(binary, "1101");
                    checkRegisterArg(lArg1);
                    checkRegisterArg(lArg2);
                    strcat(binary, decToBinary(atoi(lArg1 + 1), dr, 3, NOT_SIGNED));
                    strcat(binary, decToBinary(atoi(lArg2 + 1), sr1, 3, NOT_SIGNED));
                    strcat(binary, "00");
                    strcat(binary, decToBinary(toNum(lArg3), amt4, 4, NOT_SIGNED));
                }
                else if (strcmp(lOpcode, "rshfl") == 0) {
                    if (!(lArg4 == NULL || lArg4[0] == ';')) {
                        exitAndClose(4);
                    }
                    strcat(binary, "1101");
                    checkRegisterArg(lArg1);
                    checkRegisterArg(lArg2);
                    strcat(binary, decToBinary(atoi(lArg1 + 1), dr, 3, NOT_SIGNED));
                    strcat(binary, decToBinary(atoi(lArg2 + 1), sr1, 3, NOT_SIGNED));
                    strcat(binary, "01");
                    strcat(binary, decToBinary(toNum(lArg3), amt4, 4, NOT_SIGNED));
                }
                else if (strcmp(lOpcode, "rshfa") == 0) {
                    if (!(lArg4 == NULL || lArg4[0] == ';')) {
                        exitAndClose(4);
                    }
                    strcat(binary, "1101");
                    checkRegisterArg(lArg1);
                    checkRegisterArg(lArg2);
                    strcat(binary, decToBinary(atoi(lArg1 + 1), dr, 3, NOT_SIGNED));
                    strcat(binary, decToBinary(atoi(lArg2 + 1), sr1, 3, NOT_SIGNED));
                    strcat(binary, "11");
                    strcat(binary, decToBinary(toNum(lArg3), amt4, 4, NOT_SIGNED));
                }
                else if (strcmp(lOpcode, "stb") == 0) {
                    if (!(lArg4 == NULL || lArg4[0] == ';')) {
                        exitAndClose(4);
                    }
                    strcat(binary, "0011");
                    checkRegisterArg(lArg1);
                    checkRegisterArg(lArg2);
                    strcat(binary, decToBinary(atoi(lArg1 + 1), sr1, 3, NOT_SIGNED));
                    strcat(binary, decToBinary(atoi(lArg2 + 1), br, 3, NOT_SIGNED));
                    strcat(binary, decToBinary(toNum(lArg3), bo6, 6, SIGNED));
                }
                else if (strcmp(lOpcode, "stw") == 0) {
                    if (!(lArg4 == NULL || lArg4[0] == ';')) {
                        exitAndClose(4);
                    }
                    strcat(binary, "0111");
                    checkRegisterArg(lArg1);
                    checkRegisterArg(lArg2);
                    strcat(binary, decToBinary(atoi(lArg1 + 1), sr1, 3, NOT_SIGNED));
                    strcat(binary, decToBinary(atoi(lArg2 + 1), br, 3, NOT_SIGNED));
                    strcat(binary, decToBinary(toNum(lArg3), bo6, 6, SIGNED));
                }
                else if (strcmp(lOpcode, "trap") == 0) {
                    if (!(lArg2 == NULL || lArg2[0] == ';')) {
                        exitAndClose(4);
                    }
                    strcat(binary, "11110000");
                    if (lArg1[0] != 'x') {
                        exitAndClose(4);
                    }
                    strcat(binary, decToBinary(toNum(lArg1), trapv8, 8, NOT_SIGNED));
                }
                else if (strcmp(lOpcode, "halt") == 0) {
                    if (!(lArg1 == NULL || lArg1[0] == ';')) {
                        exitAndClose(4);
                    }
                    strcat(binary, "1111000000100101");
                }
                else if (strcmp(lOpcode, "getc") == 0) {
                    if (!(lArg1 == NULL || lArg1[0] == ';')) {
                        exitAndClose(4);
                    }
                    strcat(binary, "1111000000100000");
                }
                else if (strcmp(lOpcode, "out") == 0) {
                    if (!(lArg1 == NULL || lArg1[0] == ';')) {
                        exitAndClose(4);
                    }
                    strcat(binary, "1111000000100001");
                }
                else if (strcmp(lOpcode, "puts") == 0) {
                    if (!(lArg1 == NULL || lArg1[0] == ';')) {
                        exitAndClose(4);
                    }
                    strcat(binary, "1111000000100010");
                }
                else if (strcmp(lOpcode, "in") == 0) {
                    if (!(lArg1 == NULL || lArg1[0] == ';')) {
                        exitAndClose(4);
                    }
                    strcat(binary, "1111000000100011");
                }
                else if (strcmp(lOpcode, "xor") == 0) {
                    strcat(binary, "1001");
                    checkRegisterArg(lArg1);
                    checkRegisterArg(lArg2);
                    strcat(binary, decToBinary(atoi(lArg1 + 1), dr, 3, NOT_SIGNED));
                    strcat(binary, decToBinary(atoi(lArg2 + 1), sr1, 3, NOT_SIGNED));
                    if (lArg3[0] == 'r') {
                        if (!(lArg4 == NULL || lArg4[0] == ';')) {
                            exitAndClose(4);
                        }
                        strcat(binary, "000");
                        checkRegisterArg(lArg3);
                        strcat(binary, decToBinary(atoi(lArg3 + 1), sr2, 3, NOT_SIGNED));
                    }
                    else {
                        if (!(lArg4 == NULL || lArg4[0] == ';')) {
                            exitAndClose(4);
                        }
                        strcat(binary, "1");
                        strcat(binary, decToBinary(toNum(lArg3), imm5, 5, SIGNED));
                    }
                }
                else if (strcmp(lOpcode, "not") == 0) {
                    if (!(lArg3 == NULL || lArg3[0] == ';')) {
                        exitAndClose(4);
                    }
                    strcat(binary, "1001");
                    checkRegisterArg(lArg1);
                    checkRegisterArg(lArg2);
                    strcat(binary, decToBinary(atoi(lArg1 + 1), dr, 3, NOT_SIGNED));
                    strcat(binary, decToBinary(atoi(lArg2 + 1), sr1, 3, NOT_SIGNED));
                    strcat(binary, "111111");
                }
                else if (strcmp(lOpcode, "nop") == 0) {
                    if (!(lArg1 == NULL || lArg1[0] == ';')) {
                        exitAndClose(4);
                    }
                    strcat(binary, "0000000000000000");
                }
                binaryInstructionToHex(binary, hex, 16);
                fprintf( outfile, "0x%s\n", hex);
                address += 2;
            }
            else {
                exitAndClose(2); // INVALID OPCODE
            }
        }
    } while( lRet != DONE );
    if (done != 1) {
        exitAndClose(4); // NO .END
    }
    exitAndClose(0);
}
