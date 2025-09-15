#pragma once
#include "Preferences.h"
#include <string>

class User {
public:
    User(const std::string& name, const std::string& email);

    const std::string& getName() const { return name_; }
    const std::string& getEmail() const { return email_; }
    
    void setName(const std::string& name) { name_ = name; }
    void setEmail(const std::string& email) { email_ = email; }
    
    Preferences& getPreferences() { return preferences_; }
    const Preferences& getPreferences() const { return preferences_; }

private:
    std::string name_;
    std::string email_;
    Preferences preferences_;
};