#include "AIService.h"
#include <curl/curl.h>
#include <sstream>

AIService::AIService(const std::string& api_key, const std::string& base_url)
    : api_key_(api_key), base_url_(base_url) {
}

static size_t WriteCallback(void* contents, size_t size, size_t nmemb, std::string* userp) {
    userp->append((char*)contents, size * nmemb);
    return size * nmemb;
}

AIService::AIResponse AIService::makeRequest(const std::string& endpoint, const nlohmann::json& payload) {
    CURL* curl;
    CURLcode res;
    std::string response_string;
    
    curl = curl_easy_init();
    if (!curl) {
        return {false, "", "Failed to initialize CURL"};
    }
    
    std::string url = base_url_ + endpoint;
    std::string json_data = payload.dump();
    
    struct curl_slist* headers = nullptr;
    std::string auth_header = "Authorization: Bearer " + api_key_;
    headers = curl_slist_append(headers, "Content-Type: application/json");
    headers = curl_slist_append(headers, auth_header.c_str());
    
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, json_data.c_str());
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response_string);
    
    res = curl_easy_perform(curl);
    
    curl_slist_free_all(headers);
    curl_easy_cleanup(curl);
    
    if (res != CURLE_OK) {
        return {false, "", curl_easy_strerror(res)};
    }
    
    try {
        auto response_json = nlohmann::json::parse(response_string);
        return {true, response_string, ""};
    } catch (const std::exception& e) {
        return {false, "", "Failed to parse JSON response: " + std::string(e.what())};
    }
}