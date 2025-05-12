#pragma once
#include <iostream>
#include <vector>
#include <list>
#include <string>
#include <sstream>
#include <filesystem>
#include <exception>
#include <ctime>
#include <cpr/cpr.h>
#include <nlohmann/json.hpp>
#include <ftxui/dom/elements.hpp>
#include <ftxui/component/component.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include <ftxui/screen/screen.hpp>
#include <ftxui/component/event.hpp>
#include <threads.h>

using json = nlohmann::json;

class file_config{
    public:
    file_config();
    ~file_config();
    file_config(const file_config& new_config);

    int update_json(int new_count_days = 3,const std::string& new_town = "\0", int new_ms_update = 180000);
    int read_json();

    std::vector<std::string> get_towns(){return this->towns;}
    int get_count_days(){return this->count_days;}
    int get_ms_update(){return this->ms_update;}

    static const char* url; // base url
    static const char* config_name; // name of config file

    private:
    std::vector<std::string> towns; // i can add many basic towns
    int count_days; // 3-31
    int ms_update; // base 180000
};

class town{
    public:

    town() = default;
    town(std::string town_name);
    ~town();
    town(const town& other);

    void operator=(const town& other);

    std::string get_name() const {return this->name;}
    std::pair<double, double> get_coordinates() const {return this->coordinates;}
    std::string get_country() const {return this->country;}

    static void is_exist(const std::string& town_name);

    private:
    std::string name; // name of a town
    std::pair<double, double> coordinates; // coordinates of a town
    std::string country; // country of a town

    static const char* url; // for towns
    static const char* api_key;
};

class Exception : public std::exception{
    public:
        Exception(const std::string& _msg);
        virtual const char* what() const noexcept override;
    private:
        std::string msg;
};

struct day{
    public:
        // 0 - night 1 - morning 2 - noon 3 - evening
        day(const town& city, int day_offset);
        ~day();
        day(const day& other);

        town city;
        int day_offset;
        std::vector<std::string> weather; // type from weather_type map
        std::vector<std::pair<double, double>> temperatures; // max and min temp per part of a day
        std::string temperatures_unit = "°C";
        std::vector<std::pair<int, int>> wind_speed; // max and min
        std::string wind_speed_unit = "km/h";
        std::vector<std::string> wind_direction;
        std::vector<double> precipitation;
        std::string precipitation_unit = "mm";
        std::vector<int> precipitation_prob;
        std::string precipitation_prob_unit = "%";
        std::string day_of_week;
        
        ftxui::Element create_day_forecast();
        void update_day_forecast();
    private:
        static json get_response_data(const town& city, std::string date);
        static std::pair<std::string, tm*> get_date(int day_offset);
        static std::vector<std::pair<double, double>> get_temperature(json& data);
        static std::vector<std::pair<int, int>> get_wind_speed(json& data);
        static std::vector<std::string> get_weather(json& data, json& response_data);
        static std::vector<std::string> get_wind_direction(json& data);
        static std::vector<double> get_precipitation(json& data);
        static std::vector<int> get_precipitation_prob(json& data);

        ftxui::Element create_element(int index);
        std::string round_double_in_string(std::string number, int count);
};

class forecast{
    public:
        void print_forecast();
    private:
        std::map<std::string, std::vector<day>> forecast;
};