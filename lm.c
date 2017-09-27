#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<math.h>
#include<time.h>
#define MAXLENGTH 99999999
#define NGRAM 4

void parse_arguments(int argC, char **argV);
void writeNGrams(FILE *, unsigned int, unsigned int *,char **, int);

/**
 DEFAULT Value of nGram is 4
 **/
int nValue = NGRAM;
unsigned int getSuffixArray(FILE * fin, char superstring[MAXLENGTH],
                            char *sufArr[MAXLENGTH]) {
    /**
     TODO - Exception Handling for any errors or exceptions
     **/
    int counter;
    unsigned int i, ssl = 2;
    unsigned int disp;
    fread(&ssl, sizeof(unsigned int), 1, fin);
    for (i = 0; i < ssl; i++) {
        fread((void *) &disp, sizeof(unsigned int), 1, fin);
        sufArr[i] = superstring + disp;
    }
    fread(superstring, sizeof(char), ssl, fin);
    superstring[ssl] = '\0';
    //printf ("\n String is %s \n", superstring);
    fclose(fin);
    return ssl;
}
void getLCPArray(FILE * fin, unsigned int * lcp) {
    /**
     TODO - Exception Handling for any errors or exceptions
     **/
    unsigned int i, ssl;
    fread(&ssl, sizeof(unsigned int), 1, fin);
    fread((void *) lcp, sizeof(unsigned int), ssl, fin);
}

void getRankArray(FILE * fin, unsigned int * rank) {
    /**
     TODO - Exception Handling for any errors or exceptions
     **/
    unsigned int i, ssl;
    fread(&ssl, sizeof(unsigned int), 1, fin);
    fread((void *) rank, sizeof(unsigned int), ssl, fin);
}
int cmpfunc(const void * a, const void * b) {
    return (*(unsigned int*) a - *(unsigned int*) b);
}
/**
 start - starting index of the window
 end - ending index of the window
 ngramCount - array to store the count of each ngrams
 ngramList  - 2d array to store the actual ngrams
 nValue = value of ngram
 
 **/
unsigned int generateFasterNGram(unsigned int * ngramCount, char ** ngramList,
                                 char * superstring, char ** ptr, unsigned int ssl, unsigned int * lcp,
                                 unsigned int * rank, int nValue, unsigned int start, unsigned int end) {
    
    int auxArraySize = end - start + 1;
    printf("NGramCalculation Begins for windowLen: %d\n", auxArraySize);
    if (auxArraySize < nValue) {
        return 0;
    }
    unsigned int * auxArray = (unsigned int *) malloc(
                                                      sizeof(unsigned int) * auxArraySize);
    int i = 0, j = 0;
    for (i = start; i <= end; i++) {
        auxArray[i - start] = rank[i];
    }
    
    qsort(auxArray, auxArraySize, sizeof(unsigned int), cmpfunc);
    int count = 0, uniqueCount = 1;
    int initFlag = 0;
    char prevNgram[nValue + 1], currNgram[nValue + 1];
    for (i = 0; i < auxArraySize; i++) {
        unsigned int ind = auxArray[i];
        unsigned int suffixInd = ptr[ind] - superstring;
        unsigned int windowLen = end - suffixInd + 1;
        if (windowLen < nValue) {
            continue;
        }
        if ((suffixInd > end || suffixInd < start)) {
            continue;
        }
        for (j = 0; j < nValue; j++) {
            if (ptr[ind][j] == ' ' || j == ssl - 1) {
                break;
            }
            if (j == nValue - 1) {
                
                if (initFlag) {
                    strncpy(currNgram, ptr[ind], nValue);
                    currNgram[nValue] = '\0';
                    if (strcmp(currNgram, prevNgram) == 0) {
                        count--;
                    } else {
                        uniqueCount = 1;
                    }
                }
                
                ngramList[count] = ptr[ind];
                ngramCount[count] = uniqueCount;
                uniqueCount++;
                strncpy(prevNgram, ptr[ind], nValue);
                prevNgram[nValue] = '\0';
                count++;
                initFlag = 1;
            }
        }
    }
    printf("No of distinct ngrams is: %u\n", count);
    return count;
    
}
void writeNGrams(FILE * fout, unsigned int totalNo, unsigned int * ngramCount,
                 char ** ngramList, int nValue) {
    
    fprintf(fout, "%u\n\n", totalNo);
    char ngram[nValue + 1];
    unsigned int i = 1;
    for (i = 0; i < totalNo; i++) {
        strncpy(ngram, ngramList[i], nValue);
        ngram[nValue] = '\0';
        fprintf(fout, "%u). %s %u\n", i, ngram, ngramCount[i]);
    }
    
}

