
#ifndef PROJECT_ROOT
#define PROJECT_ROOT "."
#endif

#ifndef MY_HEADER_H   
#define MY_HEADER_H



#include <curl/curl.h>
#include <bits/stdc++.h>
#include <iomanip>
using namespace std;
#include "../../json.hpp"
using json = nlohmann::json;


#include <iostream>
#include <filesystem>
#include <string>
using namespace std;

namespace fs = std::filesystem;



struct PublishedAtDetails {
    int day_of_week;
    int hour;
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(PublishedAtDetails,  day_of_week, hour)

struct IDataType {
    std::string channelId;
    double commentCount;
    std::string id;
    double likeCount;
    std::string publishedAt;
    
    PublishedAtDetails publishedAtDetails; 
    
    double subscriberCount;
    std::string title;
    double viewCount;
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(IDataType, 
    channelId, commentCount, id, likeCount, publishedAt, 
    publishedAtDetails, subscriberCount, title, viewCount
)


struct InumericalDataInsight{
          double max_lc ,min_lc;double mean_lc,std_lc;
          double max_vc ,min_vc;double mean_vc,std_vc;
          double max_sc ,min_sc;double mean_sc,std_sc;
          double max_cc ,min_cc;double mean_cc,std_cc;
};


tm to_timestamp(const string& s,bool today=false) {
      std::tm tm = {};
      if(today) {
        time_t t = time(nullptr);
        tm = *gmtime(&t);
      }
      else{
        std::istringstream ss(s);
        ss >> std::get_time(&tm, "%Y-%m-%dT%H:%M:%SZ");
      }
      return tm;
}
  

void setFsPointer(string path,bool print=true) {
try {
        string final_path = PROJECT_ROOT+path;
        fs::path targetPath(final_path);

        if (fs::exists(targetPath)) {
            fs::current_path(targetPath);
            if(print) std::cout << "Working directory set to: " << fs::current_path() << std::endl;
        } else {
            std::cerr << "Path does not exist: " << targetPath << std::endl;
        }
    } catch (const fs::filesystem_error& e) {
        std::cerr << "Error changing directory: " << e.what() << std::endl;
    }
}



struct IFetchVideoDataResponse
  {
    bool status;
    string message;
    IDataType payload;
  };



#endif

