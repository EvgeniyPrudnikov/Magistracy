
#include "NSMongoStorage.h"

boost::thread_specific_ptr<mongocxx::client> NSMongoStorage::client;

NSMongoStorage::~NSMongoStorage() = default;

NSMongoStorage::NSMongoStorage(int fastcgiPort)
{
    std::string mongoPort;
    std::string mongoServerAddress;
    mongoPort = fastcgiPort == 20011 ? "27017" : fastcgiPort == 20012 ? "27018" : NULL;

    if (!mongoPort.empty())
    {
        mongoServerAddress = "mongodb://localhost:" + mongoPort;

    }else
    {
        throw std::invalid_argument("NULL port for mongoDB");
    }

    uri = mongocxx::uri(mongoServerAddress);
    client.reset(new mongocxx::client(uri));
}

std::string NSMongoStorage::GetNewsCollection( std::vector<std::string> &params)
{
    std::string newsCollJsonString = "{\"items\":[ ";

    mongocxx::client& locClient = getClient();

    mongocxx::database db = locClient["NewsDB"];
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
        newsCollJsonString += bsoncxx::to_json(doc) + ",";
    }
    newsCollJsonString.pop_back();
    return newsCollJsonString + "]}\n";
}

std::string NSMongoStorage::GetNewsItem(std::string &NewsId)
{
    std::string newsInstanceJsonString = "{\"items\":[ ";

    mongocxx::client& localClient = getClient();

    mongocxx::database db = localClient["NewsDB"];
    mongocxx::collection coll = db["NewsCollection"];

    bsoncxx::document::value query_value = document{} << "news_uri" << NewsId << finalize;
    mongocxx::stdx::optional<bsoncxx::document::value> maybe_result = coll.find_one(query_value.view());

    if (maybe_result)
    {
        newsInstanceJsonString += bsoncxx::to_json(*maybe_result) + "\n";
    }

    return newsInstanceJsonString + "]}\n";
}

std::chrono::time_point<std::chrono::system_clock> NSMongoStorage::getDateFromString(std::string &date)
{
    std::tm tm = {};
    strptime(date.c_str(), "%Y-%m-%dT%H:%M:%SZ", &tm);
    return std::chrono::system_clock::from_time_t(std::mktime(&tm));
}

