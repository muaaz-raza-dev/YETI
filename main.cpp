#include "app/Data Extraction/extract_data.cpp"
#include "app/Data Extraction/process_data.cpp"
int main() {


    // cout << "Search videos [1] \nFetch all videos data [2]" << "\n";
    // int option ;
    // cout << "Your choice : ";
    // cin >> option;
    // cin.ignore(numeric_limits<streamsize>::max(), '\n');
    // if(option == 1){
    //     cout << "How many searches : " ;
    //     int n;
    //     cin >> n;
    //     cin.ignore(numeric_limits<streamsize>::max(), '\n');
    //     string temp ;
    //     for(int i=1;i<=n;i++){
    //         getline(cin ,temp);
    //         string result ="";
    //         for (char c : temp) {
    //             if (c == ' ') result += "%20";
    //             else result += c;
    //         }
    //         cout << i << " Fetching " << result << "\n";
    //         if(!api.FetchVideoIds(result,"2026-03-21","2026-03-14")) break;
    //     }
    //     cout << "Completed" << "\n";
    // }
    // else {
    //     api.FetchBulkVideoDetails();
    // }
    ProcessData pd;
    pd.processssDate();
    
    return 0;
};
