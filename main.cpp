// #include "app/Data Extraction/process_data.cpp"
#include "app/ML Model/lr_model.cpp"

int main() {
    
    LinearRegressionModel model;
    cout << model.evaluateMSE() << "\n";
    // model.train(0.1,100000);
    // cout << model.evaluateMSE()  << "\n";
    
    return 0;
}
