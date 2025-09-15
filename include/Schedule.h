#pragma once
#include "Event.h"
#include <vector>
#include <chrono>

class Schedule {
public:
    Schedule();

    void addEvent(const Event& event);
    void removeEvent(const std::string& event_name);
    
    const std::vector<Event>& getEvents() const { return events_; }
    
    std::vector<Event> getEventsInRange(
        const std::chrono::system_clock::time_point& start,
        const std::chrono::system_clock::time_point& end) const;
        
    bool hasConflict(const Event& event) const;
    std::vector<std::pair<std::chrono::system_clock::time_point, std::chrono::system_clock::time_point>> 
        getFreeTimeSlots(
            const std::chrono::system_clock::time_point& start,
            const std::chrono::system_clock::time_point& end) const;

private:
    std::vector<Event> events_;
};