#include "../headers/libs.hpp"
#include <numbers>





class ProcessData {
    private :

    

    public :
    ProcessData(){ 
      setFsPointer("/app/Data Extraction/",false);
    
    }

    bool removeDuplicates(string fileName="glacier1.json"){

      unordered_set<string> ids;
      json raw_data;
      setFsPointer("/app/Data Extraction/data",false);
      ifstream inFile(fileName);
      if(!inFile.is_open()){
        cerr << RED <<  "Error : Can't able to read the data" << RESET << "\n";
        return false;
      }
      inFile >> raw_data;
      json processed_data = {{"data",json::array()},{"count",0}};
      
      for(auto x:raw_data["data"]){
          if(ids.count(x["id"])) continue;
          if(x["subscriberCount"].is_null() ) continue;
          ids.insert(x["id"]);
          json v = {
            {"id", x["id"]},
            {"publishedAt", x["publishedAt"]},
            {"channelId", x["channelId"]},
            {"title", x["title"]},
            {"expectedViewCount", x["expectedViewCount"]},
            {"currentViewCount", x["currentViewCount"]},
            {"likeCount", x["likeCount"]},
            {"subscriberCount", x["subscriberCount"]},
            {"commentCount", x["commentCount"]},
            {"averageViewsPerVideo", x["averageViewsPerVideo"]},
            {"TargetCollected",x["TargetCollected"] }
          };
          processed_data["data"].push_back(v);
      }
      processed_data["count"] = processed_data["data"].size();

      ofstream outFile(fileName);
      if(!inFile.is_open()){
        cerr << RED <<  "Error : Can't able to create the file" << RESET << "\n";
        return false;
      }

      outFile << processed_data.dump(4);
      outFile.close();
      inFile.close();
      cout << GREEN<< "Duplicates has been deleted" <<RESET << "\n";
      return true;
    }
    bool FixType(string fileName="glacier1.json"){
      json data;
      setFsPointer("/app/Data Extraction/data",false);
      ifstream inFile(fileName);
      if(!inFile.is_open()){
        cerr << RED <<  "Error : Can't able to read the data" << RESET << "\n";
        return false;
      }
      inFile >> data;
      for(auto &x:data["data"]){
          x["commentCount"] = x["commentCount"].is_number() ? x["commentCount"].get<double>() : stoll(x["commentCount"].get<string>());
          x["likeCount"] = x["likeCount"].is_number() ? x["likeCount"].get<double>() : stoll(x["likeCount"].get<string>());
          x["subscriberCount"] = x["subscriberCount"].is_number() ? x["subscriberCount"].get<double>() : stoll(x["subscriberCount"].get<string>());
          x["currentViewCount"] = x["currentViewCount"].is_number() ? x["currentViewCount"].get<double>() : stoll(x["currentViewCount"].get<string>());
          x["expectedViewCount"] = x["expectedViewCount"].is_number() ? x["expectedViewCount"].get<double>() : stoll(x["expectedViewCount"].get<string>());
      }
    ofstream outFile(fileName);
    if(!outFile.is_open()){
        cerr << RED <<  "Error : Can't able to create the temporary file" << RESET << "\n";
        return false;
    }
    outFile << data.dump(4);
    outFile.close();
    inFile.close();
    cout <<GREEN << "Expected numerical data types has been fixed " << RESET << "\n";
    return true;
  }
  bool processssDate(string fileName="glacier1.json"){
    
      json data;
      setFsPointer("/app/Data Extraction/data",false);
      ifstream inFile(fileName);
      if(!inFile.is_open()){
        cerr << RED <<  "Error : Can't able to read the data" << RESET << "\n";
        return false;
      }
      inFile >> data;
      for(auto &x:data["data"]){
        tm t =  to_timestamp(x["publishedAt"].get<string>());
        mktime(&t);

        x["publishedAtDetails"] = {};
        x["publishedAtDetails"]["hour_sin"] = t.tm_hour;
        x["publishedAtDetails"]["hour_cos"] = 0.0;
        x["publishedAtDetails"]["day_of_week_sin"] = t.tm_wday;
        x["publishedAtDetails"]["day_of_week_cos"] = 0.0;
      }

    ofstream outFile(fileName);
    if(!outFile.is_open()){
        cerr << RED <<  "Error : Can't able to create the temporary file" << RESET << "\n";
        return false;
    }
    outFile << data.dump(4);
    outFile.close();
    inFile.close();
    
    cout << GREEN << "Date's related data has been assembled" << RESET << "\n";
    return true;
  }

