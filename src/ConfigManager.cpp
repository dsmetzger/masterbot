#include "ConfigManager.h"
#include <fstream>
#include <filesystem>
#include <iostream>

ConfigManager::ConfigManager(const std::string& config_file_path)
    : config_file_path_(config_file_path) {
    setDefaults();
}

bool ConfigManager::loadConfig() {
    if (!fileExists(config_file_path_)) {
        std::cout << "Config file not found, creating default config at: " << config_file_path_ << std::endl;
        return createDefaultConfig();
    }
    
    try {
        std::ifstream file(config_file_path_);
        if (!file.is_open()) {
            std::cerr << "Failed to open config file: " << config_file_path_ << std::endl;
            return false;
        }
        
        nlohmann::json j;
        file >> j;
        config_ = fromJson(j);
        
        if (!validateConfig()) {
            std::cerr << "Invalid configuration detected" << std::endl;
            auto errors = getValidationErrors();
            for (const auto& error : errors) {
                std::cerr << "  - " << error << std::endl;
            }
            return false;
        }
        
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Failed to load config: " << e.what() << std::endl;
        return false;
    }
}

bool ConfigManager::saveConfig() const {
    try {
        // Create directory if it doesn't exist
        std::filesystem::path config_path(config_file_path_);
        std::filesystem::create_directories(config_path.parent_path());
        
        std::ofstream file(config_file_path_);
        if (!file.is_open()) {
            std::cerr << "Failed to open config file for writing: " << config_file_path_ << std::endl;
            return false;
        }
        
        nlohmann::json j = toJson(config_);
        file << j.dump(2);  // Pretty print with 2-space indentation
        
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Failed to save config: " << e.what() << std::endl;
        return false;
    }
}

bool ConfigManager::createDefaultConfig() const {
    try {
        std::filesystem::path config_path(config_file_path_);
        std::filesystem::create_directories(config_path.parent_path());
        
        // Create default config with placeholder values
        UserConfig default_config = config_;  // Use current defaults
        nlohmann::json j = toJson(default_config);
        
        std::ofstream file(config_file_path_);
        if (!file.is_open()) {
            std::cerr << "Failed to create default config file: " << config_file_path_ << std::endl;
            return false;
        }
        
        file << j.dump(2);
        std::cout << "Created default config file at: " << config_file_path_ << std::endl;
        std::cout << "Please edit this file to set your personal information and API keys." << std::endl;
        
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Failed to create default config: " << e.what() << std::endl;
        return false;
    }
}

void ConfigManager::setUserProfile(const std::string& name, const std::string& email, const std::string& phone) {
    config_.name = name;
    config_.email = email;
    config_.phone_number = phone;
}

void ConfigManager::setLocation(const UserLocation& location) {
    config_.location = location;
}

void ConfigManager::setAIProvider(const std::string& provider, const std::string& api_key) {
    config_.default_ai_provider = provider;
    if (provider == "openai") {
        config_.openai_config.api_key = api_key;
    } else if (provider == "claude") {
        config_.claude_config.api_key = api_key;
    }
}

void ConfigManager::addInterest(const std::string& interest, int weight) {
    config_.interests[interest] = weight;
}

void ConfigManager::removeInterest(const std::string& interest) {
    config_.interests.erase(interest);
}

void ConfigManager::updateNotificationSettings(const NotificationSettings& settings) {
    config_.notifications = settings;
}

bool ConfigManager::validateConfig() const {
    return getValidationErrors().empty();
}

std::vector<std::string> ConfigManager::getValidationErrors() const {
    std::vector<std::string> errors;
    
    if (config_.name.empty()) {
        errors.push_back("Name cannot be empty");
    }
    
    if (config_.email.empty() || config_.email.find('@') == std::string::npos) {
        errors.push_back("Valid email address required");
    }
    
    if (config_.default_ai_provider != "openai" && config_.default_ai_provider != "claude") {
        errors.push_back("Default AI provider must be 'openai' or 'claude'");
    }
    
    if (config_.default_ai_provider == "openai" && config_.openai_config.api_key.empty()) {
        errors.push_back("OpenAI API key required when using OpenAI as default provider");
    }
    
    if (config_.default_ai_provider == "claude" && config_.claude_config.api_key.empty()) {
        errors.push_back("Claude API key required when using Claude as default provider");
    }
    
    if (config_.max_travel_distance_km <= 0) {
        errors.push_back("Max travel distance must be positive");
    }
    
    return errors;
}

