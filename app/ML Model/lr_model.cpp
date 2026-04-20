#include "../headers/libs.hpp"

#include "../Data Extraction/process_data.cpp"

// struct IDataType {
//?     float commentCount;1
//?     float likeCount; 1
//?     PublishedAtDetails publishedAtDetails; 2
//?     float subscriberCount; 1
//?    float viewCount;1 
// };

class LinearRegressionModel{
    int nums_parameters = 5; // based on data;
    vector<double> weights={0,0,0,0,0,0}; 
    ProcessData pd;
    vector<IDataType> ds;
    public : 
    LinearRegressionModel(){
        setFsPointer("/app/ML Model/",false);

        ifstream ModelFile("lr.json");
        if(ModelFile.is_open()){
            json j;
            ModelFile >> j;
            j["weights"].get_to(weights);
            ModelFile.close();
        }
        
        
        setFsPointer("/app/Data Extraction/data/");
        ifstream DataFile("glacier1ts.json");

        if(!DataFile.is_open()) {
            cout << "Failed to load the data" << "\n";
            return;
        }
        json data;
        DataFile >>  data;
        for(auto x:data["data"]) ds.push_back(x.get<IDataType>());
        DataFile.close();
        cout << "Data has been loaded" << "\n";
    }

    
    double evaluateMSE(){    
        double ans = 0;

        for(auto x:ds){
            double y0 = fx(x.commentCount,x.likeCount,x.subscriberCount,x.publishedAtDetails.day_of_week,x.publishedAtDetails.hour);
            double y = x.viewCount;
            double temp = y-y0;
            ans += temp * temp;
        }
        return ans / ds.size();
    }
    
    double fx(double x1,double x2,double x3,double x4,double x5){
        return weights[0]*x1+weights[1] *x2+weights[2]*x3+weights[3]*x4+weights[4]*x5+weights[5];
    }

    void train(float eta=0.01,int ephocs=1){
        int g = 0;
        int n = ds.size();

        for (int i = 0; i < ephocs; i++){

            double g0 =0 , g1=0,g2 =0,g3 =0, g4=0,g5=0;          

            for(auto &x:ds){
                double y0 = fx(x.commentCount,x.likeCount,x.subscriberCount,x.publishedAtDetails.day_of_week,x.publishedAtDetails.hour);

                double y = x.viewCount;

                double constant_term = (y-y0);

                
                g0 += (-constant_term * (x.commentCount)) ;
                g1 += (-constant_term * (x.likeCount)) ;
                g2 += (-constant_term * (x.subscriberCount)) ;
                g3 += (-constant_term * (x.publishedAtDetails.day_of_week)) ;
                g4 += (-constant_term * (x.publishedAtDetails.hour)) ;
                g5 += -constant_term ;
            }        

          weights[0] -= (eta * (2.0 / n) * g0);
          weights[1] -= (eta * (2.0 / n) * g1);
          weights[2] -= g2* (2.0 / n) * eta;  
          weights[3] -= g3* (2.0 / n) * eta;  
          weights[4] -= g4* (2.0 / n) * eta;  
          weights[5] -= g5* (2.0 / n) * eta;  
          
        }
        
        json model_att = {{"weights",json::array()}};
        model_att["weights"] = weights;
        setFsPointer("/app/ML Model/",false);
        ofstream ModelFile("lr.json");
        if(!ModelFile.is_open()){
            cout << "can't able to save the trained parameters" << "\n";
            return;
        }
        ModelFile << model_att.dump(4);
        ModelFile.close();
        cout << "training is done" << "\n";
    }


    long long predict(double commentCount, double likeCount, double subscriberCount, double day_of_week, double hour) {
        InumericalDataInsight d = pd.GetNumericalDataInsights(false);


        return (fx(commentCount, likeCount, subscriberCount, day_of_week, hour) * (d.max_vc - d.min_vc)) + d.min_vc;

    }

};
