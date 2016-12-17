#include <cstdint>
#include <string>
#include <iostream>
#include <vector>
#include <bsoncxx/json.hpp>
#include <mongocxx/client.hpp>
#include <mongocxx/stdx.hpp>
#include <mongocxx/uri.hpp>

using bsoncxx::builder::stream::close_array;
using bsoncxx::builder::stream::close_document;
using bsoncxx::builder::stream::document;
using bsoncxx::builder::stream::finalize;
using bsoncxx::builder::stream::open_array;
using bsoncxx::builder::stream::open_document;


class NSMongoStorage
{
public:
    NSMongoStorage(std::string host, std::string serverPort);

    ~NSMongoStorage();

    std::string GetNewsCollection(std::vector<std::string>& params);

    std::string GetNewsItem(std::string& NewsId);

private:
    mongocxx::uri *uri;
    mongocxx::client *client;
};
