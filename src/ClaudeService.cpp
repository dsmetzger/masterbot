#include "ClaudeService.h"
#include <thread>

const std::string ClaudeService::CLAUDE_BASE_URL = "https://api.anthropic.com/v1";
const std::string ClaudeService::MODEL_NAME = "claude-3-sonnet-20240229";

ClaudeService::ClaudeService(const std::string& api_key) 
    : AIService(api_key, CLAUDE_BASE_URL) {
}

std::future<AIService::AIResponse> ClaudeService::generateResponse(const std::string& prompt) {
    return std::async(std::launch::async, [this, prompt]() {
        nlohmann::json payload = {
            {"model", MODEL_NAME},
            {"max_tokens", 1000},
            {"messages", nlohmann::json::array({
                {{"role", "user"}, {"content", prompt}}
            })}
        };
        
        auto response = makeRequest("/messages", payload);
        if (!response.success) {
            return response;
        }
        
        try {
            auto json_response = nlohmann::json::parse(response.content);
            std::string content = json_response["content"][0]["text"];
            return AIResponse{true, content, ""};
        } catch (const std::exception& e) {
            return AIResponse{false, "", "Failed to parse Claude response: " + std::string(e.what())};
        }
    });
}

std::future<AIService::AIResponse> ClaudeService::analyzePreferences(const std::string& user_data) {
    std::string prompt = "Analyze the following user data and extract preferences for event recommendations:\n" + user_data;
    return generateResponse(prompt);
}

std::future<AIService::AIResponse> ClaudeService::recommendEvents(
    const std::string& preferences, 
    const std::string& available_events) {
    
    std::string prompt = "Based on these user preferences:\n" + preferences + 
                        "\n\nRecommend events from this list:\n" + available_events +
                        "\n\nProvide a ranked list with explanations.";
    return generateResponse(prompt);
}