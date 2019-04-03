
#ifndef READER_CPP_
#define READER_CPP_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

class CReader {

public:
    CReader(void) { init(); };

    char *textFileRead(const char *chFileName);

private:
    void init(void);

    FILE *_fp;
    char *_content;
    int _count;

};

#endif
