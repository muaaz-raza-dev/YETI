#include "../headers/libs.hpp"
#include "../Data Extraction/process_data.cpp"

class LinearRegressionModel {
    vector<double> weights = vector<double>(10, 0.0);
    ProcessData pd;
    vector<IDataType> ds;

    void saveModel() {
        json model_att = {{"weights", weights}};
        setFsPointer("/app/ML Model/", false);
        ofstream ModelFile("lr.json");
        if (ModelFile.is_open()) {
            ModelFile << model_att.dump(4);
            ModelFile.close();
        } else {
            cout << RED << " [!] Error: Could not save trained parameters." << RESET << "\n";
        }
    }

public:
    LinearRegressionModel(string fileName = "glacier1trs.json") {
        setFsPointer("/app/ML Model/", false);
        ifstream ModelFile("lr.json");
        if (ModelFile.is_open()) {
            json j;
            ModelFile >> j;
            if (!j.contains("weights") || j["weights"].is_null() || !j["weights"].is_array()) {
                cerr << RED << " [!] Corrupt model detected. Resetting weights...\n" << RESET;
                fs::remove("lr.json");
            } else {
                j["weights"].get_to(weights);
            }
            ModelFile.close();
        }
        loadData(fileName);
    }

    void loadData(string fileName = "glacier1trs.json") {
        setFsPointer("/app/Data Extraction/data/", false);
        ifstream DataFile(fileName);
        if (!DataFile.is_open()) {
            cout << RED << " [!] Error: Failed to load " << fileName << RESET << "\n";
            return;
        }
        json data;
        DataFile >> data;
        ds.clear();
        for (auto& x : data["data"]) ds.push_back(x.get<IDataType>());
        DataFile.close();
    }

    double evaluateMSE() {
        if (ds.empty()) return 0.0;
        double ans = 0.0;
        for (auto& x : ds) {
            double diff = x.expectedViewCount - fx(x);
            ans += diff * diff;
        }
        return ans / ds.size();
    }

    double evaluateMAE() {
        if (ds.empty()) return 0.0;
        double ans = 0.0;
        for (auto& x : ds) {
            ans += abs(x.expectedViewCount - fx(x));
        }
        return ans / ds.size();
    }

    double fx(IDataType& d) {
        return weights[0] * d.commentCount +
               weights[1] * d.likeCount +
               weights[2] * d.subscriberCount +
               weights[3] * d.publishedAtDetails.day_of_week_sin +
               weights[4] * d.publishedAtDetails.day_of_week_cos +
               weights[5] * d.publishedAtDetails.hour_sin +
               weights[6] * d.publishedAtDetails.hour_cos +
               weights[7] * d.averageViewsPerVideo +
               weights[8] * d.currentViewCount +
               weights[9]; 
    }

    void train(float eta = 0.0000001, int epochs = 1000) {
        int n = ds.size();
        if (n == 0) {
            cout << RED << " [!] No training data found.\n" << RESET;
            return;
        }

        double initial_mse = evaluateMSE();
        cout << "\n" << MAGENTA << "+--- TRAINING ENGINE ---+" << RESET << "\n";
        cout << " Epochs: " << epochs << " | LR: " << eta << "\n";
        cout << " Start MSE: " << YELLOW << initial_mse << RESET << "\n\n";

        for (int i = 0; i < epochs; i++) {
            vector<double> g(10, 0.0);

            if (i % (max(1, epochs / 20)) == 0 || i == epochs - 1) {
                float prog = (float)(i + 1) / epochs;
                int bar = 20 * prog;
                cout << "\r " << CYAN << "[" << string(bar, '=') << string(20 - bar, ' ') << "] " 
                     << int(prog * 100) << "%" << RESET << flush;
            }

            for (auto& x : ds) {
                double error = (x.expectedViewCount - fx(x));
                g[0] += (-error * x.commentCount);
                g[1] += (-error * x.likeCount);
                g[2] += (-error * x.subscriberCount);
                g[3] += (-error * x.publishedAtDetails.day_of_week_sin);
                g[4] += (-error * x.publishedAtDetails.day_of_week_cos);
                g[5] += (-error * x.publishedAtDetails.hour_sin);
                g[6] += (-error * x.publishedAtDetails.hour_cos);
                g[7] += (-error * x.averageViewsPerVideo);
                g[8] += (-error * x.currentViewCount);
                g[9] += -error;
            }

            for (int j = 0; j <= 9; j++) {
                weights[j] -= (eta * (2.0 / n) * g[j]);
            }
        }

        double final_mse = evaluateMSE();
        saveModel();

        cout << "\n\n" << GREEN << " [+] Optimization Complete." << RESET << "\n";
        cout << " Final MSE : " << BOLD << final_mse << RESET << "\n";
        cout << " Improve   : " << YELLOW << ((initial_mse - final_mse) / initial_mse) * 100 << "%" << RESET << "\n";
        cout << " ------------------------------------\n";
    }

    string predict(IDataType& x) {
        long long value = exp(fx(x)) - 1;
        if (value < 0) value = 0;
        
        string response;
        if (value >= 1000000) response = format("{:.2f}M", (double)value / 1000000.0);
        else if (value >= 1000) response = format("{:.2f}K", (double)value / 1000.0);
        else response = to_string(value);

        
        return response;
    }

    void CompareTestTrainResultsMSE(string TrainfileName = "glacier2trs.json", string TestfileName = "glacier2ts.json") {
        double tr = evaluateMSE();
        loadData(TestfileName);
        double ts = evaluateMSE();
        loadData(TrainfileName); // Reload train data for state consistency

        cout << "\n" << BLUE << "+--- PERFORMANCE GAP (MSE) ---+" << RESET << "\n";
        cout << " Train: " << tr << "\n";
        cout << " Test : " << ts << "\n";
        
        if (tr < ts * 0.7) cout << RED << BOLD << " [!] OVERFITTING DETECTED\n" << RESET;
        else cout << GREEN << " [✓] MODEL IS STABLE\n" << RESET;
        cout << " +----------------------------+\n";
    }

    void CompareTestTrainResultsMAE(string TrainfileName = "glacier2trs.json", string TestfileName = "glacier2ts.json") {
        double tr = evaluateMAE();
        loadData(TestfileName);
        double ts = evaluateMAE();
        loadData(TrainfileName);

        cout << "\n" << BLUE << "+--- PERFORMANCE GAP (MAE) ---+" << RESET << "\n";
        cout << " Train: " << tr << "\n";
        cout << " Test : " << ts << "\n";

        if (tr < ts * 0.7) cout << RED << BOLD << " [!] OVERFITTING DETECTED\n" << RESET;
        else cout << GREEN << " [✓] MODEL IS STABLE\n" << RESET;
        cout << " +----------------------------+\n";
    }
};