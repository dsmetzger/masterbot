#include "OpenAIService.h"
#include <thread>

const std::string OpenAIService::OPENAI_BASE_URL = "https://api.openai.com/v1";
const std::string OpenAIService::MODEL_NAME = "gpt-3.5-turbo";

OpenAIService::OpenAIService(const std::string& api_key) 
    : AIService(api_key, OPENAI_BASE_URL) {
}

std::future<AIService::AIResponse> OpenAIService::generateResponse(const std::string& prompt) {
    return std::async(std::launch::async, [this, prompt]() {
        nlohmann::json payload = {
            {"model", MODEL_NAME},
            {"messages", nlohmann::json::array({
                {{"role", "user"}, {"content", prompt}}
            })},
            {"max_tokens", 1000}
        };
        
        auto response = makeRequest("/chat/completions", payload);
        if (!response.success) {
            return response;
        }
        
        try {
            auto json_response = nlohmann::json::parse(response.content);
            std::string content = json_response["choices"][0]["message"]["content"];
            return AIResponse{true, content, ""};
        } catch (const std::exception& e) {
            return AIResponse{false, "", "Failed to parse OpenAI response: " + std::string(e.what())};
        }
    });
}

std::future<AIService::AIResponse> OpenAIService::analyzePreferences(const std::string& user_data) {
    std::string prompt = "Analyze the following user data and extract preferences for event recommendations:\n" + user_data;
    return generateResponse(prompt);
}

std::future<AIService::AIResponse> OpenAIService::recommendEvents(
    const std::string& preferences, 
    const std::string& available_events) {
    
    std::string prompt = "Based on these user preferences:\n" + preferences + 
                        "\n\nRecommend events from this list:\n" + available_events +
                        "\n\nProvide a ranked list with explanations.";
    return generateResponse(prompt);
}