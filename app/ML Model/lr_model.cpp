#include "../headers/libs.hpp"

#include "../Data Extraction/process_data.cpp"



class LinearRegressionModel{
    double weights[7]={0,0,0,0,0,0,0}; 
    ProcessData pd;
    vector<IDataType> ds;
    public : 

    LinearRegressionModel(string fileName="glacier1trs.json"){
        setFsPointer("/app/ML Model/",false);

        ifstream ModelFile("lr.json");
        if(ModelFile.is_open()){
            json j;
            ModelFile >> j;
            j["weights"].get_to(weights);
            ModelFile.close();
        }
        loadData(fileName);
    }
    void loadData(string fileName="glacier1trs.json"){
        setFsPointer("/app/Data Extraction/data/",false);
        ifstream DataFile(fileName);

        if(!DataFile.is_open()) {
            cout << RED <<"ERROR : Failed to load the data" << RESET <<"\n";
            return;
        }

        json data;
        DataFile >>  data;
        ds.clear();
        for(auto x:data["data"]) ds.push_back(x.get<IDataType>());
        DataFile.close();
    }
    
    double evaluateMSE(){    
        double ans = 0;

        for(auto x:ds){
            double y0 = fx(x.commentCount,x.likeCount,x.subscriberCount,x.publishedAtDetails.day_of_week,x.publishedAtDetails.hour,x.averageViewsPerVideo);
            double y = x.viewCount;
            double temp = y-y0;
            ans += temp * temp;
        }
        return ans / ds.size();
    }
    
    double fx(double x1,double x2,double x3,double x4,double x5,double x6){
        return weights[0]*x1+weights[1] *x2+weights[2]*x3+weights[3]*x4+weights[4]*x5+weights[5]*x6 + weights[6];
    }

    void train(float eta=0.01,int ephocs=500000){
        
        int n = ds.size();
        double last_mse = evaluateMSE();
        cout << MAGENTA << "Before MSE(MEAN SQUARE ERROR) : " <<  last_mse<< RESET << "\n";
        cout << CYAN  << "Model Traning has started" << RESET << "\n";
        cout << YELLOW ;
        for (int i = 0; i < ephocs; i++){

            double g[7] = {0,0,0,0,0,0,0}; 
                     
            if(i%1000 ==0  ) cout << "\r" << i << " / " << ephocs-1 << flush;

            for(auto &x:ds){
                double y0 = fx(x.commentCount,x.likeCount,x.subscriberCount,x.publishedAtDetails.day_of_week,x.publishedAtDetails.hour,x.averageViewsPerVideo);

                double y = x.viewCount;

                double constant_term = (y-y0);

                
                g[0] += (-constant_term * (x.commentCount)) ;
                g[1] += (-constant_term * (x.likeCount)) ;
                g[2] += (-constant_term * (x.subscriberCount)) ;
                g[3] += (-constant_term * (x.publishedAtDetails.day_of_week)) ;
                g[4] += (-constant_term * (x.publishedAtDetails.hour)) ;
                g[5] += (-constant_term * (x.averageViewsPerVideo)) ;
                g[6] += -constant_term;
            }        

        for(int j=0;j<6;j++){
            weights[j] -= (eta * (2.0 / n) * g[j] );
        }
        weights[6] -= (eta * (2.0 / n) * g[6]);

        }
        cout << "\n" << CYAN  << "Model Traning has completed" << RESET << "\n";
        
        json model_att = {{"weights",json::array()}};
        model_att["weights"] = weights;
        setFsPointer("/app/ML Model/",false);
        ofstream ModelFile("lr.json");
        if(!ModelFile.is_open()){
            cout << RED<< "ERROR : could not able to save the trained parameters" << RESET << "\n";
            return;
        }
        ModelFile << model_att.dump(4);
        ModelFile.close();
        cout << MAGENTA << "After MSE(MEAN SQUARE ERROR) : "<< evaluateMSE() <<  RESET << "\n";
        
    }


    long long predict(double commentCount, double likeCount, double subscriberCount, double day_of_week, double hour,double averageViewsPerVideo) {
        InumericalDataInsight d = pd.GetNumericalDataInsights(false,"glacier2.json");
        
        return (fx(commentCount, likeCount, subscriberCount, day_of_week, hour,averageViewsPerVideo) * (d.max_vc - d.min_vc)) + d.min_vc;

    }
    void CompareTestTrainResults(string TrainfileName="glacier2trs.json",string TestfileName="glacier2ts.json"){
        double tr = evaluateMSE();
        loadData(TestfileName);
        double ts = evaluateMSE();

        loadData(TrainfileName);

        cout << CYAN << "Training set MSE(MEAN SQUARE ERROR) : "<<  BOLD << tr << RESET << "\n";
        cout << CYAN << "Test set MSE(MEAN SQUARE ERROR) : "<<  BOLD << ts <<  RESET << "\n";

        if(tr < ts) cout << RED  << "Over Fitting Detected " << RESET << "\n";
    }
};
