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



    NewsServiceHandler(fastcgi::ComponentContext *context);
    ~NewsServiceHandler();

    void ScheduleRequest(fastcgi::Request *request);

private:

    NSMongoStorage* mongoStorage;

    void RespondNewsCollection(fastcgi::Request *request, std::vector<std::string>& params);
    void RespondNewsInstance(fastcgi::Request *request, std::string& NewsId);

};