  bool LogScalling(string fileName="glacier1.json",string outFileName="glacier1-s1.json"){

      setFsPointer("/app/Data Extraction/data",false);
      ifstream inFile(fileName);
      if(!inFile.is_open()){
        cerr << RED <<  "Error : Can't able to read the data" << RESET << "\n";
        return false;
      }
      json data;
      inFile >> data;

      for(auto &x:data["data"]){
        x["subscriberCount"] = (double)log(1+x["subscriberCount"].get<double>());
        x["currentViewCount"] = (double)log(1+x["currentViewCount"].get<double>());
        x["expectedViewCount"] = (double)log(1+x["expectedViewCount"].get<double>());
        x["commentCount"] = (double)log(1+x["commentCount"].get<double>());
        x["likeCount"] = (double)log(1+x["likeCount"].get<double>());
        x["averageViewsPerVideo"] = (double)log(1+abs(x["averageViewsPerVideo"].get<double>()));
      }
    
    ofstream outFile(outFileName);
    outFile << data.dump(4);
    inFile.close();
    outFile.close();

    cout << GREEN << "Log scalling has been completed on specific data fields." << RESET << "\n";
    return true;
  }

  bool LogScalling(IDataType &d){
        d.subscriberCount = (double)log(1+d.subscriberCount);
        d.currentViewCount = (double)log(1+d.currentViewCount);
        d.expectedViewCount = (double)log(1+d.expectedViewCount);
        d.commentCount = (double)log(1+d.commentCount);
        d.likeCount = (double)log(1+d.likeCount);
        d.averageViewsPerVideo = (double)log(1+d.averageViewsPerVideo);
        return true;
  }

  bool CyclicEncoding(IDataType &x){
        x.publishedAtDetails.hour_cos = (double)cos((2*(M_PI)*x.publishedAtDetails.hour_sin) / 24.0);
        x.publishedAtDetails.hour_sin = (double)sin((2*(M_PI)*x.publishedAtDetails.hour_sin) / 24.0);

        x.publishedAtDetails.day_of_week_cos = (double)cos((2*(M_PI)*x.publishedAtDetails.day_of_week_sin) / 7.0);
        x.publishedAtDetails.day_of_week_sin = (double)cos((2*(M_PI)*x.publishedAtDetails.day_of_week_sin) / 7.0);

      return true;
  }

  bool CyclicEncoding(string fileName="glacier1.json",string outFileName="glacier1-s1.json"){

      setFsPointer("/app/Data Extraction/data",false);
      ifstream inFile(fileName);
      if(!inFile.is_open()){
        cerr << RED <<  "Error : Can't able to read the data" << RESET << "\n";
        return false;
      }
      json data;
      inFile >> data;

      for(auto &x:data["data"]){
        double h = x["publishedAtDetails"]["hour_sin"].get<double>() , d = x["publishedAtDetails"]["day_of_week_sin"].get<double>();
        
        x["publishedAtDetails"]["hour_cos"] = (double)cos((2*(M_PI)*h) / 24.0);
        x["publishedAtDetails"]["hour_sin"] = (double)sin((2*(M_PI)*h) / 24.0);

        x["publishedAtDetails"]["day_of_week_cos"] = (double)cos((2*(M_PI)*d) / 7.0);
        x["publishedAtDetails"]["day_of_week_sin"] = (double)cos((2*(M_PI)*d) / 7.0);
      }
    
    ofstream outFile(outFileName);
    outFile << data.dump(4);
    inFile.close();
    outFile.close();

    cout << GREEN << "Cyclic Encoding has been compeleted on specific data fields." << RESET << "\n";
    return true;
  }



  
 
