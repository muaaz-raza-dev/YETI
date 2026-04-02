#include "../headers/libs.h"
#include <iomanip>
struct Data{
  string channelId;
  string id;
  string publishedAt;
  string title;
  long long commentCount;
  long long likeCount;
  long long subscriberCount;
  long long viewCount;
};

class ProcessData {
    private :

    tm to_timestamp(const string& s) {
      std::tm tm = {};
      std::istringstream ss(s);
      ss >> std::get_time(&tm, "%Y-%m-%dT%H:%M:%SZ");
      return tm;
    }

    public :
    ProcessData(){
    filesystem::current_path("F:/C&CPP Project/Yeti/app/Data Extraction");
    }
    bool removeDuplicates(){

      unordered_set<string> ids;
      json raw_data;
      ifstream inFile("data/glacier1.json");
      if(!inFile.is_open()){
        cout << "Can't able to read the data";
        return false;
      }
      inFile >> raw_data;
      json processed_data = {{"data",json::array()},{"count",0}};
      
      for(auto x:raw_data["data"]){
          if(ids.count(x["id"])) continue;
          if(x["subscriberCount"].is_null()) continue;
          ids.insert(x["id"]);
          json v = {
            {"id", x["id"]},
            {"publishedAt", x["publishedAt"]},
            {"channelId", x["channelId"]},
            {"title", x["title"]},
            {"viewCount", x["viewCount"]},
            {"likeCount", x["likeCount"]},
            {"subscriberCount", x["subscriberCount"]},
            {"commentCount", x["commentCount"]}
          };
          processed_data["data"].push_back(v);
      }
      processed_data["count"] = processed_data["data"].size();

      ofstream outFile("data/glacier2.json");
      if(!inFile.is_open()){
        cout << "Can't able to create the new file";
        return false;
      }

      outFile << processed_data.dump(4);
      outFile.close();
      inFile.close();
      cout << "only unique data is saved";
      return true;
    }
    bool FixType(){
      json data;
      ifstream inFile("data/glacier1.json");
      if(!inFile.is_open()){
        cout << "Can't able to read the data";
        return false;
      }
      inFile >> data;
      for(auto &x:data["data"]){
          x["commentCount"] = stoll(x["commentCount"].get<string>());
          x["likeCount"] = stoll(x["likeCount"].get<string>());
          x["subscriberCount"] = stoll(x["subscriberCount"].get<string>());
          x["viewCount"] = stoll(x["viewCount"].get<string>());
      }
    ofstream outFile("data/glacier2.json");
    if(!outFile.is_open()){
        cout << "Can't able to create the temp file";
        return false;
    }
    outFile << data.dump(4);
    outFile.close();
    inFile.close();
    cout << "Type has been fixed";
    return true;
  }
  bool processssDate(){
      json data;
      ifstream inFile("data/glacier1.json");
      if(!inFile.is_open()){
        cout << "Can't able to read the data";
        return false;
      }
      inFile >> data;
      for(auto &x:data["data"]){
        tm t =  to_timestamp(x["publishedAt"].get<string>());
        mktime(&t);

        x["publishedAtDetails"] = {};
        x["publishedAtDetails"]["day"] = t.tm_mday;
        x["publishedAtDetails"]["hour"] = t.tm_hour;
        x["publishedAtDetails"]["minute"] = t.tm_min;
        x["publishedAtDetails"]["day_of_week"] = t.tm_wday;
      }

    ofstream outFile("data/glacier2.json");
    if(!outFile.is_open()){
        cout << "Can't able to create the temp file";
        return false;
    }
    outFile << data.dump(4);
    outFile.close();
    inFile.close();
    
    cout << "Date data has been assembled";
    return true;
  }
};