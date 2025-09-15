#pragma once
#include <string>
#include <vector>
#include <future>
#include <nlohmann/json.hpp>

class AIService {
public:
    AIService(const std::string& api_key, const std::string& base_url = "");
    virtual ~AIService() = default;

    struct AIResponse {
        bool success;
        std::string content;
        std::string error_message;
    };

    virtual std::future<AIResponse> generateResponse(const std::string& prompt) = 0;
    virtual std::future<AIResponse> analyzePreferences(const std::string& user_data) = 0;
    virtual std::future<AIResponse> recommendEvents(
        const std::string& preferences, 
        const std::string& available_events) = 0;

protected:
    std::string api_key_;
    std::string base_url_;
    
    AIResponse makeRequest(const std::string& endpoint, const nlohmann::json& payload);
};