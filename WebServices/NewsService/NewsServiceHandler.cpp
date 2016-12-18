
#include "NewsServiceHandler.h"

const unsigned long NEWS_PREFIX_SIZE = sizeof("/news/") - 2;

bool IstNewsCollection(const std::string &path, const std::string &method)
{
    return path == "/news" && method == "GET";
}

bool IsNewsInstance(const std::string &path, const std::string &method, std::string *NewsId)
{
    if (method == "GET" && path.find("/news/") == 0)
    {
        *NewsId = path.substr(NEWS_PREFIX_SIZE);
        return true;
    }
    return false;
}

NewsServiceHandler::NewsServiceHandler(fastcgi::ComponentContext *context)
{
    int fastcgiPort = context->getConfig()->asInt("/fastcgi/daemon/monitor_port");
    this->mongoStorage = new NSMongoStorage(fastcgiPort);
}

NewsServiceHandler::~NewsServiceHandler() = default;

void NewsServiceHandler::ScheduleRequest(fastcgi::Request *request, fastcgi::HandlerContext *context)
{
    auto &path = request->getScriptName();
    auto &method = request->getRequestMethod();

    request->setContentType("application/json");

    std::string NewsId;

    if (IstNewsCollection(path, method))
    {
        std::vector<std::string> params;
        params.push_back(request->hasArg("start_dt") ? request->getArg("start_dt") : "NULL");
        params.push_back(request->hasArg("end_dt") ? request->getArg("end_dt") : "NULL");
        params.push_back(request->hasArg("source_id") ? request->getArg("source_id") : "NULL");

        bool missingParam = any_of(params.begin(), params.end(), [](std::string i) { return i == "NULL"; });

        if (!missingParam)
        {
            RespondNewsCollection(request, mongoStorage, params);

        } else
        {
            request->setStatus(404);
        }

    } else if (IsNewsInstance(path, method, &NewsId))
    {
        if (!NewsId.empty())
        {
            RespondNewsInstance(request, mongoStorage, NewsId);

        } else
        {
            request->setStatus(404);
        }

    } else
    {
        request->setStatus(404);
    }
}

void NewsServiceHandler::RespondNewsCollection(fastcgi::Request *request, NSMongoStorage *mongoStorage,
                                               std::vector<std::string> &params)
{
    std::string response;

    response = mongoStorage->GetNewsCollection(params);

    request->write(response.c_str(), response.size());

}

void
NewsServiceHandler::RespondNewsInstance(fastcgi::Request *request, NSMongoStorage *mongoStorage, std::string& NewsId)
{
    std::string response;

    response = mongoStorage->GetNewsItem(NewsId);

    request->write(response.c_str(), response.size());

}






