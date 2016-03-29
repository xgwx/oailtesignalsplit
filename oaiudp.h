#include <sys/socket.h>
#include <netinet/in.h>


#define OAI_PORT 8888
#define MAXLINE 1024
#define DBS_NUM 10

extern int oaisockfd;

extern struct sockaddr_in oaiservaddr;

extern struct dbsstat[DBS_NUM];

extern int bsstart;
