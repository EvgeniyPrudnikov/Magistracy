
#include "NSMongoStorage.h"

NSMongoStorage::~NSMongoStorage() = default;

NSMongoStorage::NSMongoStorage(std::string host, std::string serverPort)
{
    std::string mongoPort;
    std::string mongoServerAddress;

    mongoPort = serverPort == "80" ? "27017" : serverPort == "2564" ? "27018" : NULL;

    mongoServerAddress = "mongodb://" + host + ":" + mongoPort;


    uri = new mongocxx::uri(mongoServerAddress);

    client = new mongocxx::client(*uri);

}

std::string NSMongoStorage::GetNewsCollection(std::vector<std::string>& params)
{
    std::string newsCollJsonString = "";
    mongocxx::database db = (*client)["NewsDB"];
    mongocxx::collection coll = db["NewsCollection"];

    mongocxx::cursor cursor = coll.find(document{} << finalize);
    for(auto doc : cursor) {
        newsCollJsonString += bsoncxx::to_json(doc) + "\n";
    }
    return newsCollJsonString;
}

std::string NSMongoStorage::GetNewsItem(std::string& NewsId)
{
    std::string newsInstanceJsonString = "";
    mongocxx::database db = (*client)["NewsDB"];
    mongocxx::collection coll = db["NewsCollection"];

    mongocxx::stdx::optional<bsoncxx::document::value> maybe_result =
            coll.find_one(document{} << "Nid" << NewsId << finalize);

    if(maybe_result)
    {
        newsInstanceJsonString += bsoncxx::to_json(*maybe_result) + "\n";
    }

    return newsInstanceJsonString;
}

