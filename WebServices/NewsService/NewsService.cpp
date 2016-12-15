#include <fastcgi2/component.h>
#include <fastcgi2/component_factory.h>
#include <fastcgi2/handler.h>
#include <fastcgi2/request.h>
#include <iostream>

class NewsService : virtual public fastcgi::Component, virtual public fastcgi::Handler
{

public:
        NewsService(fastcgi::ComponentContext *context) : fastcgi::Component(context)
        {
        }
        virtual ~NewsService()
        {
        }

public:
        virtual void onLoad()
        {
        }
        virtual void onUnload()
        {
        }
        virtual void handleRequest(fastcgi::Request *request, fastcgi::HandlerContext *context)
        {
        	request->write((char *)"Hello World\n",12);
        }
};

FCGIDAEMON_REGISTER_FACTORIES_BEGIN()
FCGIDAEMON_ADD_DEFAULT_FACTORY("service_factory", NewsService)
FCGIDAEMON_REGISTER_FACTORIES_END()