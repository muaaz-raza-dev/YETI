#include "../headers/libs.hpp"






class ProcessData {
    private :

    

    public :
    ProcessData(){ //Constructor
      setFsPointer("/app/Data Extraction");
    
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
        x["publishedAtDetails"]["hour"] = t.tm_hour;
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


  
 
  InumericalDataInsight GetNumericalDataInsights(bool print=true){
        setFsPointer("/app/Data Extraction",false);
        struct InumericalDataInsight m;
          ifstream dataFile("data/glacier1std.json");
          if(!dataFile.is_open()){
            cout << "Unable to read the data to get numerical Insights" << "\n";
            return m;
          }
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
  IDataType& ScaleMinMax(IDataType &dp){
        struct InumericalDataInsight insights = GetNumericalDataInsights(false);
        insights.min_sc = min(insights.min_sc,dp.subscriberCount);
        insights.max_sc = max(insights.max_sc,dp.subscriberCount);

        insights.min_cc = min(insights.min_cc,dp.commentCount);
        insights.max_cc = max(insights.max_cc,dp.commentCount);

        insights.min_vc = min(insights.min_vc,dp.viewCount);
        insights.max_vc = max(insights.max_vc,dp.viewCount);

        insights.min_lc = min(insights.min_lc,dp.likeCount);
        insights.max_lc = max(insights.max_lc,dp.likeCount);

        dp.subscriberCount = (double)(dp.subscriberCount-insights.min_sc)/(insights.max_sc-insights.min_sc);
        dp.commentCount = (double)(dp.commentCount-insights.min_cc)/(insights.max_cc-insights.min_cc);
        dp.likeCount = (double)(dp.likeCount-insights.min_lc)/(insights.max_lc-insights.min_lc);
        dp.viewCount = (double)(dp.viewCount-insights.min_vc)/(insights.max_vc-insights.min_vc);
        dp.publishedAtDetails.day_of_week = (double)(dp.publishedAtDetails.day_of_week)/(6);
        dp.publishedAtDetails.hour = (double)(dp.publishedAtDetails.hour)/(23);
        cout << "Local Min Max Scaling is done ";
        return dp;
  }

   bool ScaleMinMaxAll(){
    struct InumericalDataInsight insights = GetNumericalDataInsights(false);

      ifstream inFile("data/glacier1.json");
      if(!inFile.is_open()){
      cerr << "Can't able to read the data file" << "\n";
      return false;

    }
    
    ofstream outFile("data/glacier1mm.json");
    if(!outFile.is_open()){
      cerr << "Can't able to create the new data file" << "\n";
      return false;
      
    }
    json data;
    inFile >> data;
    
    for(auto &x:data["data"]){
      x["subscriberCount"] = (double)(x["subscriberCount"].get<int>()-insights.min_sc)/(insights.max_sc-insights.min_sc);
        x["commentCount"] = (double)(x["commentCount"].get<int>()-insights.min_cc)/(insights.max_cc-insights.min_cc);
        x["likeCount"] = (double)(x["likeCount"].get<int>()-insights.min_lc)/(insights.max_lc-insights.min_lc);
        x["viewCount"] = (double)(x["viewCount"].get<int>()-insights.min_vc)/(insights.max_vc-insights.min_vc);
        x["publishedAtDetails"]["day_of_week"] = (double)(x["publishedAtDetails"]["day_of_week"].get<int>())/(6);
        x["publishedAtDetails"]["hour"] = (double)(x["publishedAtDetails"]["hour"].get<int>())/(23);
      }
      outFile << data.dump(4);
      outFile.close();
      inFile.close();
    
    
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
        x["commentCount"] = (float)(x["commentCount"].get<int>()-insights.mean_cc)/(insights.std_cc);
        x["likeCount"] = (float)(x["likeCount"].get<int>()-insights.mean_lc)/(insights.std_lc);
    }

    outFile << data.dump(4);
    outFile.close();
    inFile.close();
    cout << "Standardization scalling is done" << "\n";
    return true;
  }
  bool SplitData(){
      json data = {{"count",0},{"data",json::array()}};
      ifstream DataFile("data/glacier1mm.json");
      if(!DataFile.is_open()){
        cout << "Can't able to read the data file" << "\n";
        return false;
      }
    DataFile >> data;
    int count = data["count"].get<int>();

    vector<IDataType> ds = data["data"].get<vector<IDataType>>();
    std::random_device rd;
    std::mt19937 g(rd());

    shuffle(ds.begin(),ds.end(),g);

    int limit= count * 2/10;
  
    vector<IDataType> testset(ds.begin(), ds.begin() + limit);
    vector<IDataType> trainset(ds.begin() + limit, ds.end());

    ofstream TrainSetF("data/glacier1trs.json");
    ofstream TestSetF("data/glacier1ts.json");

    if(!TrainSetF.is_open()||!TestSetF.is_open()){
        cout << "Can't able to create the data files" << "\n";
        return false;
    }

    json trsj = {{"count",trainset.size()},{"data",trainset}} ;
    json tsj = {{"count",testset.size()},{"data",testset}} ;


    TrainSetF << trsj.dump(4);
    TestSetF << tsj.dump(4);

    TrainSetF.close();
    TestSetF.close();
    DataFile.close();

    cout << "Split has been created successfully" << "\n";

    return true;


  }

};
