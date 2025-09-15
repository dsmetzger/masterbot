#pragma once
#include <string>
#include <vector>
#include <unordered_map>

class Preferences {
public:
    Preferences();

    void addInterest(const std::string& interest, int weight = 1);
    void removeInterest(const std::string& interest);
    void setInterestWeight(const std::string& interest, int weight);
    int getInterestWeight(const std::string& interest) const;
    
    const std::unordered_map<std::string, int>& getInterests() const { return interests_; }
    
    void setPreferredTimeSlots(const std::vector<std::pair<int, int>>& time_slots);
    const std::vector<std::pair<int, int>>& getPreferredTimeSlots() const { return preferred_time_slots_; }
    
    void setMaxTravelDistance(double distance) { max_travel_distance_ = distance; }
    double getMaxTravelDistance() const { return max_travel_distance_; }
    
    void setLocation(const std::string& location) { user_location_ = location; }
    const std::string& getLocation() const { return user_location_; }

private:
    std::unordered_map<std::string, int> interests_;
    std::vector<std::pair<int, int>> preferred_time_slots_;
    double max_travel_distance_;
    std::string user_location_;
};