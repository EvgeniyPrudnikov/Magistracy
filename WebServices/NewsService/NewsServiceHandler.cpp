
#include "NewsServiceHandler.h"

const size_t POLL_PREFIX_SIZE = sizeof("/news/") - 1;

bool IstNewsCollection(const std::string &path, const std::string &method)
{
    return path == "/news" && method == "GET";
}

bool IsNewsInstance(const std::string &path, const std::string &method, std::string *NewsId)
{
    if (method == "GET" && path.find("/news/") == 0 && path.find('/', POLL_PREFIX_SIZE) == std::string::npos)
    {
        *NewsId = path.substr(POLL_PREFIX_SIZE);
        return true;
    }
    return false;
}

NewsServiceHandler::NewsServiceHandler()
{}

NewsServiceHandler::~NewsServiceHandler() = default;

void NewsServiceHandler::ScheduleRequest(fastcgi::Request *request, fastcgi::HandlerContext *context)
{
    this->mongoStorage = new NSMongoStorage(request->getHost(), std::to_string(request->getServerPort()));

    auto &path = request->getScriptName();
    auto &method = request->getRequestMethod();

    request->setContentType("application/json");

    std::string NewsId;

    if (IstNewsCollection(path, method))
    {
        RespondNewsCollection(request, mongoStorage);

    } else if (IsNewsInstance(path, method, &NewsId))
    {
        RespondNewsInstance(request,mongoStorage,NewsId);

    } else
    {
        request->setStatus(404);
    }
}

void NewsServiceHandler::RespondNewsCollection(fastcgi::Request *request, NSMongoStorage *mongoStorage)
{
    std::string response;

    response = mongoStorage->GetNewsCollection();

    request->write(response.c_str(), response.size());

}

void NewsServiceHandler::RespondNewsInstance(fastcgi::Request *request, NSMongoStorage *mongoStorage, std::string NewsId)
{
    std::string response;

    response = mongoStorage->GetNewsItem(NewsId);

    request->write(response.c_str(), response.size());

}