  // InumericalDataInsight GetNumericalDataInsights(bool print=true,string fileName="glacier1.json"){
  //       setFsPointer("/app/Data Extraction/data",false);
  //       struct InumericalDataInsight m;
  //         ifstream dataFile(fileName);
  //         if(!dataFile.is_open()){
  //           cerr << RED <<  "Error : Can't able to read the data file" << RESET << "\n";
  //           return m;
  //         }
  //         json data;
  //         dataFile >> data;
  //         double max_lc = 0, min_lc=INT_MAX;
  //         double max_vc = 0, min_vc=INT_MAX;
  //         double max_sc = 0, min_sc=INT_MAX;
  //         double max_cc = 0, min_cc=INT_MAX;
  //         double max_vv = 0, min_vv=INT_MAX;
  //         long long slc = 0 , svc = 0 , scc=0 ,ssc=0 , svv=0;

  //         for(auto x:data["data"]){
  //           if(max_lc < x["likeCount"].get<int>())  max_lc = x["likeCount"];
  //           if(min_lc > x["likeCount"].get<int>()) min_lc = x["likeCount"];
  //           slc+= x["likeCount"].get<int>();

  //           if(max_vv < x["averageViewsPerVideo"].get<double>())  max_vv = x["averageViewsPerVideo"];
  //           if(min_vv > x["averageViewsPerVideo"].get<double>()) min_vv = x["averageViewsPerVideo"];
  //           svv+= x["averageViewsPerVideo"].get<double>();

  //           if(max_sc < x["subscriberCount"].get<int>())  max_sc = x["subscriberCount"];
  //           if(min_sc > x["subscriberCount"].get<int>()) min_sc = x["subscriberCount"];
  //           ssc += x["subscriberCount"].get<int>();

  //           if(max_cc < x["commentCount"].get<int>())  max_cc = x["commentCount"];
  //           if(min_cc > x["commentCount"].get<int>()) min_cc = x["commentCount"];
  //           scc += x["commentCount"].get<int>();

  //           if(max_vc < x["currentViewCount"].get<int>())  max_vc = x["currentViewCount"];
  //           if(min_vc > x["currentViewCount"].get<int>()) min_vc = x["currentViewCount"];
  //           svc += x["currentViewCount"].get<int>();
  //         }
  //         int n = data["count"].get<int>();
  //         double mean_lc = slc/n , mean_sc = ssc/n , mean_vc = svc/n , mean_cc = scc/n , mean_vv = svv/n;
  //         double std_lc = 0.0,std_sc=0,std_vc =0,std_cc=0,std_vv=0;

  //         for(auto x:data["data"]){
  //               double s1= mean_lc-x["likeCount"].get<int>();
  //               std_lc += s1*s1;

  //               s1= mean_cc-x["commentCount"].get<int>();
  //               std_cc += s1*s1;

  //               s1= mean_vc-x["viewCount"].get<int>();
  //               std_vc += s1*s1;

  //               s1= mean_sc-x["subscriberCount"].get<int>();
  //               std_sc += s1*s1;

  //               s1= mean_vv-x["averageViewsPerVideo"].get<double>();
  //               std_vv += s1*s1;
  //         }

  //         std_lc = sqrt(std_lc/n) ;std_sc=sqrt(std_sc/n);std_vc =sqrt(std_vc/n);std_cc=sqrt(std_cc/n), std_vv = sqrt(std_vv/n);

