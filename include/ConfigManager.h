#pragma once
#include <string>
#include <vector>
#include <nlohmann/json.hpp>

struct UserLocation {
    std::string address;
    std::string city;
    std::string state;
    std::string country;
    std::string timezone;
    double latitude;
    double longitude;
};

struct TimeSlot {
    int start_hour;
    int end_hour;
    std::vector<std::string> days;
};

struct BudgetLimits {
    double daily;
    double weekly;
    double monthly;
    std::string currency;
};

struct NotificationSettings {
    bool email_notifications;
    bool sms_notifications;
    bool push_notifications;
    std::string daily_recommendations_time;
    std::vector<int> event_reminder_minutes;
    std::string weekly_summary_time;
    bool quiet_hours_enabled;
    std::string quiet_hours_start;
    std::string quiet_hours_end;
};

struct AIServiceConfig {
    std::string api_key;
    std::string model;
    int max_tokens;
    double temperature;
};

struct UserConfig {
    // User Profile
    std::string name;
    std::string email;
    std::string phone_number;
    UserLocation location;
    std::string date_of_birth;
    std::string preferred_language;
    
    // AI Services
    std::string default_ai_provider;
    AIServiceConfig openai_config;
    AIServiceConfig claude_config;
    
    // Preferences
    std::map<std::string, int> interests;
    std::vector<TimeSlot> preferred_time_slots;
    double max_travel_distance_km;
    std::vector<std::string> preferred_transportation;
    BudgetLimits budget_limits;
    std::vector<std::string> accessibility_needs;
    std::vector<std::string> dietary_restrictions;
    
    // Notifications
    NotificationSettings notifications;
    
    // Privacy
    bool analytics_sharing;
    bool third_party_sharing;
    bool marketing_sharing;
    bool location_tracking;
    bool activity_logging;
    int data_retention_days;
    
    // App Settings
    std::string theme;
    std::string date_format;
    std::string time_format;
    std::string first_day_of_week;
    bool auto_sync;
    int sync_interval_minutes;
    bool offline_mode;
    int cache_size_mb;
    std::string log_level;
    
    // Advanced Settings
    std::string recommendation_algorithm;
    double learning_rate;
    double diversity_factor;
    double novelty_boost;
    double popularity_weight;
    double recency_bias;
    int max_recommendations_per_day;
    double min_recommendation_score;
};

class ConfigManager {
public:
    ConfigManager(const std::string& config_file_path = "config/user_config.json");
    
    bool loadConfig();
    bool saveConfig() const;
    bool createDefaultConfig() const;
    
    UserConfig& getConfig() { return config_; }
    const UserConfig& getConfig() const { return config_; }
    
    // Convenience methods for common operations
    void setUserProfile(const std::string& name, const std::string& email, const std::string& phone);
    void setLocation(const UserLocation& location);
    void setAIProvider(const std::string& provider, const std::string& api_key);
    void addInterest(const std::string& interest, int weight);
    void removeInterest(const std::string& interest);
    void updateNotificationSettings(const NotificationSettings& settings);
    
    // Validation methods
    bool validateConfig() const;
    std::vector<std::string> getValidationErrors() const;
    
    // JSON conversion helpers
    static UserConfig fromJson(const nlohmann::json& j);
    static nlohmann::json toJson(const UserConfig& config);

private:
    std::string config_file_path_;
    UserConfig config_;
    
    void setDefaults();
    bool fileExists(const std::string& path) const;
};