unsigned int searchNgram(char ** str, char *substr, unsigned int l,
                         unsigned int r, int nValue) {
    unsigned int saveTotalNo = r;
    unsigned int mid;
    char ngram[nValue + 1];
    int i = 0;
    while (r > l + 1) {
        mid = l + (r - l) / 2;
        strncpy(ngram, str[mid], nValue);
        ngram[nValue] = '\0';
        if (strcmp(substr, ngram) == 0) {
            return mid;
        } else if (strcmp(substr, str[mid]) < 0) {
            r = mid;
        } else {
            l = mid;
        }
        i++;
        
    }
    if ((strcmp(substr, ngram) < 0) || (l + 1 >= saveTotalNo)) {
        return l;
    } else {
        return l + 1;
    }
    
}
unsigned int countN_1Gram(char * n_1gram, unsigned int * ngramCount,
                          char ** ngramList, unsigned int totalNo, int nValue) {
    unsigned int pos = searchNgram(ngramList, n_1gram, 0, totalNo, nValue + 1);
    unsigned int i = pos, count = 0;
    if (strncmp(n_1gram, ngramList[i], nValue) != 0) {
        i++;
    }
    while (i < totalNo && strncmp(n_1gram, ngramList[i], nValue) == 0) {
        count += (ngramCount[i] + 1);
        i++;
    }
    if (count == 0) {
        return 1;
    }
    return count;
}

