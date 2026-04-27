#include "../headers/libs.hpp"
#include "../utils/api.hpp"
#include <cstdlib>

class YTAPI : public API
{
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
  
  bool saveVideoIds(){

    json updated_vids_json = {{"ids", json::array()}, {"total", 0}};
    for (const auto &id : vids)
      updated_vids_json["ids"].push_back(id);
    updated_vids_json["total"] = vids.size();
    std::ofstream outFile("vids.json");
    if (!outFile.is_open()){
      std::cerr << RED << "Error : Cannot open file for writing!\n" <<RESET;
      outFile.close();
      return false;
    }
    outFile << updated_vids_json.dump(4);
    outFile.close();

    return true;
  }

  string safeToString(json &j)
  {
    if (j.is_null())
      return "0";
    if (j.is_string())
      return j.get<string>();
    return to_string(j.get<long long>());
  }
  double safeToDouble(json &j)
  {
    if (j.is_null())
      return 0.0;
    if (j.is_string())
      return  stoll(j.get<string>());
    return j.get<double>();
  }

  bool fetchChannelDetails(json &response){

    string channelIds = "";
    for (auto x : response["payload"]["items"]){
      channelIds.append(x["snippet"]["channelId"].get<string>() + ",");
    }
    int n = channelIds.size();
    if (!n)
      return false;
    // replacing last comma with the & to follow the url format
    if (n)
      channelIds[n - 1] = '&';
    string url = baseurl + "channels?part=statistics&id=" + channelIds + "key=" + APIKEY;

    json responseChannels = get(url);
    if (responseChannels["payload"].contains("error"))
    {
      cout << responseChannels["payload"]["error"]["message"] << "\n";
      return false;
    }
    if (!responseChannels["status"])
      return false;
    n = response["payload"]["items"].size();
    for (int i = 0; i <= n; i++)
    {
      if (responseChannels["payload"]["items"][i].contains("statistics")){
        response["payload"]["items"][i]["subscriberCount"] = responseChannels["payload"]["items"][i]["statistics"]["subscriberCount"];
        int views = stoll(responseChannels["payload"]["items"][i]["statistics"]["viewCount"].get<string>());
        int videos = stoll(responseChannels["payload"]["items"][i]["statistics"]["videoCount"].get<string>());

        response["payload"]["items"][i]["averageViewsPerVideo"] = (videos > 0) ? (double)views / videos : 0.0f;
      }
    }
    cout << GREEN << "Channel's Details has attached" << RESET << "\n";
    return true;
  }
  string decodeIdFromURL(string url)
  {

    bool found = false;
    string id;

    for (int i = 0; i < url.size(); i++)
    {
      if (found)
      {
        if (url[i] == '&')
          break;
        id.push_back(url[i]);
        continue;
      }
      if (url[i] == 'v' && url[i + 1] == '=')
      {
        found = true;
        i++;
      }
    }
    return id;
  }

public:
  YTAPI()
  {
    setFsPointer("/app/Data Extraction",false);
    const char *apiKeyEnv = std::getenv("YT_API_KEY");

    if (!apiKeyEnv) {
      std::cerr << RED << "Error: YT_API_KEY not set in environment!\n" << RESET;
      abort();
    }
    else {
      APIKEY = string(apiKeyEnv);
      loadIDs();
    }
  }

