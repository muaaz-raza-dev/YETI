#include <curl/curl.h>
#include <bits/stdc++.h>
using namespace std;
#include "json.hpp"
using json = nlohmann::json;

class API{
  CURL* curl;
  CURLcode result;
  string response;

  static size_t WriteCallback(void* contents, size_t size, size_t nmemb, string* resp) {
      size_t totalSize = size * nmemb;
      resp->append((char*)contents, totalSize);
      return totalSize;
  }

  public:
    API (){
    curl = curl_easy_init();
    if(!curl) {
      cerr << "Failed to initialize curl" << endl;
    }
  }
  ~API() {
        if(curl) curl_easy_cleanup(curl);
        curl_global_cleanup();
  }

  bool get(string url,bool print = false) {
    if(!curl){
        cerr << "Failed to initialize curl" << endl;
        return false;
      }
    response.clear();
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

    result = curl_easy_perform(curl);
    if(result  != CURLE_OK){
       cerr << "Request failed: " << curl_easy_strerror(result) << endl;
      return false;
    }
    if(print) PrintResponse();
    return true;
  }

  void PrintResponse() const{
    cout << "Server Response: " << response << endl;
  }


};

int main() {

    API api;
    api.get("http://127.0.0.1:8000/",true);

    
    return 0;
}