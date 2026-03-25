#include "../headers/libs.h"
#include "../utils/api.hpp"
#include <cstdlib>

// vids.json
// {
//     ids : []
// }

class YTAPI : public API {
  string APIKEY;
  string part = "snippets";
  string baseurl = "https://www.googleapis.com/youtube/v3/";
  unordered_set<string> vids;
  void loadIDs() {
    json vids_json;
    ifstream inFile("data/vids.json");
    if (!inFile.is_open()) {
        cout << "could not open the file" << "\n";
        return;
    }
    try {
      inFile >> vids_json;
      for (auto id : vids_json["ids"]) vids.insert(id.get<string>());
      inFile.close(); 
    } catch (json::parse_error &e) {
      std::cerr << "Parse error: " << e.what() << "\n";
      inFile.close(); 
      return;
    }
  }

public:
  YTAPI() {
    filesystem::current_path("F:/C&CPP Project/Yeti/app/Data Extraction");
    const char *apiKeyEnv = std::getenv("YT_API_KEY");
    if (!apiKeyEnv)
      std::cerr << "Error: YT_API_KEY not set in environment!\n";
    else {
      APIKEY = string(apiKeyEnv);
      loadIDs();
    }
  }
  bool searchVideoIds(string q, string past7, string past14,
                      string part_ = "snippet", string order = "viewCount") {
    // 2026-03-19 , Y-M-D
    string endpoint = baseurl + "search" + "?" + "part=" + part_ + "&q=" + q +
                      "&order=" + order+"&relevanceLanguage=en" + "&publishedBefore=" + past7 +
                      "T00:00:00Z" + "&publishedAfter=" + past14 +
                      "T00:00:00Z" + "&maxResults=50&key=" + APIKEY;
    json j =  get(endpoint,false);
    if (j["status"]) {
      if (j["payload"].contains("items")) {
        for (auto each : j["payload"]["items"]) {
            if(each.contains("id") && each["id"].contains("videoId")) {
                vids.insert(each["id"]["videoId"].get<string>());
            }
        }
        // to save to the file
        json updated_vids_json = {{"ids",json::array()}};
        for(auto id:vids) updated_vids_json["ids"].push_back(id);
        std::ofstream outFile("data/vids.json");
        if (!outFile.is_open()) {
              cout << updated_vids_json.dump(4) <<  "\n" ;
              std::cerr << "Cannot open file for writing!\n";
              outFile.close();
              return false;
        }
        outFile << updated_vids_json.dump(4);
        
        outFile.close();
        return true;
      }
    } else
      cout << "failed to fetch any";
      return false;
  }
};