  IFetchVideoDataResponse FetchVideoData(string url){
    IFetchVideoDataResponse res{false, "", IDataType()};
    string id = decodeIdFromURL(url);
    if (!id.size()) {
      cerr << RED << "ERROR : Invalid youtube url" << RESET  << "\n";
      return res;
    }
    url = "https://www.googleapis.com/youtube/v3/videos/?part=snippet,statistics,contentDetails&id=" + id + "&key=" + APIKEY;

    json response = get(url);
    if (!response["status"]){
      res.message= "Failed to fetch the video's details";
      return res;
    }
    if (response["payload"].contains("error")){
      res.message= response["payload"]["error"]["message"];
      return res;
    }
    if (response["payload"]["items"][0]["snippet"]["publishedAt"].is_null()){
        res.message = "Video data is inaccesable";
        return res;
    }

    tm t = to_timestamp(response["payload"]["items"][0]["snippet"]["publishedAt"].get<string>());
    tm today = to_timestamp("", true);
    mktime(&t);
    mktime(&today);
    int diff = abs(t.tm_yday - today.tm_yday);

    if (diff < 3 || diff > 6) {
      res.message = "Video must be 3-6 days old.";
      return res;
    }

    if (!fetchChannelDetails(response)) return res;

    res.payload.subscriberCount = safeToDouble(response["payload"]["items"][0]["subscriberCount"]);
    res.payload.commentCount = safeToDouble(response["payload"]["items"][0]["statistics"]["commentCount"]);
    res.payload.currentViewCount = safeToDouble(response["payload"]["items"][0]["statistics"]["viewCount"]);
    res.payload.expectedViewCount = 0.0;
    res.payload.averageViewsPerVideo = safeToDouble(response["payload"]["items"][0]["averageViewsPerVideo"]);
    res.payload.likeCount = safeToDouble(response["payload"]["items"][0]["statistics"]["likeCount"]);

    

    

    res.payload.publishedAtDetails = {};
    res.payload.publishedAtDetails.hour_sin = t.tm_hour;
    res.payload.publishedAtDetails.day_of_week_sin = t.tm_wday;
    res.status = true;

    cout  << GREEN << "Video data has fetched successully" << RESET << "\n";
    return res;
  }

