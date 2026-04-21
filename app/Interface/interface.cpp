#include "../Data Extraction/extract_data.cpp"
#include "../ML Model/lr_model.cpp"

class UserInterface{
    string url;
    YTAPI yt;
    ProcessData pd;
    LinearRegressionModel ml;
    public :
    long long predict(string url){
        IFetchVideoDataResponse params = yt.FetchVideoData(url);
        if(!params.status){
            cout << params.message << "\n";
            return -99.99;
        }
        
        pd.ScaleMinMax(params.payload);
        IDataType &d = params.payload;
        return ml.predict(d.commentCount,d.likeCount,d.subscriberCount,d.publishedAtDetails.day_of_week,d.publishedAtDetails.hour);
    }
    void SearchVideos(){ 
        yt.FetchDataCLI();
    }
};