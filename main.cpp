#include "app/Interface/interface.cpp"

int main() {
    UserInterface us;
    string url;
    cout << "Enter the url : "  ;
    cin >> url;
    cout << us.predict(url) << "\n";
    
    return 0;
}
