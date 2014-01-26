#ifndef __UPNPREPLYPARSE_H__
#define __UPNPREPLYPARSE_H__

#ifndef WIN32
#include <sys/queue.h>
#endif

struct NameValue {
    LIST_ENTRY(NameValue) entries;
    char name[64];
    char value[64];
};

struct NameValueParserData {
    LIST_HEAD(listhead, NameValue) head;
    char curelt[64];
};

void ParseNameValue(const char * buffer, int bufsize,
                    struct NameValueParserData * data);

void ClearNameValueList(struct NameValueParserData * pdata);

char * GetValueFromNameValueList(struct NameValueParserData * pdata, const char * Name);

void DisplayNameValueList(char * buffer, int bufsize);

#endif

