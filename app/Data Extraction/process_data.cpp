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
  struct InumericalDataInsight{
          double max_lc ,min_lc;double mean_lc,std_lc;
          double max_vc ,min_vc;double mean_vc,std_vc;
          double max_sc ,min_sc;double mean_sc,std_sc;
          double max_cc ,min_cc;double mean_cc,std_cc;
  };
  InumericalDataInsight GetNumericalDataInsights(bool print=true){
          ifstream dataFile("data/glacier1std.json");
          json data;
          dataFile >> data;
          double max_lc = 0, min_lc=INT_MAX;
          double max_vc = 0, min_vc=INT_MAX;
          double max_sc = 0, min_sc=INT_MAX;
          double max_cc = 0, min_cc=INT_MAX;
          long long slc = 0 , svc = 0 , scc=0 ,ssc=0;

          for(auto x:data["data"]){
            if(max_lc < x["likeCount"].get<int>())  max_lc = x["likeCount"];
            if(min_lc > x["likeCount"].get<int>()) min_lc = x["likeCount"];
            slc+= x["likeCount"].get<int>();

            if(max_sc < x["subscriberCount"].get<int>())  max_sc = x["subscriberCount"];
            if(min_sc > x["subscriberCount"].get<int>()) min_sc = x["subscriberCount"];
            ssc += x["subscriberCount"].get<int>();

            if(max_cc < x["commentCount"].get<int>())  max_cc = x["commentCount"];
            if(min_cc > x["commentCount"].get<int>()) min_cc = x["commentCount"];
            scc += x["commentCount"].get<int>();

            if(max_vc < x["viewCount"].get<int>())  max_vc = x["viewCount"];
            if(min_vc > x["viewCount"].get<int>()) min_vc = x["viewCount"];
            svc += x["viewCount"].get<int>();
          }
          int n = data["count"].get<int>();
          double mean_lc = slc/n , mean_sc = ssc/n , mean_vc = svc/n , mean_cc = scc/n;
          double std_lc = 0.0,std_sc=0,std_vc =0,std_cc=0;

          for(auto x:data["data"]){
                double s1= mean_lc-x["likeCount"].get<int>();
                std_lc += s1*s1;

                s1= mean_cc-x["commentCount"].get<int>();
                std_cc += s1*s1;

                s1= mean_vc-x["viewCount"].get<int>();
                std_vc += s1*s1;

                s1= mean_sc-x["subscriberCount"].get<int>();
                std_sc += s1*s1;
          }

          std_lc = sqrt(std_lc/n) ;std_sc=sqrt(std_sc/n);std_vc =sqrt(std_vc/n);std_cc=sqrt(std_cc/n);

          if(print){
            cout << "Like Count \nmax : " << max_lc << " min : " << min_lc  << " mean : " << mean_lc << " STD : " <<std_lc << "\n";
            cout << "Subscriber Count \nmax : " << max_sc << " min : " << min_sc << " mean : " << mean_sc << " STD : " <<std_sc<< "\n";
            cout << "Comment Count \nmax : " << max_cc << " min : " << min_cc << " mean : " << mean_cc << " STD : " <<std_cc << "\n";
            cout << "View Count \nmax : " << max_vc << " min : " << min_vc << " mean : " << mean_vc << " STD : " <<std_vc << "\n";
          }
          struct InumericalDataInsight nm({max_lc ,min_lc,mean_lc,std_lc ,max_vc ,min_vc,mean_vc,std_vc,max_sc ,min_sc,mean_sc,std_sc,max_cc ,min_cc,mean_cc,std_cc});
          dataFile.close();
          return nm;
          
  }
  bool ScaleMinMaxAll(){
    struct InumericalDataInsight insights = GetNumericalDataInsights(false);
    ifstream inFile("data/glacier1.json");
    if(!inFile.is_open()){
      cout << "Can't able to read the data file" << "\n";
      return false;

    }
    
    ofstream outFile("data/glacier1mm.json");
    if(!outFile.is_open()){
      cout << "Can't able to create the new data file" << "\n";
      return false;

    }
    json data;
    inFile >> data;

    for(auto &x:data["data"]){
        x["subscriberCount"] = (float)(x["subscriberCount"].get<int>()-insights.min_sc)/(insights.max_sc-insights.min_sc);
        x["viewCount"] = (float)(x["viewCount"].get<int>()-insights.min_vc)/(insights.max_vc-insights.min_vc);
        x["commentCount"] = (float)(x["commentCount"].get<int>()-insights.min_cc)/(insights.max_cc-insights.min_cc);
        x["likeCount"] = (float)(x["likeCount"].get<int>()-insights.min_lc)/(insights.max_lc-insights.min_lc);
    }
    outFile << data.dump(4);
    outFile.close();
    inFile.close();
    cout << "Min Max scalling is done" << "\n";
    return true;

  }
  bool StandardizarionScaling(){
struct InumericalDataInsight insights = GetNumericalDataInsights(false);
    ifstream inFile("data/glacier1.json");
    if(!inFile.is_open()){
      cout << "Can't able to read the data file" << "\n";
      return false;

    }
    
    ofstream outFile("data/glacier1std.json");
    if(!outFile.is_open()){
      cout << "Can't able to create the new data file" << "\n";
      return false;

    }
    json data;

    inFile >> data;
        for(auto &x:data["data"]){
        x["subscriberCount"] = (float)(x["subscriberCount"].get<int>()-insights.mean_sc)/(insights.std_sc);
        x["viewCount"] = (float)(x["viewCount"].get<int>()-insights.mean_vc)/(insights.std_vc);
        x["commentCount"] = (float)(x["commentCount"].get<int>()-insights.mean_cc)/(insights.std_cc);
        x["likeCount"] = (float)(x["likeCount"].get<int>()-insights.mean_lc)/(insights.std_lc);
    }

    outFile << data.dump(4);
    outFile.close();
    inFile.close();
    cout << "Standardization scalling is done" << "\n";
    return true;
    
  }

};