double computePerplexity(unsigned int * ngramCountTest, char ** ngramListTest,
                         unsigned int * ngramCount, char ** ngramList, char * superstring,
                         char * superstring2, int nValue, int wi_startTest, int wi_endTest,
                         unsigned int totalNoTest, unsigned int totalNo, unsigned int ssl,
                         unsigned int ssl2) {
    printf("Perplexity Calculation Begins for %u unique NGrams\n", totalNoTest);
    unsigned int i = 0;
    char ngram[nValue + 1];
    char n_1gram[nValue];
    double res = 0;
    unsigned int saveLeft = 0;
    /**
     TODO - optimization in binary search for left index because both are already sorted
     do n-1gram first and then use base index and final index to search ngram
     
     error handling
     
     clarify doubt in formula E=−1N∑j=1Ctest−njlog2(Ctrain−njCtrain−(n−1)j)
     */
    double N = ssl2 - nValue + 1;
    for (i = 0; i < totalNoTest; i++) {
        strncpy(ngram, ngramListTest[i], nValue);
        strncpy(n_1gram, ngramListTest[i], nValue - 1);
        ngram[nValue] = '\0';
        n_1gram[nValue - 1] = '\0';
        unsigned int pos = searchNgram(ngramList, ngram, saveLeft, totalNo,
                                       nValue);
        saveLeft = pos;
        //printf ("\n Pos is %u\n",pos);
        double countN = 1;
        char ng[nValue];
        strncpy(ng, ngramList[pos], nValue);
        ng[nValue] = '\0';
        if (strcmp(ngram, ng) == 0) {
            // printf ("\n Here Reached %s  %u \n",ng,ngramCount[pos]);
            countN = ngramCount[pos] + 1;
        }
        
        double countN_1 = countN_1Gram(n_1gram, ngramCount, ngramList, totalNo,
                                       nValue - 1);
        double countNTest = ngramCountTest[i] + 1;
        // double countN_1Test=countN_1Gram(n_1gram,ngramCountTest,ngramListTest,totalNoTest,nValue-1);
        double val = log2(countN / countN_1);
        val = val * (countNTest);
        res += val;
    }
    res /= (N + totalNoTest);
    //	printf("\nFinal: %f\n", res);
    double perplexity = pow(2, -res);
    return perplexity;
}
char trainFileName[100], testFileName[100], trainNGramFile[100],
testNGramFile[100];
int main(int argc, char **argv) {
    char **ptr;
    char *superstring, *str, **ngramList;
    unsigned int *lcp, *ngramCount, *rank, tstart, tend, tdiff;
    superstring = (char *) malloc(sizeof(char) * MAXLENGTH);
    ptr = (char **) malloc(sizeof(char *) * MAXLENGTH);
    FILE *fin, *fin2, *fin3, *fin6, *fin4, *fin5;
    
    char prefixFileName[100], rankFileName[100], testprefixFileName[100],
    testrankFileName[100];
    parse_arguments(argc, argv);
    
    if(strlen(trainFileName)==0 ){
        printf ("\nERROR: Please pass the training fileName correcyly in the commandline\n");
        exit(0);
    }else{
        sprintf(prefixFileName, "%s.lcp", trainFileName);
        sprintf(rankFileName, "%s.rnk", trainFileName);
        fin = fopen(trainFileName, "rb");
        if (!fin){
            printf("ERROR: Unable to open file!");
            exit(0);
        }
        fin2 = fopen(rankFileName, "rb");
        if (!fin2){
            printf("ERROR: Unable to open file!");
            exit(0);
        }
        fin3 = fopen(prefixFileName, "rb");
        if (!fin3){
            printf("ERROR: Unable to open file!");
            exit(0);
        }
    }
    
    if(strlen(testFileName)==0 ){
        printf ("\nERROR: Please pass the test fileName correcyly in the commandline\n");
        exit(0);
    }else{
        sprintf(testprefixFileName, "%s.lcp", testFileName);
        sprintf(testrankFileName, "%s.rnk", testFileName);
        fin4 = fopen(testFileName, "rb");
        if (!fin4){
            printf("ERROR: Unable to open file!");
            exit(0);
        }
        fin5 = fopen(testrankFileName, "rb");
        if (!fin5){
            printf("ERROR: Unable to open file!");
            exit(0);
        }
        fin6 = fopen(testprefixFileName, "rb");
        if (!fin6){
            printf("ERROR: Unable to open file!");
            exit(0);
        }
    }
    
    printf("\nnGram Model Used: %d\n", nValue);
    printf("----------------------\n\n");
    
    /*************************************************
     Train Sequence
     *************************************************/
    unsigned int ssl = getSuffixArray(fin, superstring, ptr);
    lcp = (unsigned int *) malloc(sizeof(unsigned int) * ssl);
    rank = (unsigned int *) malloc(sizeof(unsigned int) * ssl);
    getRankArray(fin2, rank);
    getLCPArray(fin3, lcp);
    printf("ssl Length %d\n", ssl);
    
    int wi_start = 0, wi_end = ssl;
    int windowLength = wi_end - wi_start + 1;
    
    ngramList = (char **) malloc(sizeof(char *) * MAXLENGTH);
    ngramCount = (unsigned int *) malloc(sizeof(unsigned int) * windowLength);
    tstart = time(NULL);
    unsigned int totalNo = generateFasterNGram(ngramCount, ngramList,
                                               superstring, ptr, ssl, lcp, rank, nValue, wi_start, wi_end);
    tend = time(NULL);
    tdiff = tend - tstart;
    printf("Train Ngram Generation Time: %d %s.\n", tdiff,
           tdiff == 1 ? "second" : "seconds");
    
    /*************************************************
     Test Sequence
     *************************************************/    char **ptr2;
    char *superstring2, *str2, **ngramList2;
    unsigned int *lcp2, *ngramCount2, *rank2, tstart2, tend2, tdiff2;
    superstring2 = (char *) malloc(sizeof(char) * MAXLENGTH);
    ptr2 = (char **) malloc(sizeof(char *) * MAXLENGTH);
    
    unsigned int ssl2 = getSuffixArray(fin4, superstring2, ptr2);
    lcp2 = (unsigned int *) malloc(sizeof(unsigned int) * ssl2);
    rank2 = (unsigned int *) malloc(sizeof(unsigned int) * ssl2);
    printf("\n----------------------\n\n");
    printf("ssl2 Length %d\n", ssl2);
    getRankArray(fin5, rank2);
    getLCPArray(fin6, lcp2);
    
    int wi_start2 = 0, wi_end2 = ssl2;
    int windowLength2 = wi_end2 - wi_start2 + 1;
    
    ngramList2 = (char **) malloc(sizeof(char *) * MAXLENGTH);
    ngramCount2 = (unsigned int *) malloc(sizeof(unsigned int) * windowLength2);
    tstart2 = time(NULL);
    unsigned int totalNo2 = generateFasterNGram(ngramCount2, ngramList2,
                                                superstring2, ptr2, ssl2, lcp2, rank2, nValue, wi_start2, wi_end2);
    tend2 = time(NULL);
    tdiff2 = tend2 - tstart2;
    printf("Test Ngram Generation Time: %d %s.\n", tdiff2,
           tdiff2 == 1 ? "second" : "seconds");
    printf("\n----------------------\n\n");
    FILE *fout = NULL;
    if (strlen(trainNGramFile)) {
        fout = fopen(trainNGramFile, "w");
        if (!fout){
            printf("ERROR: Unable to open file!");
        }
        writeNGrams(fout, totalNo, ngramCount, ngramList, nValue);
        printf("train nGramns written to file %s\n",trainNGramFile);
        printf("\n----------------------\n\n");
    }
    
    if (strlen(testNGramFile)) {
        fout = fopen(testNGramFile, "w");
        if (!fout){
            printf("ERROR: Unable to open file!");
        }
        writeNGrams(fout, totalNo2, ngramCount2, ngramList2, nValue);
        printf("test nGramns written to file %s\n",testNGramFile);
        printf("\n----------------------\n\n");
    }
    
    tstart2 = time(NULL);
    
    /*************************************************
     Perplexity Calculation
     *************************************************/
    double p = computePerplexity(ngramCount2, ngramList2, ngramCount, ngramList,
                                 superstring, superstring2, nValue, wi_start2, wi_end2, totalNo2,
                                 totalNo, ssl, ssl2);
    printf("Perplexity: %lf \n", p);
    tend2 = time(NULL);
    tdiff2 = tend2 - tstart2;
    printf("Perplexity calculation Time: %d %s.\n\n", tdiff2,
           tdiff2 == 1 ? "second" : "seconds");
    printf("\n----------------------\n\n");
    return 0;
}
void parse_arguments(int argC, char **argV) {
    
    int i, j, strNum = 1, charNum;
    char *Switch[] = { "train", "test", "nGram", "trainNGram", "testNGram",
        "##END##" };
    typedef enum {
        train, test, nGram, trainNGram, testNGram, f_end
    } Params;
    trainFileName[0] = '\0';
    testFileName[0] = '\0';
    trainNGramFile[0] = '\0';
    testNGramFile[0] = '\0';
    while (strNum < argC) {
        if (argV[strNum][0] == '-') {
            for (i = 0; i < f_end; i++)
                if (!strcmp(argV[strNum] + 1, Switch[i]))
                    break;
            if (i == f_end) { /* The word doesnt match switch */
                fprintf(stderr, "\nInapproppriate Switch %s\tIgnored\n",
                        argV[strNum]);
                strNum++;
                continue;
            }
            
            switch (i) {
                case train:
                    strcpy(trainFileName, argV[strNum + 1]);
                    strNum += 2;
                    continue;
                case test:
                    strcpy(testFileName, argV[strNum + 1]);
                    strNum += 2;
                    continue;
                case nGram:
                    nValue = atoi(argV[strNum + 1]);
                    strNum += 2;
                    continue;
                case trainNGram:
                    strcpy(trainNGramFile, argV[strNum + 1]);
                    strNum += 2;
                    continue;
                case testNGram:
                    strcpy(testNGramFile, argV[strNum + 1]);
                    strNum += 2;
                    continue;
            }
        }
        strNum++; // This statement is reached only if the '-' didnt match above
    }
}
