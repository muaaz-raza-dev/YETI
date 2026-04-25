#include "../headers/libs.hpp"

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

  json get(string url,bool print = false) {
    json j={{"status",false},{"payload",""}};
    if(!curl){
        cerr << "Failed to initialize curl" << endl;
        return j;
      }
    response.clear();
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

    result = curl_easy_perform(curl);
    if(result  != CURLE_OK){
       cerr << "Request failed: " << curl_easy_strerror(result) << endl;
      return j;
    }
    
    j["status"] = true;
    j["payload"] = json::parse(response);
    if(print) PrintResponse();
    return j;
  }

  void PrintResponse() const{
    cout << "Server Response: " << response << endl;
  }


};

