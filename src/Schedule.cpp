#include "Schedule.h"
#include <algorithm>

Schedule::Schedule() {
}

void Schedule::addEvent(const Event& event) {
    events_.push_back(event);
    std::sort(events_.begin(), events_.end(), 
              [](const Event& a, const Event& b) {
                  return a.getStartTime() < b.getStartTime();
              });
}

void Schedule::removeEvent(const std::string& event_name) {
    events_.erase(
        std::remove_if(events_.begin(), events_.end(),
                      [&event_name](const Event& e) {
                          return e.getName() == event_name;
                      }),
        events_.end());
}

std::vector<Event> Schedule::getEventsInRange(
    const std::chrono::system_clock::time_point& start,
    const std::chrono::system_clock::time_point& end) const {
    
    std::vector<Event> result;
    for (const auto& event : events_) {
        if (event.getStartTime() >= start && event.getEndTime() <= end) {
            result.push_back(event);
        }
    }
    return result;
}

bool Schedule::hasConflict(const Event& event) const {
    for (const auto& existing_event : events_) {
        if (event.getStartTime() < existing_event.getEndTime() &&
            event.getEndTime() > existing_event.getStartTime()) {
            return true;
        }
    }
    return false;
}

std::vector<std::pair<std::chrono::system_clock::time_point, std::chrono::system_clock::time_point>> 
Schedule::getFreeTimeSlots(
    const std::chrono::system_clock::time_point& start,
    const std::chrono::system_clock::time_point& end) const {
    
    std::vector<std::pair<std::chrono::system_clock::time_point, std::chrono::system_clock::time_point>> free_slots;
    
    auto current_time = start;
    for (const auto& event : events_) {
        if (event.getStartTime() >= end) break;
        if (event.getEndTime() <= start) continue;
        
        if (current_time < event.getStartTime()) {
            free_slots.push_back({current_time, event.getStartTime()});
        }
        current_time = std::max(current_time, event.getEndTime());
    }
    
    if (current_time < end) {
        free_slots.push_back({current_time, end});
    }
    
    return free_slots;
}