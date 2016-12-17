
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

std::string NSMongoStorage::GetNewsCollection(std::vector<std::string> &params)
{
    std::string newsCollJsonString = "";
    mongocxx::database db = (*client)["NewsDB"];
    mongocxx::collection coll = db["NewsCollection"];

    std::string start_dt = params[0];
    int days_diff = std::stoi(params[1]);
    std::string source_id = params[2];

    std::chrono::time_point<std::chrono::system_clock> lowerDate =
            getDateFromString(start_dt) + std::chrono::hours(24 * days_diff);

    bsoncxx::document::value *query_value;
    if (!source_id.empty())
    {
        query_value = new bsoncxx::document::value(
                document{} << "news_date" << open_document << "$gt" << bsoncxx::types::b_date{lowerDate}
                           << close_document
                           << "source_uri" << open_document << "$eq" << source_id << close_document << finalize);
    } else
    {
        query_value = new bsoncxx::document::value(
                document{} << "news_date" << open_document << "$gt" << bsoncxx::types::b_date{lowerDate}
                           << close_document << finalize);
    }

    mongocxx::cursor cursor = coll.find(query_value->view());
    for (auto doc : cursor)
    {
        newsCollJsonString += bsoncxx::to_json(doc) + "\n";
    }
    return newsCollJsonString;
}

std::string NSMongoStorage::GetNewsItem(std::string &NewsId)
{
    std::string newsInstanceJsonString = "";
    mongocxx::database db = (*client)["NewsDB"];
    mongocxx::collection coll = db["NewsCollection"];

    bsoncxx::document::value query_value = document{} << "Nid" << NewsId << finalize;
    mongocxx::stdx::optional<bsoncxx::document::value> maybe_result = coll.find_one(query_value.view());

    if (maybe_result)
    {
        newsInstanceJsonString += bsoncxx::to_json(*maybe_result) + "\n";
    }

    return newsInstanceJsonString;
}

std::chrono::time_point<std::chrono::system_clock> NSMongoStorage::getDateFromString(std::string &date)
{
    std::tm tm = {};
    strptime(date.c_str(), "%Y-%m-%dT%H:%M:%SZ", &tm);
    return std::chrono::system_clock::from_time_t(std::mktime(&tm));
}