  bool FetchVideoIds(string q, string part_ = "snippet", string order = "viewCount"){
    setFsPointer("/app/Data Extraction/data",false);
    
    string past3 = [&]
    { auto t = chrono::system_clock::to_time_t(chrono::system_clock::now() - chrono::hours(24*3)); ostringstream s; s << put_time(localtime(&t), "%Y-%m-%d"); return s.str(); }();

    string past5 = [&]
    { auto t = chrono::system_clock::to_time_t(chrono::system_clock::now() - chrono::hours(24*3)); ostringstream s; s << put_time(localtime(&t), "%Y-%m-%d"); return s.str(); }();

    
    string endpoint = baseurl + "search" + "?" + "part=" + part_ + "&q=" + q + "&order=" + order + "&relevanceLanguage=en" + "&publishedBefore=" + past3 +
                      "T00:00:00Z" + "&publishedAfter=" + past5 +
                      "T00:00:00Z" + "&maxResults=50&key=" + APIKEY;
    json j = get(endpoint);
    if (j["payload"].contains("error")) {cout <<RED << "Error : " <<  j["payload"]["error"]["message"] << RESET << "\n"; return false;}
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
      json updated_vids_json = {{"ids", json::array()}, {"total", 0}};
      for (auto id : vids)
        updated_vids_json["ids"].push_back(id);

      updated_vids_json["total"] = vids.size();
      std::ofstream outFile("vids.json");
      if (!outFile.is_open()){
        std::cerr << RED << "Error : Cannot open file for writing!\n" << RESET;
        outFile.close();
        return false;
      }
      outFile << updated_vids_json.dump(4);
      outFile.close();
      cout << YELLOW << "Videos search has sucessfully completed" << RESET << "\n";
      return true;
    }
    else cout << RED <<  "failed to fetch any" << RESET << "\n";
    return false;
  }

  bool FetchBulkVideoDetails(){
    int i = 0;
    if(!vids.size())  cout << YELLOW << "There is no videos's ids to fetch details" << RESET << "\n";
    while (vids.size()) {
      cout << "Fetching " << i++ << "th" << " Batch." << "\n";
      if (!FetchVideoDetailsParams())return false;
    }
    
    return true;
  }

  bool FetchVideoDetailsParams(string part_ = "snippet,statistics,contentDetails"){
    setFsPointer("/app/Data Extraction/data",false);

    


    unordered_set copy(vids);

    int i = 0;
    string vididlist ;
    if (!vids.size()){
      cout << YELLOW << "There is no videos's ids to fetch details" << RESET << "\n";
      return false;
    }

    for (auto &x : copy) {
      if (i >= 50) break;
      vididlist.append(x);
      vididlist.append(",");
      vids.erase(x);
      i++;
    }
    vididlist.pop_back();


    string url = "https://www.googleapis.com/youtube/v3/videos/?part=" + part_ + "&id=" + vididlist + "&key=" + APIKEY;

    json response = get(url);

    if (!response["status"]){
      cout << "Failed to fetch the video's details" << "\n";
      return false;
    }
    if (response["payload"].contains("error")){
       cout <<RED << "Error : " <<  response["payload"]["error"]["message"] << RESET << "\n"; return false;
    }
    json data;

    if (!fetchChannelDetails(response)) return false;

    ifstream inFile("glacier3.json");

    if (inFile.is_open())
    {
      if (inFile.peek() == ifstream::traits_type::eof())
      {
        data = {
            {"data", json::array()},
            {"count", 0}};
      }
      else
        inFile >> data;
      inFile.close();
    }
    

    if (!data["data"].is_array()) data["data"] = json::array();
    if (!data["count"].is_number()) data["count"] = 0;

    for (auto x : response["payload"]["items"]){
      json v = {
          {"id", safeToString(x["id"])},
          {"publishedAt", safeToString(x["snippet"]["publishedAt"])},
          {"channelId", safeToString(x["snippet"]["channelId"])},
          {"title", safeToString(x["snippet"]["title"])},
          {"currentViewCount", safeToString(x["statistics"]["viewCount"])},
          {"expectedViewCount", 0.0},
          {"likeCount", safeToString(x["statistics"]["likeCount"])},
          {"subscriberCount", x["subscriberCount"]},
          {"averageViewsPerVideo", x["averageViewsPerVideo"]},
          {"commentCount", safeToString(x["statistics"]["commentCount"])},
          {"TargetCollected", json::boolean_t(false)}
        };

      data["data"].push_back(v);
    }
    int num = (int)data["count"] + (int)response["payload"]["items"].size();
    data["count"] = num;

    if (!saveVideoIds()) {
      cout << "\033[31mfailed to delete the trash video ids\033[0m\n";
      vids = copy;
      return false;
    };
    std::ofstream outFile("glacier3.json");
    outFile << data.dump(4);
    outFile.close();

    cout << "\033[32mVideos's data is fetched\033[0m\n";
    return true;
  }

  bool FetchVideoDetailsTarget(string part_ = "statistics"){
    json data;
    setFsPointer("/app/Data Extraction/data",false);
    ifstream inFile("glacier3.json");
    
    if(!inFile.is_open()){
        cout <<RED  << "Error : Unable to read the data \n" << RESET;
        return false;
    }

    inFile >> data;
    
    string ids;
    int i = 1;
    map<string,int> indices;
    
    try{
      auto processBatch = [&]() {
        if(ids.empty()) return;
        ids.pop_back(); // remove last ','

    string url = "https://www.googleapis.com/youtube/v3/videos?part="  + part_ + "&id=" + ids + "&key=" + APIKEY;

    json response = get(url);

    if (!response["status"].get<bool>() ) {
        cout <<RED  << "Error : Failed to fetch the video's Targets\n" << RESET;
        throw "Failed to fetch the video's details";
    }

    if(response["payload"].contains("error")){
      cout <<RED  << "Error : " << response["payload"]["error"]["message"] << RESET;
      throw response["payload"]["error"]["message"];
    }

    for (auto &res : response["payload"]["items"]) {
        data["data"][indices[res["id"].get<string>()]]["expectedViewCount"] = res["statistics"]["viewCount"];
        data["data"][indices[res["id"].get<string>()]]["TargetCollected"] = json::boolean_t(true);
    }
    

    cout << GREEN <<  "Data has been successfully fetched!" << RESET  << "\n";

    i = 0;
    ids.clear();
    indices.clear();
};
      int j=0;
      for (auto x : data["data"]){
        if (!x["TargetCollected"].get<bool>()){
          if (i >= 50) processBatch();

          ids.append(x["id"].get<string>());
          ids.append(",");
          indices.insert({x["id"].get<string>(),j});
          i++;
        }
        j++;
      }
    }
    catch (const char *message){
      ofstream outFile("glacier3.json");

      if(!outFile.is_open()){
        cout <<RED  << "Error : Unable to create the temporary data file \n" << RESET;
        return false;
      }

      outFile << data.dump(4);
      inFile.close();
      outFile.close();
      cout << RED << "Error : " <<  message << RESET << "\n";
      return false;
    }
    catch (const nlohmann::json::exception &e) {
    cout << RED <<  "JSON Error: " << e.what() << RESET << "\n";
    return false;
    }
    
    ofstream outFile("glacier3.json");
    if(!outFile.is_open()){
        cout <<RED  << "Error : Unable to create the temporary data file \n" << RESET;
        return false;
    }
    outFile << data.dump(4);
    inFile.close();
    outFile.close();
    return true;
  }




};

