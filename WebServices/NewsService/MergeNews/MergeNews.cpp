#define ROOT "items"
#define ID "news_uri"

#include <iostream>
#include <json/json.h>
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


std::string GetNewsCollectionForMerge(std::string uriString, int days_diff_to_now)
{
    std::string newsCollJsonString = "{\"items\": [";
    mongocxx::uri uri (uriString);
    mongocxx::client client (uri);
    mongocxx::database db = client["NewsDB"];
    mongocxx::collection coll = db["NewsCollection"];


    std::chrono::time_point<std::chrono::system_clock> lowerDate = std::chrono::system_clock::now() + std::chrono::hours(24 * days_diff_to_now);

    bsoncxx::document::value query_value =
                document{} << "news_date" << open_document << "$gt" << bsoncxx::types::b_date{lowerDate}
                           << close_document << finalize;


    mongocxx::cursor cursor = coll.find(query_value.view());
    for (auto doc : cursor)
    {
        newsCollJsonString += bsoncxx::to_json(doc) + ",";
    }
    newsCollJsonString.pop_back();

    return newsCollJsonString + "]}\n";
}

void WriteDiffData(std::string uriString, std::vector<Json::Value>& diff)
{
    mongocxx::uri uri (uriString);
    mongocxx::client client (uri);
    mongocxx::database db = client["NewsDB"];
    mongocxx::collection coll = db["NewsCollection"];

    std::vector<bsoncxx::document::value> documents;
    Json::FastWriter writer;
    for(auto item_diff:diff)
    {
        std::cerr<<item_diff<<std::endl;
        documents.push_back(bsoncxx::from_json(writer.write(item_diff)));
    }
    coll.insert_many(documents);
}


void merge(Json::Value& a, const Json::Value& b , std::vector<Json::Value>& diff)
{

    for (const Json::Value& item_b:b[ROOT])
    {
        bool find = false;
        for(Json::Value& item_a:a[ROOT])
        {
            if (item_a[ID].asString() == item_b[ID].asString())
            {
                find = true;
                continue;
            }
        }
        if (!find)
        {
            diff.push_back(item_b);
        }
    }
}


int main(int argc, char* argv[])
{

    if (argc != 4)
    {
        std::cerr
                << "Wrong number of arguments!\nUsage ./MergeNews HOST_DB_INTO_MERGE:PORT_DB_INTO_MERGE HOST_DB_FROM_MERGE:PORT_DB_FROM_MERGE DAYS_DIFF_TO_NOW"<<std::endl;
        return 1;
    }

    std::string mongoServerAddressInto = !std::string(argv[1]).empty() ? "mongodb://" + std::string(argv[1]):"NULL";
    std::string mongoServerAddressFrom = !std::string(argv[2]).empty() ? "mongodb://" + std::string(argv[2]):"NULL";
    int days_diff_to_now = atoi(argv[3]);

    if(mongoServerAddressInto == "NULL" || mongoServerAddressFrom == "NULL")
    {
        throw std::invalid_argument("Wrong mongo address");
    }

    std::string NewsJsonStrDBInto = GetNewsCollectionForMerge(mongoServerAddressInto, days_diff_to_now);
    std::string NewsJsonStrDBFrom = GetNewsCollectionForMerge(mongoServerAddressFrom, days_diff_to_now);

    Json::Reader reader;

    Json::Value NewsJsonDBInto;
    Json::Value NewsJsonDBFrom;
    bool parsingSuccessfulJsInto = reader.parse(NewsJsonStrDBInto, NewsJsonDBInto);
    bool parsingSuccessfulJsFrom = reader.parse(NewsJsonStrDBFrom, NewsJsonDBFrom);

    if (!parsingSuccessfulJsInto || !parsingSuccessfulJsFrom)
    {
        throw "Json parsing error";
    }

    std::vector<Json::Value> diff;

    merge(NewsJsonDBInto, NewsJsonDBFrom, diff);

    if (!diff.empty())
    {
        WriteDiffData(mongoServerAddressInto, diff);
        std::cerr<<"Data merged successfully!"<<std::endl;
    }else
    {
        std::cerr<<"Data is the same!"<<std::endl;
    }

    return 0;
}