#include "../Data Extraction/extract_data.cpp"
#include "../ML Model/lr_model.cpp"
#include "../headers/libs.hpp"



class UserInterface {
    YTAPI yt;
    ProcessData pd;
    LinearRegressionModel ml{"glacier2-s1.json"};
    void PrintHeader(string title, string color) {
    const int width = 40;
    int total_padding = width - (int)title.length();
    int left_padding = max(0, total_padding / 2);
    int right_padding = max(0, total_padding - left_padding);

    string line = string(width, '-');

    cout << "\n" << color << "+" << line << "+" << RESET << "\n";
    cout << color << "| " << RESET 
         << string(max(0, left_padding - 1), ' ') << BOLD << title << RESET 
         << color << string(max(0, right_padding - 1), ' ') << " |" << RESET << "\n";
    cout << color << "+" << line << "+" << RESET << "\n";
    }

    void ClearInput() {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
    }

public:
    void Interface() {
        int choice;
        while (true) {
            PrintHeader("YETI - THE MASTER", CYAN);
            cout << YELLOW << " [1] " << RESET << "Machine Learning Model\n";
            cout << YELLOW << " [2] " << RESET << "YouTube Data API\n";
            cout << YELLOW << " [3] " << RESET << "Process Data Pipeline\n";
            cout << RED    << " [0] " << RESET << "Exit System\n";
            cout << DIM << " ────────────────────────────────────" << RESET << "\n";
            cout << BOLD << " Selection » " << RESET;

            if (!(cin >> choice)) {
                cout << RED << "(!) Invalid input. Please enter a number.\n" << RESET;
                ClearInput();
                continue;
            }

            if (choice == 0) break;

            switch (choice) {
                case 1: MLSubMenu(); break;
                case 2: APISubMenu(); break;
                case 3: DataSubMenu(); break;
                default: cout << RED << "(!) Selection out of range.\n" << RESET;
            }
        }
    }

private:
    void MLSubMenu() {
        int option;
        while (true) {
            PrintHeader("LINEAR REGRESSION MODEL", MAGENTA);
            cout << YELLOW << " [1] " << RESET << "Predict Video Performance\n";
            cout << YELLOW << " [2] " << RESET << "Train Model\n";
            cout << YELLOW << " [3] " << RESET << "View Assessment Metrics\n";
            cout << RED    << " [0] " << RESET << "Back to Main\n";
            cout << BOLD << " Selection » " << RESET;

            if (!(cin >> option)) { ClearInput(); continue; }
            if (option == 0) break;

            switch (option) {
                case 1: predict(); break;
                case 2: 
                    cout << CYAN << "[*] Training started..." << RESET << endl;
                    ml.train(); 
                    cout << GREEN << "[+] Training Complete." << RESET << endl;
                    break;
                case 3: MetricsSubMenu(); break;
                default: cout << RED << "(!) Invalid Choice.\n" << RESET;
            }
        }
    }

void MetricsSubMenu() {
    int metric;
    while (true) {
        PrintHeader("MODEL ASSESSMENT", MAGENTA);
        
        // Aligned and coherent option list
        cout << YELLOW << " [1] " << RESET << "MSE (Mean Square Error)\n";
        cout << YELLOW << " [2] " << RESET << "MAE (Mean Absolute Error)\n";
        cout << YELLOW << " [3] " << RESET << "Compare Train/Test MSE\n";
        cout << YELLOW << " [4] " << RESET << "Compare Train/Test MAE\n";
        cout << DIM    << " ------------------------------------" << RESET << "\n";
        cout << RED    << " [0] " << RESET << "Return to ML Menu\n";
        
        cout << "\n" << BOLD << " Selection » " << RESET;

        if (!(cin >> metric)) { 
            cout << RED << " [!] Numeric input only." << RESET << endl;
            ClearInput(); 
            continue; 
        }
        
        if (metric == 0) break;

        // Visual Separator for Results
        cout << DIM << " Result: " << RESET;
        switch (metric) {
            case 1: cout << CYAN << "MSE = " << BOLD << ml.evaluateMSE() << RESET << "\n"; break;
            case 2: cout << CYAN << "MAE = " << BOLD << ml.evaluateMAE() << RESET << "\n"; break;
            case 3: ml.CompareTestTrainResultsMSE(); break;
            case 4: ml.CompareTestTrainResultsMAE(); break;
            default: cout << RED << "Invalid Selection." << RESET << "\n";
        }
        
        // Pause briefly so the user can read the metric result
        cout << DIM << "\n Press Enter to continue..." << RESET;
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cin.get();
    }
}
void APISubMenu() {
    int option;
    while (true) {
        PrintHeader("YOUTUBE DATA API", BLUE);
        cout << YELLOW << " [1] " << RESET << "Search Videos via Query\n";
        cout << YELLOW << " [2] " << RESET << "Fetch Metadata (Bulk)\n";
        cout << YELLOW << " [3] " << RESET << "Update Target Metrics\n";
        cout << DIM    << " ------------------------------------" << RESET << "\n";
        cout << RED    << " [0] " << RESET << "Return to Main Menu\n";
        cout << "\n" << BOLD << " Selection » " << RESET;

        if (!(cin >> option)) { 
            cout << RED << " [!] Numeric input only." << RESET << endl;
            ClearInput(); 
            continue; 
        }
        
        if (option == 0) break;

        switch (option) {
            case 1: {
                int searches;
                cout << CYAN << " Number of Queries (1-50): " << RESET;
                
                // Fix 1: Validate numeric input for searches
                if (!(cin >> searches)) {
                    cout << RED << " [!] Invalid number." << RESET << endl;
                    ClearInput();
                    break;
                }
                
                // Fix 2: CRITICAL - Clear the buffer before the first getline
                ClearInput(); 

                for (int i = 1; i <= searches; i++) {
                    string temp, result = "";
                    cout << DIM << " [" << i << "/" << searches << "]" << RESET << " Query: ";
                    
                    if (!getline(cin, temp) || temp.empty()) {
                        i--; // Retake this index if input failed
                        continue;
                    }

                    // URL Encoding logic
                    for (char c : temp) {
                        if (c == ' ') result += "%20";
                        else result += c;
                    }

                    cout << GREEN << "  * Requesting: " << RESET << temp << "...\n";
                    if (!yt.FetchVideoIds(result)) {
                        cout << RED << "  [!] API limit reached or connection lost." << RESET << endl;
                        break;
                    }
                }
                cout << GREEN << "\n [+] Batch processing complete." << RESET << endl;
                break;
            }

            case 2: 
                cout << CYAN << " [*] Running Bulk Metadata Fetcher..." << RESET << endl;
                yt.FetchBulkVideoDetails(); 
                break;

            case 3: 
                cout << CYAN << " [*] Syncing Target View Counts..." << RESET << endl;
                yt.FetchVideoDetailsTarget(); 
                break;

            default:
                cout << RED << " [!] Selection out of range." << RESET << endl;
        }

        // Pause to let the user see the result of the operation
        cout << DIM << "\n Press Enter to return..." << RESET;
        cin.get(); 
    }
}
    void DataSubMenu() {
        int option;
        while (true) {
            PrintHeader("DATA PIPELINE", GREEN);
            cout << YELLOW << " [1] " << RESET << "Clean Data (Types/Duplicates/Dates)\n";
            cout << YELLOW << " [2] " << RESET << "Scale Data (Log/Cyclic)\n";
            cout << YELLOW << " [3] " << RESET << "Split Train/Test Sets\n";
            cout << RED    << " [0] " << RESET << "Back to Main\n";
            cout << BOLD << " Selection » " << RESET;

            if (!(cin >> option)) { ClearInput(); continue; }
            if (option == 0) break;

            switch (option) {
                case 1: CleanData(); break;
                case 2: StandardScalling(); cout << GREEN << "[Done] Scaling Applied.\n" << RESET; break;
                case 3: 
                    pd.SplitData("glacier3-s1.json", "glacier3trs.json", "glacier3ts.json"); 
                    ml.loadData("glacier3trs.json");
                    cout << GREEN << "[Done] Data Split & Loaded.\n" << RESET;
                    break;
            }
        }
    }

    void predict() {
        string url;
        cout << "\n" << BOLD << "Enter YouTube URL: " << RESET;
        cin >> url;
        auto params = yt.FetchVideoData(url);
        if (!params.status) {
            cout << RED << "Error: " << params.message << RESET << "\n";
            return;
        }
        IDataType &d = params.payload;
        StandardScalling(d);
        cout << "\n " << BOLD << YELLOW << ">>> PREDICTION RESULT <<<" << RESET << "\n";
        cout << " Expected (7-Day): " << BOLD << GREEN << ml.predict(d) << RESET << " views\n\n";
    }

    void StandardScalling(IDataType &d) { pd.LogScalling(d); pd.CyclicEncoding(d); }
    void StandardScalling() { 
        pd.LogScalling("glacier3.json", "glacier3-s1.json"); 
        pd.CyclicEncoding("glacier3-s1.json", "glacier3-s1.json"); 
    }
    void CleanData() {
        if(!pd.removeDuplicates("glacier3.json") || !pd.FixType("glacier3.json") || !pd.processssDate("glacier3.json")){
            cout <<  DIM << GREEN << "[Done] Data Cleaned." << RESET << "\n";
        }
        
        

    }
};