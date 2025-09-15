#include "RecommendationEngine.h"
#include <algorithm>
#include <cmath>
#include <sstream>

RecommendationEngine::RecommendationEngine(std::shared_ptr<AIService> ai_service)
    : ai_service_(ai_service) {
}

std::vector<RecommendationEngine::EventRecommendation> RecommendationEngine::recommendEvents(
    const User& user,
    const std::vector<Event>& available_events,
    const Schedule& user_schedule,
    int max_recommendations) {
    
    std::vector<EventRecommendation> recommendations;
    const auto& preferences = user.getPreferences();
    
    for (const auto& event : available_events) {
        if (user_schedule.hasConflict(event)) {
            continue;
        }
        
        double score = calculateEventScore(event, preferences);
        recommendations.push_back({event, score, "Basic compatibility score"});
    }
    
    std::sort(recommendations.begin(), recommendations.end(),
              [](const EventRecommendation& a, const EventRecommendation& b) {
                  return a.score > b.score;
              });
    
    if (recommendations.size() > max_recommendations) {
        recommendations.resize(max_recommendations);
    }
    
    auto ai_response = ai_service_->recommendEvents(
        formatPreferences(preferences),
        formatEventData(available_events)
    );
    
    try {
        auto result = ai_response.get();
        if (result.success) {
            for (auto& rec : recommendations) {
                rec.reasoning = "AI-enhanced reasoning: " + result.content.substr(0, 100);
            }
        }
    } catch (const std::exception& e) {
    }
    
    return recommendations;
}

void RecommendationEngine::updateUserInterests(User& user, const std::vector<Event>& attended_events) {
    auto& preferences = user.getPreferences();
    
    for (const auto& event : attended_events) {
        for (const auto& tag : event.getTags()) {
            int current_weight = preferences.getInterestWeight(tag);
            preferences.setInterestWeight(tag, current_weight + 1);
        }
    }
}

double RecommendationEngine::calculateEventScore(const Event& event, const Preferences& preferences) {
    double interest_score = calculateInterestScore(event, preferences);
    double time_score = calculateTimePreferenceScore(event, preferences);
    double location_score = calculateLocationScore(event, preferences);
    
    return (interest_score * 0.5) + (time_score * 0.3) + (location_score * 0.2);
}

double RecommendationEngine::calculateTimePreferenceScore(const Event& event, const Preferences& preferences) {
    auto start_time = std::chrono::system_clock::to_time_t(event.getStartTime());
    struct tm* tm_info = std::localtime(&start_time);
    int hour = tm_info->tm_hour;
    
    const auto& time_slots = preferences.getPreferredTimeSlots();
    if (time_slots.empty()) {
        return 1.0;
    }
    
    for (const auto& slot : time_slots) {
        if (hour >= slot.first && hour <= slot.second) {
            return 1.0;
        }
    }
    
    return 0.5;
}

double RecommendationEngine::calculateInterestScore(const Event& event, const Preferences& preferences) {
    const auto& interests = preferences.getInterests();
    double total_score = 0.0;
    int matching_tags = 0;
    
    for (const auto& tag : event.getTags()) {
        auto it = interests.find(tag);
        if (it != interests.end()) {
            total_score += it->second;
            matching_tags++;
        }
    }
    
    return matching_tags > 0 ? total_score / matching_tags : 0.0;
}

double RecommendationEngine::calculateLocationScore(const Event& event, const Preferences& preferences) {
    if (event.getLocation().empty() || preferences.getLocation().empty()) {
        return 0.8;
    }
    
    return 1.0;
}

std::string RecommendationEngine::formatEventData(const std::vector<Event>& events) {
    std::ostringstream oss;
    for (const auto& event : events) {
        oss << "Event: " << event.getName() << "\n";
        oss << "Description: " << event.getDescription() << "\n";
        oss << "Location: " << event.getLocation() << "\n";
        oss << "Tags: ";
        for (const auto& tag : event.getTags()) {
            oss << tag << " ";
        }
        oss << "\n\n";
    }
    return oss.str();
}

std::string RecommendationEngine::formatPreferences(const Preferences& preferences) {
    std::ostringstream oss;
    oss << "User Interests:\n";
    for (const auto& interest : preferences.getInterests()) {
        oss << "- " << interest.first << " (weight: " << interest.second << ")\n";
    }
    
    oss << "\nPreferred Time Slots:\n";
    for (const auto& slot : preferences.getPreferredTimeSlots()) {
        oss << "- " << slot.first << ":00 to " << slot.second << ":00\n";
    }
    
    oss << "\nLocation: " << preferences.getLocation() << "\n";
    oss << "Max Travel Distance: " << preferences.getMaxTravelDistance() << " km\n";
    
    return oss.str();
}