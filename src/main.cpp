#include "User.h"
#include "Event.h"
#include "Schedule.h"
#include "RecommendationEngine.h"
#include "OpenAIService.h"
#include "ClaudeService.h"
#include "ConfigManager.h"
#include <iostream>
#include <memory>
#include <chrono>

void printRecommendations(const std::vector<RecommendationEngine::EventRecommendation>& recommendations) {
    std::cout << "\n=== Event Recommendations ===\n";
    for (size_t i = 0; i < recommendations.size(); ++i) {
        const auto& rec = recommendations[i];
        std::cout << (i + 1) << ". " << rec.event.getName() << " (Score: " << rec.score << ")\n";
        std::cout << "   Description: " << rec.event.getDescription() << "\n";
        std::cout << "   Location: " << rec.event.getLocation() << "\n";
        std::cout << "   Reasoning: " << rec.reasoning << "\n\n";
    }
}

void setupSampleData(User& user, std::vector<Event>& events, const UserConfig& config) {
    // Load preferences from config
    auto& preferences = user.getPreferences();
    for (const auto& interest : config.interests) {
        preferences.addInterest(interest.first, interest.second);
    }
    
    // Convert time slots from config
    std::vector<std::pair<int, int>> time_slots;
    for (const auto& slot : config.preferred_time_slots) {
        time_slots.push_back({slot.start_hour, slot.end_hour});
    }
    preferences.setPreferredTimeSlots(time_slots);
    preferences.setLocation(config.location.city);
    preferences.setMaxTravelDistance(config.max_travel_distance_km);
    
    auto now = std::chrono::system_clock::now();
    auto tomorrow = now + std::chrono::hours(24);
    auto day_after = now + std::chrono::hours(48);
    
    events.push_back(Event("Tech Conference 2024", "Annual technology conference", 
                          tomorrow, tomorrow + std::chrono::hours(8), 
                          "San Francisco Convention Center", {"technology", "networking"}));
    
    events.push_back(Event("Jazz Night", "Live jazz music performance", 
                          tomorrow + std::chrono::hours(19), tomorrow + std::chrono::hours(22), 
                          "Blue Note SF", {"music", "entertainment"}));
    
    events.push_back(Event("Basketball Game", "Local team championship", 
                          day_after + std::chrono::hours(15), day_after + std::chrono::hours(18), 
                          "Oracle Arena", {"sports", "entertainment"}));
    
    events.push_back(Event("Cooking Workshop", "Learn Italian cuisine", 
                          day_after + std::chrono::hours(11), day_after + std::chrono::hours(14), 
                          "Culinary Institute", {"cooking", "education"}));
}

int main() {
    std::cout << "=== MasterBot Schedule Manager ===\n";
    
    ConfigManager config_manager;
    
    if (!config_manager.loadConfig()) {
        std::cerr << "Failed to load configuration. Please check your config file.\n";
        return 1;
    }
    
    const auto& config = config_manager.getConfig();
    
    std::cout << "Welcome, " << config.name << "!\n";
    std::cout << "Location: " << config.location.city << ", " << config.location.state << "\n";
    std::cout << "Using AI provider: " << config.default_ai_provider << "\n\n";
    
    std::shared_ptr<AIService> ai_service;
    if (config.default_ai_provider == "openai") {
        if (config.openai_config.api_key.empty()) {
            std::cout << "OpenAI API key not configured. Enter API key: ";
            std::string api_key;
            std::cin >> api_key;
            ai_service = std::make_shared<OpenAIService>(api_key);
        } else {
            ai_service = std::make_shared<OpenAIService>(config.openai_config.api_key);
        }
        std::cout << "Using OpenAI service\n";
    } else {
        if (config.claude_config.api_key.empty()) {
            std::cout << "Claude API key not configured. Enter API key: ";
            std::string api_key;
            std::cin >> api_key;
            ai_service = std::make_shared<ClaudeService>(api_key);
        } else {
            ai_service = std::make_shared<ClaudeService>(config.claude_config.api_key);
        }
        std::cout << "Using Claude service\n";
    }
    
    User user(config.name, config.email);
    Schedule schedule;
    std::vector<Event> available_events;
    
    setupSampleData(user, available_events, config);
    
    std::cout << "\nSample events loaded:\n";
    for (const auto& event : available_events) {
        std::cout << "- " << event.getName() << " at " << event.getLocation() << "\n";
    }
    
    RecommendationEngine engine(ai_service);
    
    std::cout << "\nGenerating recommendations...\n";
    auto recommendations = engine.recommendEvents(user, available_events, schedule, 5);
    
    printRecommendations(recommendations);
    
    std::cout << "\nWould you like to add any events to your schedule? (y/n): ";
    char add_choice;
    std::cin >> add_choice;
    
    if (add_choice == 'y' || add_choice == 'Y') {
        std::cout << "Enter event number to add (1-" << recommendations.size() << "): ";
        int event_num;
        std::cin >> event_num;
        
        if (event_num >= 1 && event_num <= static_cast<int>(recommendations.size())) {
            schedule.addEvent(recommendations[event_num - 1].event);
            std::cout << "Event added to your schedule!\n";
            
            std::vector<Event> attended = {recommendations[event_num - 1].event};
            engine.updateUserInterests(user, attended);
            std::cout << "User preferences updated based on selection.\n";
        }
    }
    
    std::cout << "\nThank you for using MasterBot!\n";
    return 0;
}