  //         if(print){
  //           cout << "Total :" << data["count"].get<int>() << "\n";
  //           cout << "Like Count \nmax : " << max_lc << " min : " << min_lc  << " mean : " << mean_lc << " STD : " <<std_lc << "\n";
  //           cout << "Subscriber Count \nmax : " << max_sc << " min : " << min_sc << " mean : " << mean_sc << " STD : " <<std_sc<< "\n";
  //           cout << "Comment Count \nmax : " << max_cc << " min : " << min_cc << " mean : " << mean_cc << " STD : " <<std_cc << "\n";
  //           cout << "View Count \nmax : " << max_vc << " min : " << min_vc << " mean : " << mean_vc << " STD : " <<std_vc << "\n";
  //           cout << "Average Views Per Video (Channel) \nmax : " << max_vv << " min : " << min_vv << " mean : " << mean_vv << " STD : " <<std_vv << "\n";
  //         }
  //         struct InumericalDataInsight nm({max_lc ,min_lc,mean_lc,std_lc ,max_vc ,min_vc,mean_vc,std_vc,max_sc ,min_sc,mean_sc,std_sc,max_cc ,min_cc,mean_cc,std_cc,max_vv,min_vv,mean_vv,std_vv});
  //         dataFile.close();
  //         return nm;
          
  // }
  // IDataType& ScaleMinMax(IDataType &dp,string fileName="glacier1.json"){

  //       struct InumericalDataInsight insights = GetNumericalDataInsights(false,fileName);
  //       insights.min_sc = min(insights.min_sc,dp.subscriberCount);
  //       insights.max_sc = max(insights.max_sc,dp.subscriberCount);

  //       insights.min_cc = min(insights.min_cc,dp.commentCount);
  //       insights.max_cc = max(insights.max_cc,dp.commentCount);

  //       insights.min_vc = min(insights.min_vc,dp.viewCount);
  //       insights.max_vc = max(insights.max_vc,dp.viewCount);

  //       insights.min_lc = min(insights.min_lc,dp.likeCount);
  //       insights.max_lc = max(insights.max_lc,dp.likeCount);

  //       insights.min_vv = min(insights.min_vv,dp.averageViewsPerVideo);
  //       insights.max_vv = max(insights.max_vv,dp.averageViewsPerVideo);

  //       dp.subscriberCount = (double)(dp.subscriberCount-insights.min_sc)/(insights.max_sc-insights.min_sc);
  //       dp.commentCount = (double)(dp.commentCount-insights.min_cc)/(insights.max_cc-insights.min_cc);
  //       dp.likeCount = (double)(dp.likeCount-insights.min_lc)/(insights.max_lc-insights.min_lc);
  //       dp.viewCount = (double)(dp.viewCount-insights.min_vc)/(insights.max_vc-insights.min_vc);
  //       dp.averageViewsPerVideo = (double)(dp.averageViewsPerVideo-insights.min_vv)/(insights.max_vv-insights.min_vv);
  //       dp.publishedAtDetails.day_of_week = (double)(dp.publishedAtDetails.day_of_week)/(6.0);
  //       dp.publishedAtDetails.hour = (double)(dp.publishedAtDetails.hour)/(23.0);

  //       cout <<GREEN << "Scalled (Min Max Scaling)" << RESET << "\n";
  //       return dp;
  // }

  //  bool ScaleMinMaxAll(string dataFileName="glacier1.json",string outputFileName="glacier1mm.json"){
  //   struct InumericalDataInsight insights = GetNumericalDataInsights(false,dataFileName);
  //     setFsPointer("/app/Data Extraction/data",false);
  //     ifstream inFile(dataFileName);
  //     if(!inFile.is_open()){
  //     cerr << RED <<  "Error : Can't read the data from the file" << RESET << "\n";
  //     return false;

  //   }
    
  //   ofstream outFile(outputFileName);
  //   if(!outFile.is_open()){
  //     cerr << RED <<  "Error : Can't able to create the output file" << RESET << "\n";
  //     return false;
      
  //   }
  //   json data;
  //   inFile >> data;

  //   for(auto &x:data["data"]){
  //       x["subscriberCount"] = (double)(x["subscriberCount"].get<double>()-insights.min_sc)/(insights.max_sc-insights.min_sc);
  //       x["commentCount"] = (double)(x["commentCount"].get<double>()-insights.min_cc)/(insights.max_cc-insights.min_cc);
  //       x["likeCount"] = (double)(x["likeCount"].get<double>()-insights.min_lc)/(insights.max_lc-insights.min_lc);
  //       x["viewCount"] = (double)(x["viewCount"].get<double>()-insights.min_vc)/(insights.max_vc-insights.min_vc);
  //       x["averageViewsPerVideo"] = (double)(x["averageViewsPerVideo"].get<double>()-insights.min_vv)/(insights.max_vv-insights.min_vv);
  //       x["publishedAtDetails"]["day_of_week"] = (double)(x["publishedAtDetails"]["day_of_week"].get<double>())/(6.0);
  //       x["publishedAtDetails"]["hour"] = (double)(x["publishedAtDetails"]["hour"].get<double>())/(23.0);
  //     }
  //     outFile << data.dump(4);
  //     outFile.close();
  //     inFile.close();
    
