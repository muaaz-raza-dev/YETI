#include "../Data Extraction/extract_data.cpp"
#include "../ML Model/lr_model.cpp"
#include "../headers/libs.hpp"

class UserInterface
{
    string url;
    YTAPI yt;
    ProcessData pd;
    LinearRegressionModel ml{"glacier2-s1.json"};

public:
    void ClearInput()
    {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
    }

    void Interface(){
        int choice;
        while (true)
        {
            cout << "\n"
                 << BOLD << CYAN << "====================================\n";
            cout << "         YETI - THE MASTER          \n";
            cout << "====================================\n"
                 << RESET;
            cout << YELLOW << "[1]" << RESET << " Machine Learning Model\n";
            cout << YELLOW << "[2]" << RESET << " Youtube Data API\n";
            cout << YELLOW << "[3]" << RESET << " Process Data\n";
            cout << RED << "[0]" << RESET << " Exit\n";
            cout << BOLD << "Your Choice: " << RESET;

            if (cin >> choice)
            {
                if (choice == 0)
                {
                    cout << GREEN << "Exiting YETI. Goodbye!\n"
                         << RESET;
                    break;
                }

                switch (choice)
                {
                case 1:
                {
                    while (true)
                    {
                        int option;
                        cout << "\n"
                             << BOLD << MAGENTA << "------- Linear Regression Model -------\n"
                             << RESET;
                        cout << YELLOW << "[1]" << RESET << " Predict\n";
                        cout << YELLOW << "[2]" << RESET << " Train\n";
                        cout << YELLOW << "[3]" << RESET << " Get Metrics\n";
                        cout << RED << "[0]" << RESET << " Back to Main Menu\n";
                        cout << BOLD << "Your Choice: " << RESET;

                        if (cin >> option){
                            if (option == 0) break;
                            else if (option == 1)  predict() ;
                            else if (option == 2)  ml.train() ;
                            else if (option == 3)  {
                                while(true){

                                    int metric;
                                    cout << BOLD << MAGENTA << "------- Model Assestment Metirics -------\n" << RESET;
                                    cout << YELLOW << "[1]" << RESET << " MSE (Mean Square Error) \n";
                                    cout << YELLOW << "[2]" << RESET << " MAE (Mean absolute Error) \n";
                                    cout << YELLOW << "[3]" << RESET << " Compare MSE (Mean Square Error) \n";
                                    cout << YELLOW << "[4]" << RESET << " Compare MAE (Mean Square Error) \n";
                                cout << RED << "[0]" << RESET << " Back to Main Menu\n";
                                cout << BOLD << "Your Choice: " << RESET;
                                if(cin >> metric){
                                    if(metric == 0) break;
                                    else if(metric == 1) cout << CYAN <<  "MSE : "  << BOLD <<  ml.evaluateMSE() << RESET << "\n";
                                    else if(metric == 2) cout << CYAN <<  "MAE : "  << BOLD <<  ml.evaluateMAE() << RESET << "\n";
                                    else if(metric == 3) ml.CompareTestTrainResultsMSE();
                                    else if(metric == 4) ml.CompareTestTrainResultsMAE();
                                    else cout << RED << "Invalid Option Selected.\n" << RESET;
                                }
                                else {cout << RED << "Invalid Option Selected.\n" << RESET; ClearInput();} 
                            }
                        }
                            else {cout << RED << "Invalid Option Selected.\n" << RESET;ClearInput();}
                        }
                        else {
                            cout << RED << "Invalid input. Please enter a number.\n" << RESET;
                            ClearInput();
                        }
                    }
                    break;
                }

                case 2:
                {
                    while (true)
                    {
                        int option;
                        cout << "\n"<< BOLD << BLUE << "---------- YOUTUBE DATA API ----------\n"<< RESET;
                        cout << YELLOW << "[1]" << RESET << " Fetch Videos (Query)\n";
                        cout << YELLOW << "[2]" << RESET << " Fetch Video's Data (Queried videos)\n";
                        cout << YELLOW << "[3]" << RESET << " Fetch Video's Data Target (Views)\n";
                        cout << RED << "[0]" << RESET << " Back to Main Menu\n";
                        cout << BOLD << "Your Choice: " << RESET;

                        if (cin >> option)
                        {
                            if (option == 0)
                                break;
                            else if (option == 1)
                            {
                                int searches;
                                cout << CYAN << "How many Queries (1-50): " << RESET;
                                if (cin >> searches)
                                {
                                    ClearInput(); 
                                    string temp;
                                    for (int i = 1; i <= searches; i++)
                                    {
                                        cout << "Enter query " << i << ": ";
                                        getline(cin, temp);
                                        string result = "";
                                        for (char c : temp)
                                        {
                                            if (c == ' ')
                                                result += "%20";
                                            else
                                                result += c;
                                        }
                                        cout << "\r" <<  GREEN << "[" << i << "] Fetching " << result << "...\n" << RESET;
                                        if (!yt.FetchVideoIds(result)) break;
                                    }
                                    cout << "\r" << GREEN << "--- Completed ---\n" << RESET;
                                }
                                else
                                {
                                    cout << RED << "Invalid number of queries.\n"
                                         << RESET;
                                    ClearInput();
                                }
                            }
                            else if (option == 2) yt.FetchBulkVideoDetails();
                            else if (option == 3)   yt.FetchVideoDetailsTarget() ;
                            else cout << RED << "Invalid Choice.\n" << RESET;
                        }
                        else
                        {
                            cout << RED << "Invalid input. Please enter a number.\n"
                                 << RESET;
                            ClearInput();
                        }
                    }
                    break;
                }

                case 3:
                {
                    while (true)
                    {
                        int option;
                        cout << "\n"
                             << BOLD << GREEN << "------------ Process Data ------------\n"
                             << RESET;
                        cout << YELLOW << "[1]" << RESET << " Clean the Data (Type fix, Duplicates, Dates)\n";
                        // cout << YELLOW << "[2]" << RESET << " Get Insights\n";
                        cout << YELLOW << "[3]" << RESET << " Scale Data\n";
                        cout << YELLOW << "[4]" << RESET << " Split Data\n";
                        cout << RED << "[0]" << RESET << " Back to Main Menu\n";
                        cout << BOLD << "Your Choice: " << RESET;

                        if (cin >> option){
                            if (option == 0) break;
                            else if (option == 1) CleanData(); 
                            // else if (option == 2)  pd.GetNumericalDataInsights(true,"glacier2.json") ;
                            else if (option == 3)  StandardScalling();
                            else if (option == 4){  pd.SplitData("glacier2-s1.json","glacier2trs.json","glacier2ts.json"); ml.loadData("glacier2trs.json");}
                            else cout << RED << "Invalid Option Selected.\n" << RESET;
                        }
                        else {
                            cout << RED << "Invalid input. Please enter a number.\n" << RESET;
                            ClearInput();
                        }
                    }
                    break; 
                }

                default:
                    cout << RED << "Invalid Selection. Please choose 1, 2, 3, or 0.\n"
                         << RESET;
                    break;
                }
            }
            else
            {
                cout << RED << "Invalid input. Please enter a valid number.\n"
                     << RESET;
                ClearInput();
            }
        }
    }

    void predict(){

        string url;
        cout << "Enter the URL of the yotube video : ";
        cin >> url;
        IFetchVideoDataResponse params = yt.FetchVideoData(url);
        if (!params.status){
            cout << RED << "ERROR : " <<  params.message << RESET << "\n";
            return ;
        }
        IDataType &d = params.payload;
        StandardScalling(d);
        cout << GREEN << "Predicted views (7th day) : " << BOLD<<   ml.predict(d) << RESET<<"\n";
    }

    void StandardScalling(IDataType &d){
        pd.LogScalling(d);
        pd.CyclicEncoding(d);
    }
    void StandardScalling(){
        pd.LogScalling("glacier2.json","glacier2-s1.json");
        pd.CyclicEncoding("glacier2-s1.json","glacier2-s1.json");
    }
    void CleanData()
    {
        pd.removeDuplicates("glacier2.json");
        pd.FixType("glacier2.json");
        pd.processssDate("glacier2.json");
    }
};