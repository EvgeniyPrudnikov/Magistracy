#include <fastcgi2/handler.h>
#include <fastcgi2/request.h>

#include <string>

#include "NSMongoStorage.h"


class NewsServiceHandler
{
public:

    NSMongoStorage* mongoStorage = NULL;

    NewsServiceHandler();
    ~NewsServiceHandler();

    void ScheduleRequest(fastcgi::Request *request, fastcgi::HandlerContext *context);

private:

    void RespondNewsCollection(fastcgi::Request *request, NSMongoStorage* mongoStorage);
    void RespondNewsInstance(fastcgi::Request *request, NSMongoStorage* mongoStorage, std::string NewsId);

};