UserConfig ConfigManager::fromJson(const nlohmann::json& j) {
    UserConfig config;
    
    // User Profile
    auto user_profile = j["user_profile"];
    config.name = user_profile["name"];
    config.email = user_profile["email"];
    config.phone_number = user_profile["phone_number"];
    config.date_of_birth = user_profile["date_of_birth"];
    config.preferred_language = user_profile["preferred_language"];
    
    // Location
    auto location = user_profile["location"];
    config.location.address = location["address"];
    config.location.city = location["city"];
    config.location.state = location["state"];
    config.location.country = location["country"];
    config.location.timezone = location["timezone"];
    config.location.latitude = location["coordinates"]["latitude"];
    config.location.longitude = location["coordinates"]["longitude"];
    
    // AI Services
    auto ai_services = j["ai_services"];
    config.default_ai_provider = ai_services["default_provider"];
    
    auto openai = ai_services["openai"];
    config.openai_config.api_key = openai["api_key"];
    config.openai_config.model = openai["model"];
    config.openai_config.max_tokens = openai["max_tokens"];
    config.openai_config.temperature = openai["temperature"];
    
    auto claude = ai_services["claude"];
    config.claude_config.api_key = claude["api_key"];
    config.claude_config.model = claude["model"];
    config.claude_config.max_tokens = claude["max_tokens"];
    config.claude_config.temperature = 0.7; // Default for Claude
    
    // Preferences
    auto preferences = j["preferences"];
    config.interests = preferences["interests"];
    config.max_travel_distance_km = preferences["max_travel_distance_km"];
    config.preferred_transportation = preferences["preferred_transportation"];
    config.accessibility_needs = preferences["accessibility_needs"];
    config.dietary_restrictions = preferences["dietary_restrictions"];
    
    // Time slots
    for (const auto& slot : preferences["preferred_time_slots"]) {
        TimeSlot ts;
        ts.start_hour = slot["start_hour"];
        ts.end_hour = slot["end_hour"];
        ts.days = slot["days"];
        config.preferred_time_slots.push_back(ts);
    }
    
    // Budget
    auto budget = preferences["budget_limits"];
    config.budget_limits.daily = budget["daily"];
    config.budget_limits.weekly = budget["weekly"];
    config.budget_limits.monthly = budget["monthly"];
    config.budget_limits.currency = budget["currency"];
    
    // Notifications
    auto notifications = j["notification_settings"];
    config.notifications.email_notifications = notifications["email_notifications"];
    config.notifications.sms_notifications = notifications["sms_notifications"];
    config.notifications.push_notifications = notifications["push_notifications"];
    config.notifications.daily_recommendations_time = notifications["notification_times"]["daily_recommendations"];
    config.notifications.event_reminder_minutes = notifications["notification_times"]["event_reminders"];
    config.notifications.weekly_summary_time = notifications["notification_times"]["weekly_summary"];
    config.notifications.quiet_hours_enabled = notifications["quiet_hours"]["enabled"];
    config.notifications.quiet_hours_start = notifications["quiet_hours"]["start_time"];
    config.notifications.quiet_hours_end = notifications["quiet_hours"]["end_time"];
    
    // Privacy
    auto privacy = j["privacy_settings"];
    config.analytics_sharing = privacy["data_sharing"]["analytics"];
    config.third_party_sharing = privacy["data_sharing"]["third_party"];
    config.marketing_sharing = privacy["data_sharing"]["marketing"];
    config.location_tracking = privacy["location_tracking"];
    config.activity_logging = privacy["activity_logging"];
    config.data_retention_days = privacy["data_retention_days"];
    
    // App Settings
    auto app = j["app_settings"];
    config.theme = app["theme"];
    config.date_format = app["date_format"];
    config.time_format = app["time_format"];
    config.first_day_of_week = app["first_day_of_week"];
    config.auto_sync = app["auto_sync"];
    config.sync_interval_minutes = app["sync_interval_minutes"];
    config.offline_mode = app["offline_mode"];
    config.cache_size_mb = app["cache_size_mb"];
    config.log_level = app["log_level"];
    
    // Advanced Settings
    auto advanced = j["advanced_settings"];
    config.recommendation_algorithm = advanced["recommendation_algorithm"];
    config.learning_rate = advanced["learning_rate"];
    config.diversity_factor = advanced["diversity_factor"];
    config.novelty_boost = advanced["novelty_boost"];
    config.popularity_weight = advanced["popularity_weight"];
    config.recency_bias = advanced["recency_bias"];
    config.max_recommendations_per_day = advanced["max_recommendations_per_day"];
    config.min_recommendation_score = advanced["min_recommendation_score"];
    
    return config;
}

