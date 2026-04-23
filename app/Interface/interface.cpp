#include "../Data Extraction/extract_data.cpp"
#include "../ML Model/lr_model.cpp"

class UserInterface
{
    string url;
    YTAPI yt;
    ProcessData pd;
    LinearRegressionModel ml;

public:
    const string RESET = "\033[0m";
    const string BOLD = "\033[1m";
    const string RED = "\033[31m";
    const string GREEN = "\033[32m";
    const string YELLOW = "\033[33m";
    const string BLUE = "\033[34m";
    const string MAGENTA = "\033[35m";
    const string CYAN = "\033[36m";

    void ClearInput()
    {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
    }

    void Interface()
    {
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

                        if (cin >> option)
                        {
                            if (option == 0)
                                break;
                            else if (option == 1) {predict() ;
                                cout << GREEN << "Predicting...\n"
                                     << RESET;}
                            else if (option == 2) { ml.train(0.001, 1000) ;
                                cout << GREEN << "Training model...\n"
                                     << RESET;}
                            else if (option == 3) { ml.evaluateMSE();
                                cout << GREEN << "Evaluating MSE...\n"
                                     << RESET;
                                }
                            else cout << RED << "Invalid Option Selected.\n" << RESET;
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
                                        cout << GREEN << "[" << i << "] Fetching " << result << "...\n"
                                             << RESET;
                                        if (!yt.FetchVideoIds(result)) break;
                                    }
                                    cout << GREEN << "--- Completed ---\n"
                                         << RESET;
                                }
                                else
                                {
                                    cout << RED << "Invalid number of queries.\n"
                                         << RESET;
                                    ClearInput();
                                }
                            }
                            else if (option == 2){ yt.FetchBulkVideoDetails();
                                cout << GREEN << "Fetching bulk details...\n"
                                     << RESET;}
                            else if (option == 3)  {
                                yt.FetchVideoDetailsTarget() ;
                                cout << GREEN << "Fetching target details...\n"
                                     << RESET;
                                }
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
                        cout << YELLOW << "[2]" << RESET << " Get Insights\n";
                        cout << YELLOW << "[3]" << RESET << " Scale Data\n";
                        cout << YELLOW << "[4]" << RESET << " Split Data\n";
                        cout << RED << "[0]" << RESET << " Back to Main Menu\n";
                        cout << BOLD << "Your Choice: " << RESET;

                        if (cin >> option)
                        {
                            if (option == 0)
                                break;
                            else if (option == 1) {

                                CleanData();
                                cout << GREEN << "Cleaning Data...\n"
                                << RESET;
                            } 
                            else if (option == 2) { 
                                pd.GetNumericalDataInsights(true,"glacier2.json") ;
                                cout << GREEN << "Getting Insights...\n"
                                     << RESET;
                            }
                            else if (option == 3)
                            {
                                int scaling;
                                cout << "\n"
                                     << BOLD << "--- Scaling Options ---\n"
                                     << RESET;
                                cout << YELLOW << "[1]" << RESET << " Min Max Scaling\n";
                                cout << YELLOW << "[2]" << RESET << " Standardization Scaling\n";
                                cout << BOLD << "Your Choice: " << RESET;

                                if (cin >> scaling)
                                {
                                    if (scaling == 1) {
                                        pd.ScaleMinMaxAll("glacier2.json","glacier2mm.json") ;
                                        cout << GREEN << "Applying Min Max Scaling...\n"
                                             << RESET;}
                                    else if (scaling == 2) { pd.StandardizarionScaling("glacier2.json","glacier2std.json") ;
                                        cout << GREEN << "Applying Standardization...\n"
                                             << RESET;
                                    }
                                    else cout << RED << "Invalid Scaling Option.\n" << RESET;
                                }
                                else
                                {
                                    cout << RED << "Invalid input.\n"
                                         << RESET;
                                    ClearInput();
                                }
                            }
                            else if (option == 4) { pd.SplitData("glacier2mm.json","glacier2trs.json","glacier2ts.json");
                                cout << GREEN << "Splitting Data...\n" << RESET;
                                    }
                            else
                                cout << RED << "Invalid Option Selected.\n"
                                     << RESET;
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

    long long predict()
    {
        string url;
        cout << "Enter the URL of the yotube video : ";
        cin >> url;
        IFetchVideoDataResponse params = yt.FetchVideoData(url);
        if (!params.status)
        {
            cout << params.message << "\n";
            return -99.99;
        }

        pd.ScaleMinMax(params.payload);
        IDataType &d = params.payload;
        return ml.predict(d.commentCount, d.likeCount, d.subscriberCount, d.publishedAtDetails.day_of_week, d.publishedAtDetails.hour,d.averageViewsPerVideo);
    }
    void SearchVideos()
    {
        yt.FetchDataCLI();
    }
    void CleanData()
    {
        pd.removeDuplicates("glacier2.json");
        pd.FixType("glacier2.json");
        pd.processssDate("glacier2.json");
    }
};