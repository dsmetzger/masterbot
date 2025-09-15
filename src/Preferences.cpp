#include "Preferences.h"

Preferences::Preferences() : max_travel_distance_(10.0) {
}

void Preferences::addInterest(const std::string& interest, int weight) {
    interests_[interest] = weight;
}

void Preferences::removeInterest(const std::string& interest) {
    interests_.erase(interest);
}

void Preferences::setInterestWeight(const std::string& interest, int weight) {
    if (interests_.find(interest) != interests_.end()) {
        interests_[interest] = weight;
    }
}

int Preferences::getInterestWeight(const std::string& interest) const {
    auto it = interests_.find(interest);
    return it != interests_.end() ? it->second : 0;
}

void Preferences::setPreferredTimeSlots(const std::vector<std::pair<int, int>>& time_slots) {
    preferred_time_slots_ = time_slots;
}