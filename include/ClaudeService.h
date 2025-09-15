#pragma once
#include "AIService.h"

class ClaudeService : public AIService {
public:
    explicit ClaudeService(const std::string& api_key);

    std::future<AIResponse> generateResponse(const std::string& prompt) override;
    std::future<AIResponse> analyzePreferences(const std::string& user_data) override;
    std::future<AIResponse> recommendEvents(
        const std::string& preferences, 
        const std::string& available_events) override;

private:
    static const std::string CLAUDE_BASE_URL;
    static const std::string MODEL_NAME;
};