  //   cout << GREEN << "Min Max scalling has done and saved the data at :"  << BOLD << outputFileName << RESET << "\n";
  //   return true;

  // }
  // bool StandardizarionScaling(string dataFileName="glacier1.json",string outputFileName="glacier1std.json"){
  //   struct InumericalDataInsight insights = GetNumericalDataInsights(false,dataFileName);
    
  //   setFsPointer("/app/Data Extraction/data",false);

  //   ifstream inFile(dataFileName);

  //   if(!inFile.is_open()){
  //     cerr << RED <<  "Error : Can't able to read the data file" << RESET << "\n";
  //     return false;

  //   }
    
  //   ofstream outFile(outputFileName);
  //   if(!outFile.is_open()){
  //     cerr << RED <<  "Error : Can't able to create the output file " << RESET << "\n";
  //     return false;

  //   }
  //   json data;

  //   inFile >> data;
  //       for(auto &x:data["data"]){
  //       x["subscriberCount"] = (double)(x["subscriberCount"].get<int>()-insights.mean_sc)/(insights.std_sc);
  //       x["viewCount"] = (double)(x["viewCount"].get<int>()-insights.mean_vc)/(insights.std_vc);
  //       x["averageViewsPerVideo"] = (double)(x["averageViewsPerVideo"].get<double>()-insights.mean_vv)/(insights.std_vv);
  //       x["commentCount"] = (double)(x["commentCount"].get<int>()-insights.mean_cc)/(insights.std_cc);
  //       x["likeCount"] = (double)(x["likeCount"].get<int>()-insights.mean_lc)/(insights.std_lc);
  //   }

  //   outFile << data.dump(4);
  //   outFile.close();
  //   inFile.close();
  //   cout << GREEN << "Standardization scalling is done and saved at : " << BOLD << outputFileName << RESET << "\n";
  //   return true;
  // }


  bool SplitData(string dataFileName="glacier1mm.json",string trsetFileName="glacier1trs.json",string tsetFileName="glacier1ts.json",double ratio=(double)2/10){
      json data = {{"count",0},{"data",json::array()}};

      setFsPointer("/app/Data Extraction/data",false);

      ifstream DataFile(dataFileName);
      if(!DataFile.is_open()){
        cerr << RED <<  "Error : Can't able to read the data file" << RESET << "\n";
        return false;
      }
    DataFile >> data;
    int count = data["count"].get<int>();

    vector<IDataType> ds = data["data"].get<vector<IDataType>>();
    
    std::random_device rd;
    std::mt19937 g(rd());

    shuffle(ds.begin(),ds.end(),g);

    int limit= count * ratio;
    
    vector<IDataType> testset(ds.begin(), ds.begin() + limit);
    vector<IDataType> trainset(ds.begin() + limit, ds.end());

    ofstream TrainSetF(trsetFileName);
    ofstream TestSetF(tsetFileName);

    if(!TrainSetF.is_open()||!TestSetF.is_open()){
        cerr << RED <<  "Error : Can't able to create the data files" << RESET << "\n";
        return false;
    }

    json trsj = {{"count",trainset.size()},{"data",trainset}} ;
    json tsj = {{"count",testset.size()},{"data",testset}} ;


    TrainSetF << trsj.dump(4);
    TestSetF << tsj.dump(4);

    TrainSetF.close();
    TestSetF.close();
    DataFile.close();

    cout << GREEN <<  "Split has been created successfully" << RESET << "\n";

    return true;


  }

};
