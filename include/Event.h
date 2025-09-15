#pragma once
#include <string>
#include <chrono>
#include <vector>

class Event {
public:
    Event(const std::string& name, const std::string& description, 
          std::chrono::system_clock::time_point start_time,
          std::chrono::system_clock::time_point end_time,
          const std::string& location = "",
          const std::vector<std::string>& tags = {});

    const std::string& getName() const { return name_; }
    const std::string& getDescription() const { return description_; }
    const std::chrono::system_clock::time_point& getStartTime() const { return start_time_; }
    const std::chrono::system_clock::time_point& getEndTime() const { return end_time_; }
    const std::string& getLocation() const { return location_; }
    const std::vector<std::string>& getTags() const { return tags_; }
    
    void setName(const std::string& name) { name_ = name; }
    void setDescription(const std::string& description) { description_ = description; }
    void setStartTime(const std::chrono::system_clock::time_point& time) { start_time_ = time; }
    void setEndTime(const std::chrono::system_clock::time_point& time) { end_time_ = time; }
    void setLocation(const std::string& location) { location_ = location; }
    void addTag(const std::string& tag) { tags_.push_back(tag); }

private:
    std::string name_;
    std::string description_;
    std::chrono::system_clock::time_point start_time_;
    std::chrono::system_clock::time_point end_time_;
    std::string location_;
    std::vector<std::string> tags_;
};