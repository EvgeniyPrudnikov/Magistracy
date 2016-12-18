#include <fastcgi2/config.h>
#include <fastcgi2/component.h>
#include <fastcgi2/handler.h>
#include <fastcgi2/request.h>
#include <vector>
#include <string>

#include "NSMongoStorage.h"


class NewsServiceHandler
{
public:

    NSMongoStorage* mongoStorage;

    NewsServiceHandler(fastcgi::ComponentContext *context);
    ~NewsServiceHandler();

    void ScheduleRequest(fastcgi::Request *request, fastcgi::HandlerContext *context);

private:

    void RespondNewsCollection(fastcgi::Request *request, NSMongoStorage* mongoStorage, std::vector<std::string>& params);
    void RespondNewsInstance(fastcgi::Request *request, NSMongoStorage* mongoStorage, std::string& NewsId);

};
