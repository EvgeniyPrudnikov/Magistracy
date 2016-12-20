#include <cstdint>
#include <chrono>
#include <string>
#include <iostream>
#include <vector>
#include <bsoncxx/json.hpp>
#include <mongocxx/client.hpp>
#include <mongocxx/stdx.hpp>
#include <mongocxx/uri.hpp>
#include <bsoncxx/types.hpp>

using bsoncxx::builder::stream::close_array;
using bsoncxx::builder::stream::close_document;
using bsoncxx::builder::stream::document;
using bsoncxx::builder::stream::finalize;
using bsoncxx::builder::stream::open_array;
using bsoncxx::builder::stream::open_document;


class NSMongoStorage
{
public:

    NSMongoStorage(int fastcgiPort);

    ~NSMongoStorage();

    std::string GetNewsCollection(std::vector<std::string>& params);

    std::string GetNewsItem( std::string& NewsId);

private:

    mongocxx::uri uri;
    thread_local static mongocxx::client client;
    std::chrono::time_point<std::chrono::system_clock> getDateFromString(std::string& date);

};