nlohmann::json ConfigManager::toJson(const UserConfig& config) {
    nlohmann::json j;
    
    // User Profile
    j["user_profile"]["name"] = config.name;
    j["user_profile"]["email"] = config.email;
    j["user_profile"]["phone_number"] = config.phone_number;
    j["user_profile"]["date_of_birth"] = config.date_of_birth;
    j["user_profile"]["preferred_language"] = config.preferred_language;
    
    // Location
    j["user_profile"]["location"]["address"] = config.location.address;
    j["user_profile"]["location"]["city"] = config.location.city;
    j["user_profile"]["location"]["state"] = config.location.state;
    j["user_profile"]["location"]["country"] = config.location.country;
    j["user_profile"]["location"]["timezone"] = config.location.timezone;
    j["user_profile"]["location"]["coordinates"]["latitude"] = config.location.latitude;
    j["user_profile"]["location"]["coordinates"]["longitude"] = config.location.longitude;
    
    // AI Services
    j["ai_services"]["default_provider"] = config.default_ai_provider;
    j["ai_services"]["openai"]["api_key"] = config.openai_config.api_key;
    j["ai_services"]["openai"]["model"] = config.openai_config.model;
    j["ai_services"]["openai"]["max_tokens"] = config.openai_config.max_tokens;
    j["ai_services"]["openai"]["temperature"] = config.openai_config.temperature;
    j["ai_services"]["claude"]["api_key"] = config.claude_config.api_key;
    j["ai_services"]["claude"]["model"] = config.claude_config.model;
    j["ai_services"]["claude"]["max_tokens"] = config.claude_config.max_tokens;
    
    // Preferences
    j["preferences"]["interests"] = config.interests;
    j["preferences"]["max_travel_distance_km"] = config.max_travel_distance_km;
    j["preferences"]["preferred_transportation"] = config.preferred_transportation;
    j["preferences"]["accessibility_needs"] = config.accessibility_needs;
    j["preferences"]["dietary_restrictions"] = config.dietary_restrictions;
    
    // Time slots
    j["preferences"]["preferred_time_slots"] = nlohmann::json::array();
    for (const auto& slot : config.preferred_time_slots) {
        nlohmann::json slot_json;
        slot_json["start_hour"] = slot.start_hour;
        slot_json["end_hour"] = slot.end_hour;
        slot_json["days"] = slot.days;
        j["preferences"]["preferred_time_slots"].push_back(slot_json);
    }
    
    // Budget
    j["preferences"]["budget_limits"]["daily"] = config.budget_limits.daily;
    j["preferences"]["budget_limits"]["weekly"] = config.budget_limits.weekly;
    j["preferences"]["budget_limits"]["monthly"] = config.budget_limits.monthly;
    j["preferences"]["budget_limits"]["currency"] = config.budget_limits.currency;
    
    // Notifications
    j["notification_settings"]["email_notifications"] = config.notifications.email_notifications;
    j["notification_settings"]["sms_notifications"] = config.notifications.sms_notifications;
    j["notification_settings"]["push_notifications"] = config.notifications.push_notifications;
    j["notification_settings"]["notification_times"]["daily_recommendations"] = config.notifications.daily_recommendations_time;
    j["notification_settings"]["notification_times"]["event_reminders"] = config.notifications.event_reminder_minutes;
    j["notification_settings"]["notification_times"]["weekly_summary"] = config.notifications.weekly_summary_time;
    j["notification_settings"]["quiet_hours"]["enabled"] = config.notifications.quiet_hours_enabled;
    j["notification_settings"]["quiet_hours"]["start_time"] = config.notifications.quiet_hours_start;
    j["notification_settings"]["quiet_hours"]["end_time"] = config.notifications.quiet_hours_end;
    
    // Privacy
    j["privacy_settings"]["data_sharing"]["analytics"] = config.analytics_sharing;
    j["privacy_settings"]["data_sharing"]["third_party"] = config.third_party_sharing;
    j["privacy_settings"]["data_sharing"]["marketing"] = config.marketing_sharing;
    j["privacy_settings"]["location_tracking"] = config.location_tracking;
    j["privacy_settings"]["activity_logging"] = config.activity_logging;
    j["privacy_settings"]["data_retention_days"] = config.data_retention_days;
    
    // App Settings
    j["app_settings"]["theme"] = config.theme;
    j["app_settings"]["date_format"] = config.date_format;
    j["app_settings"]["time_format"] = config.time_format;
    j["app_settings"]["first_day_of_week"] = config.first_day_of_week;
    j["app_settings"]["auto_sync"] = config.auto_sync;
    j["app_settings"]["sync_interval_minutes"] = config.sync_interval_minutes;
    j["app_settings"]["offline_mode"] = config.offline_mode;
    j["app_settings"]["cache_size_mb"] = config.cache_size_mb;
    j["app_settings"]["log_level"] = config.log_level;
    
    // Advanced Settings
    j["advanced_settings"]["recommendation_algorithm"] = config.recommendation_algorithm;
    j["advanced_settings"]["learning_rate"] = config.learning_rate;
    j["advanced_settings"]["diversity_factor"] = config.diversity_factor;
    j["advanced_settings"]["novelty_boost"] = config.novelty_boost;
    j["advanced_settings"]["popularity_weight"] = config.popularity_weight;
    j["advanced_settings"]["recency_bias"] = config.recency_bias;
    j["advanced_settings"]["max_recommendations_per_day"] = config.max_recommendations_per_day;
    j["advanced_settings"]["min_recommendation_score"] = config.min_recommendation_score;
    
    return j;
}

