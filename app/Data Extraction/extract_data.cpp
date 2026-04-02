#include "../headers/libs.h"
#include "../utils/api.hpp"
#include <cstdlib>




class YTAPI : public API{
  string APIKEY;
  string baseurl = "https://www.googleapis.com/youtube/v3/";
  unordered_set<string> vids;
  void loadIDs()
  {
    json vids_json;
    ifstream inFile("data/vids.json");
    if (!inFile.is_open())
    {
      cout << "could not open the file" << "\n";
      return;
    }
    try
    {
      inFile >> vids_json;
      for (auto id : vids_json["ids"])
        vids.insert(id.get<string>());
      inFile.close();
    }
    catch (json::parse_error &e)
    {
      std::cerr << "Parse error: " << e.what() << "\n";
      inFile.close();
      return;
    }
  }
  bool allSet()
  {
    return APIKEY.size() ? true : false;
  }
  bool saveVideoIds(){
    json updated_vids_json = {{"ids", json::array()},{"total",0}};
    for (const auto &id : vids) updated_vids_json["ids"].push_back(id);
    updated_vids_json["total"] = vids.size(); 
    std::ofstream outFile("data/vids.json");
    if (!outFile.is_open()){
      std::cerr << "Cannot open file for writing!\n";
      outFile.close();
      return false;
    }
    outFile << updated_vids_json.dump(4);
    outFile.close();

    return true;
  }
  string safeToString(json &j) {
    if (j.is_null()) return "0";
    if (j.is_string()) return j.get<string>();
    return to_string(j.get<long long>());
  }
  bool fetchChannelDetails(json &response){
    cout << "Fetching Channel Details" << "\n";
    string channelIds="";
    for (auto x : response["payload"]["items"]){
        channelIds.append(x["snippet"]["channelId"].get<string>()+",");
    }
    int n = channelIds.size();
    if(!n) return false;
    //replacing last comma with the & to follow the url format
    if(n) channelIds[n-1] = '&';
    string url = baseurl+"channels?part=statistics&id="+channelIds+"key="+APIKEY;

    json responseChannels = get(url);
    if(responseChannels["payload"].contains("error")) {
      cout << responseChannels["payload"]["error"]["message"] << "\n";
      return false;
    }
    if(!responseChannels["status"]) return false; 
    n = response["payload"]["items"].size();
    for(int i=0;i<=n;i++){
      if(responseChannels["payload"]["items"][i].contains("statistics")){
          response["payload"]["items"][i]["subscriberCount"] = responseChannels["payload"]["items"][i]["statistics"]["subscriberCount"];
        }
    }
    cout << "Channel Details attached" << "\n";
    return true;
  }

public:
  YTAPI()
  {
    filesystem::current_path("F:/C&CPP Project/Yeti/app/Data Extraction");
    const char *apiKeyEnv = std::getenv("YT_API_KEY");
    if (!apiKeyEnv)
      std::cerr << "Error: YT_API_KEY not set in environment!\n";
    else
    {
      APIKEY = string(apiKeyEnv);
      loadIDs();
    }
  }
  
  bool FetchVideoIds(string q, string past7, string past14, string part_ = "snippet", string order = "viewCount")
  {
    if (!allSet()){
      cerr << "Missing YT init setup" << "\n";
      return false;
    }
    // 2026-03-19 , Y-M-D
    string endpoint = baseurl + "search" + "?" + "part=" + part_ + "&q=" + q +
                      "&order=" + order + "&relevanceLanguage=en" + "&publishedBefore=" + past7 +
                      "T00:00:00Z" + "&publishedAfter=" + past14 +
                      "T00:00:00Z" + "&maxResults=50&key=" + APIKEY;
    json j = get(endpoint);
    if(j["payload"].contains("error")) {
      cout << j["payload"]["error"]["message"] << "\n";
      return false;
    }
    if (j["status"] && j["payload"].contains("items"))
    {
      for (auto each : j["payload"]["items"])
      {
        if (each.contains("id") && each["id"].contains("videoId"))
        {
          vids.insert(each["id"]["videoId"].get<string>());
        }
      }
      // to save to the file
      json updated_vids_json = {{"ids", json::array()},{"total",0}};
      for (auto id : vids) updated_vids_json["ids"].push_back(id);

      updated_vids_json["total"] = vids.size(); 
      std::ofstream outFile("data/vids.json");
      if (!outFile.is_open())
      {
        cout << updated_vids_json.dump(4) << "\n";
        std::cerr << "Cannot open file for writing!\n";
        outFile.close();
        return false;
      }
      outFile << updated_vids_json.dump(4);
      outFile.close();

      return true;
    }
    else
      cout << "failed to fetch any";
    return false;
  }
  bool FetchBulkVideoDetails(){
    int i =0;
    while(vids.size()) {
      cout << "Fetching " << i++ << "th" << " Batch." << "\n";
      if(!FetchVideoDetails()) return false;
    }
    return true;
  }

  bool FetchVideoDetails(string part_ = "snippet,statistics,contentDetails"){
    if (!allSet()){
      cerr << "Missing YT init setup" << "\n";
      return false;
    }
    unordered_set copy(vids);

    int i = 0;
    string vididlist = "";
    if (!vids.size()){
      cout << "There is no vids to fetch details";
      return false;
    }
    for (auto &x : vids)
    {
      if (i >= 50)
        break;
      if (i != 0)
        vididlist.append(",");
      vididlist.append(x);
      vids.erase(x);
      i++;
    }
    string url = "https://www.googleapis.com/youtube/v3/videos/?part=" + part_ + "&id=" + vididlist + "&key=" + APIKEY;

    json response = get(url);
    if(!response["status"]) {
      cout << "Failed to fetch the video's details" << "\n";
      return false;
    }
    if(response["payload"].contains("error")) {
      cout << response["payload"]["error"]["message"] << "\n";
      return false;
    }
    json data ;
    if(!fetchChannelDetails(response)) return false;

      ifstream inFile("data/glacier1.json");

      if (inFile.is_open()){
        if (inFile.peek() == ifstream::traits_type::eof()){
          data = {
        {"data", json::array()},
        {"count", 0}
      };
        }
        else inFile >> data;
        inFile.close();
      }
      else{
         data = {
        {"data", json::array()},
        {"count", 0}
      };
      }
      if (!data["data"].is_array()) data["data"] = json::array();
      if (!data["count"].is_number()) data["count"] = 0;

      for (auto x : response["payload"]["items"]){
        json v = {
            {"id", safeToString(x["id"])},
            {"publishedAt", safeToString(x["snippet"]["publishedAt"])},
            {"channelId", safeToString(x["snippet"]["channelId"])},
            {"title", safeToString(x["snippet"]["title"])},
            {"viewCount", safeToString(x["statistics"]["viewCount"])},
            {"likeCount", safeToString(x["statistics"]["likeCount"])},
            {"subscriberCount", x["subscriberCount"]},
            {"commentCount", safeToString(x["statistics"]["commentCount"])}
          };

        data["data"].push_back(v);
      }
      int num = (int)data["count"] + (int)response["payload"]["items"].size();
      data["count"] = num;
      if (!saveVideoIds()){
        cout << "\033[31mfailed to delete the trash video ids\033[0m\n";
        vids = copy;
        return false;
      };
    std::ofstream outFile("data/glacier1.json");
    outFile << data.dump(4);
    outFile.close();

    

    cout << "\033[32mVideos's data is fetched\033[0m\n";
    return true; 
  }

  

};
