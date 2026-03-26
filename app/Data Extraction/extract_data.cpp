#include "../headers/libs.h"
#include "../utils/api.hpp"
#include <cstdlib>

// vids.json
// {
//     ids : []
// }

struct vidDetails
{
  string id, publishedAt, channelId, title;
  long long viewCount;
  long long likeCount;
  long long subscriberCount;
  long commentCount;
};

class YTAPI : public API
{
  string APIKEY;
  string part = "snippets";
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
  bool saveVideoIds()
  {

    json updated_vids_json = {{"ids", json::array()}};
    for (auto id : vids)
      updated_vids_json["ids"].push_back(id);
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
  string safeToString(json &j) {
    if (j.is_null()) return "0";
    if (j.is_string()) return j.get<string>();
    return to_string(j.get<long long>());
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
  bool searchVideoIds(string q, string past7, string past14, string part_ = "snippet", string order = "viewCount")
  {
    if (!allSet())
    {
      cerr << "Missing YT init setup" << "\n";
      return false;
    }
    // 2026-03-19 , Y-M-D
    string endpoint = baseurl + "search" + "?" + "part=" + part_ + "&q=" + q +
                      "&order=" + order + "&relevanceLanguage=en" + "&publishedBefore=" + past7 +
                      "T00:00:00Z" + "&publishedAfter=" + past14 +
                      "T00:00:00Z" + "&maxResults=50&key=" + APIKEY;
    json j = get(endpoint, false);
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
      json updated_vids_json = {{"ids", json::array()}};
      for (auto id : vids)
        updated_vids_json["ids"].push_back(id);
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

  bool searchVideoDetails(string part_ = "snippet,statistics,contentDetails")
  {
    unordered_set copy(vids);
    int i = 0;
    string vididlist = "";
    if (!vids.size())
    {
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
    json data = {{"data", json::array()}};

    if (response["status"] && response["payload"].contains("items"))
    {

      ifstream inFile("data/glacier1.json");
      if (inFile.is_open())
      {
        if (inFile.peek() == ifstream::traits_type::eof())
        {
          data["videos"] = json::array(); // initialize
        }
        else
        {
          inFile >> data;
        }
        inFile.close();
      }
      else
        data["data"] = json::array();

      for (auto x : response["payload"]["items"])
      {
        json v = {
            {"id", x["id"]},
            {"publishedAt", x["snippet"]["publishedAt"]},
            {"channelId", x["snippet"]["channelId"]},
            {"title", x["snippet"]["title"]},

            {"viewCount", safeToString(x["statistics"]["viewCount"])},
            {"likeCount", safeToString(x["statistics"]["likeCount"])},
            {"subscriberCount", 0},
            {"commentCount", safeToString(x["statistics"]["commentCount"])}};

        data["data"].push_back(v);
      }
    }
    else
    {
      cout << "Failed to fetch the video's details" << "\n";
      return false;
    }

    std::ofstream outFile("data/glacier1.json");
    outFile << data.dump(4);
    outFile.close();

    if (!saveVideoIds())
    {
      cout << "failed to delete the trash video ids" << "\n";
    };

    return true;
  }
};
