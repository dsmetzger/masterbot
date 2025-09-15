#pragma once
#include "User.h"
#include "Event.h"
#include "Schedule.h"
#include "AIService.h"
#include <memory>
#include <vector>

class RecommendationEngine {
public:
    struct EventRecommendation {
        Event event;
        double score;
        std::string reasoning;
    };

    explicit RecommendationEngine(std::shared_ptr<AIService> ai_service);

    std::vector<EventRecommendation> recommendEvents(
        const User& user,
        const std::vector<Event>& available_events,
        const Schedule& user_schedule,
        int max_recommendations = 10
    );

    void updateUserInterests(User& user, const std::vector<Event>& attended_events);
    
    double calculateEventScore(const Event& event, const Preferences& preferences);

private:
    std::shared_ptr<AIService> ai_service_;
    
    double calculateTimePreferenceScore(const Event& event, const Preferences& preferences);
    double calculateInterestScore(const Event& event, const Preferences& preferences);
    double calculateLocationScore(const Event& event, const Preferences& preferences);
    
    std::string formatEventData(const std::vector<Event>& events);
    std::string formatPreferences(const Preferences& preferences);
};