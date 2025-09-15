#include "Event.h"

Event::Event(const std::string& name, const std::string& description, 
             std::chrono::system_clock::time_point start_time,
             std::chrono::system_clock::time_point end_time,
             const std::string& location,
             const std::vector<std::string>& tags)
    : name_(name), description_(description), start_time_(start_time), 
      end_time_(end_time), location_(location), tags_(tags) {
}