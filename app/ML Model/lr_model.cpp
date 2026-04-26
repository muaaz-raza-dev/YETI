#include "../headers/libs.hpp"

#include "../Data Extraction/process_data.cpp"



class LinearRegressionModel{
    vector<double> weights = vector<double>(10, 0.0);

    ProcessData pd;
    vector<IDataType> ds;

    public : 

    LinearRegressionModel(string fileName="glacier1trs.json"){

        setFsPointer("/app/ML Model/",false);
        ifstream ModelFile("lr.json");
        if(ModelFile.is_open()){
            json j;
            ModelFile >> j;
            bool bad = false;

    if (!j.contains("weights") || j["weights"].is_null() || !j["weights"].is_array() || j["weights"][0].is_null()) {
        bad = true;
    }

    if (bad) {
        cerr << RED << "ERROR : Corrupt model detected. Deleting file...\n" << RESET;
        fs::remove("lr.json");
        return;
    }

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
        double ans = 0.0;

        for(auto &x:ds){
            double y0 = fx(x);
            double y = x.expectedViewCount;
            double temp = y-y0;
            ans += temp * temp;
        }
        return ans / ds.size();
    }
    
    double evaluateMAE(){    
        double ans = 0.0;

        for(auto &x:ds){
            double y0 = fx(x);
            double y = x.expectedViewCount;
            ans += abs(y-y0);
        }
        return ans / ds.size();
    }
    
    double fx(IDataType &d){
        return weights[0]*d.commentCount+weights[1] *d.likeCount+weights[2]*d.subscriberCount+weights[3]*d.publishedAtDetails.day_of_week_sin+weights[4]*
        d.publishedAtDetails.day_of_week_cos+weights[5]*d.publishedAtDetails.hour_sin + weights[6]*d.publishedAtDetails.hour_cos + weights[7]*d.averageViewsPerVideo + weights[8]*d.currentViewCount + weights[9];
    }

    void train(float eta=0.0000001,int ephocs=1000000){
        
        int n = ds.size();
        double last_mse = evaluateMSE();
        cout << MAGENTA << "Before MSE(MEAN SQUARE ERROR) : " <<  last_mse<< RESET << "\n";
        cout << CYAN  << "Model Traning has started" << RESET << "\n";
        cout << YELLOW ;
        for (int i = 0; i < ephocs; i++){

            vector<double> g = vector<double>(10, 0.0);
                     
            if(i%1000 ==0  ) cout << "\r" << i << " / " << ephocs-1 << flush;

            for(auto &x:ds){
                double y0 = fx(x);

                double y = x.expectedViewCount;

                double constant_term = (y-y0);

                
                g[0] += (-constant_term * (x.commentCount)) ;
                g[1] += (-constant_term * (x.likeCount)) ;
                g[2] += (-constant_term * (x.subscriberCount)) ;
                g[3] += (-constant_term * (x.publishedAtDetails.day_of_week_sin)) ;
                g[4] += (-constant_term * (x.publishedAtDetails.day_of_week_cos)) ;
                g[5] += (-constant_term * (x.publishedAtDetails.hour_sin)) ;
                g[6] += (-constant_term * (x.publishedAtDetails.hour_cos)) ;
                g[7] += (-constant_term * (x.averageViewsPerVideo)) ;
                g[8] += (-constant_term * (x.currentViewCount)) ;
                g[9] += -constant_term;
            }        

        for(int j=0;j<=9;j++){
            weights[j] -= (eta * (2.0 / n) * g[j] );
        }

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


    long long predict(IDataType &x) {
        return exp(fx(x)) -1 ;
    }

    
    void CompareTestTrainResultsMSE(string TrainfileName="glacier2trs.json",string TestfileName="glacier2ts.json"){
        double tr = evaluateMSE();
        loadData(TestfileName);
        double ts = evaluateMSE();

        loadData(TrainfileName);

        cout << CYAN << "Training set MSE(MEAN SQUARE ERROR) : "<<  BOLD << tr << RESET << "\n";
        cout << CYAN << "Test set MSE(MEAN SQUARE ERROR) : "<<  BOLD << ts <<  RESET << "\n";

        if(tr < ts) cout << RED  << "Over Fitting Detected " << RESET << "\n";
    }

    void CompareTestTrainResultsMAE(string TrainfileName="glacier2trs.json",string TestfileName="glacier2ts.json"){
        double tr = evaluateMAE();
        loadData(TestfileName);
        double ts = evaluateMAE();

        loadData(TrainfileName);

        cout << CYAN << "Training set MAE(MEAN ABSOLUTE ERROR) : "<<  BOLD << tr << RESET << "\n";
        cout << CYAN << "Test set MAE(MEAN ABSOLUTE ERROR) : "<<  BOLD << ts <<  RESET << "\n";

        if(tr < ts) cout << RED  << "Over Fitting Detected " << RESET << "\n";
    }
};