void ConfigManager::setDefaults() {
    config_.name = "User";
    config_.email = "user@example.com";
    config_.phone_number = "";
    config_.date_of_birth = "";
    config_.preferred_language = "en-US";
    
    // Default location
    config_.location.address = "";
    config_.location.city = "";
    config_.location.state = "";
    config_.location.country = "";
    config_.location.timezone = "UTC";
    config_.location.latitude = 0.0;
    config_.location.longitude = 0.0;
    
    // Default AI settings
    config_.default_ai_provider = "openai";
    config_.openai_config = {"", "gpt-3.5-turbo", 1000, 0.7};
    config_.claude_config = {"", "claude-3-sonnet-20240229", 1000, 0.7};
    
    // Default preferences
    config_.max_travel_distance_km = 25.0;
    config_.preferred_transportation = {"walking", "public_transport"};
    
    // Default budget
    config_.budget_limits = {100.0, 500.0, 2000.0, "USD"};
    
    // Default notifications
    config_.notifications.email_notifications = true;
    config_.notifications.sms_notifications = false;
    config_.notifications.push_notifications = true;
    config_.notifications.daily_recommendations_time = "08:00";
    config_.notifications.event_reminder_minutes = {60, 15};
    config_.notifications.weekly_summary_time = "sunday:10:00";
    config_.notifications.quiet_hours_enabled = true;
    config_.notifications.quiet_hours_start = "22:00";
    config_.notifications.quiet_hours_end = "07:00";
    
    // Default privacy
    config_.analytics_sharing = true;
    config_.third_party_sharing = false;
    config_.marketing_sharing = false;
    config_.location_tracking = true;
    config_.activity_logging = true;
    config_.data_retention_days = 365;
    
    // Default app settings
    config_.theme = "auto";
    config_.date_format = "MM/DD/YYYY";
    config_.time_format = "12h";
    config_.first_day_of_week = "sunday";
    config_.auto_sync = true;
    config_.sync_interval_minutes = 30;
    config_.offline_mode = false;
    config_.cache_size_mb = 100;
    config_.log_level = "info";
    
    // Default advanced settings
    config_.recommendation_algorithm = "hybrid";
    config_.learning_rate = 0.1;
    config_.diversity_factor = 0.3;
    config_.novelty_boost = 0.2;
    config_.popularity_weight = 0.1;
    config_.recency_bias = 0.8;
    config_.max_recommendations_per_day = 10;
    config_.min_recommendation_score = 0.6;
}

bool ConfigManager::fileExists(const std::string& path) const {
    return std::filesystem::exists(path);
}