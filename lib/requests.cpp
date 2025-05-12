#include "requests.h"

const char* town::url = "https://api.api-ninjas.com/v1/city";
const char* file_config::url = "https://api.open-meteo.com/v1/forecast";
const char* town::api_key = "0eWHdDwOQhj1T86CplL3pQ==fUMXIhsejM1pMRku";
const char* file_config::config_name = "/home/user/Desktop/repos/labwork7-bemooooooooo/build/bin/config.json";

file_config::file_config(): towns{std::vector<std::string>{"Moscow", "St. Petersburg", "Kaliningrad", "Omsk", "Ufa"}}, count_days{3}, ms_update{180000} {
    if(!std::filesystem::exists(file_config::config_name)){
        std::ofstream out(file_config::config_name);
        json data = {
            {"towns", this->towns}, // mb can't work, do tests
            {"count_days", this->count_days},
            {"ms_update", this->ms_update},
            {"weather_type", {
                             {0, "Clear sky"} ,{1, "Mainly clear"} ,{2, "Partly cloudy"} ,{3, "Overcast"}
                            ,{45, "Fog"} ,{48  , "Depositing rime fog"} ,{51, "Light drizzle"} ,{53, "Moderate drizzle"}
                            ,{55, "Dense intensity drizzle"} ,{56, "Light freezing drizzle"} ,{57, "Dense intensity freezing drizzle"} ,{61, "Slight rain"} 
                            ,{63, "Moderate rain"} ,{65, "Heavy intensity rain"} ,{66, "Light freezing rain"} ,{67, "Heavy intensity freezing rain"} 
                            ,{71, "Slight snow fall"} ,{73, "Moderate snow fall"} ,{75, "Heavy intensity snow fall"} ,{77, "Snow grains"} 
                            ,{80, "Slight rain showers"} ,{81, "Moderate rain showers"} ,{82, "Violent rain showers"} ,{85, "Snow showers slight"}
                            ,{86, "Snow showers heavy"} ,{95, "Sligth thunderstorm"} ,{96, "Moderate thunderstorm"} ,{99, "Thunderstorm with slight and heavy hail"}
                             }
            }};
        out<<data;
        out.close();
    }
}
file_config::~file_config(){}
file_config::file_config(const file_config& new_config):towns{new_config.towns}, count_days{new_config.count_days}, ms_update{new_config.ms_update} {}

int file_config::update_json(int new_count_days,const std::string& new_town, int new_ms_update){
    if(new_count_days!=this->count_days){
        if(new_count_days < 3 || new_count_days > 31){
            return -1;
        }
        this->count_days = new_count_days;
    }
    if(new_town!="\0"){
        try{
            town::is_exist(new_town);
        }
        catch(Exception& error){
            return -1;
        }
        this->towns.push_back(new_town);
    }
    if(new_ms_update!=this->ms_update){
        if (new_ms_update<1000){
            return -1;
        }
        this->ms_update = new_ms_update;
    }
    json data = {
            {"towns", this->towns}, // mb can't work, do tests
            {"count_days", this->count_days},
            {"ms_update", this->ms_update},
            {"weather_type", {
                             {0, "Clear sky"} ,{1, "Mainly clear"} ,{2, "Partly cloudy"} ,{3, "Overcast"}
                            ,{45, "Fog"} ,{48  , "Depositing rime fog"} ,{51, "Light drizzle"} ,{53, "Moderate drizzle"}
                            ,{55, "Dense intensity drizzle"} ,{56, "Light freezing drizzle"} ,{57, "Dense intensity freezing drizzle"} ,{61, "Slight rain"} 
                            ,{63, "Moderate rain"} ,{65, "Heavy intensity rain"} ,{66, "Light freezing rain"} ,{67, "Heavy intensity freezing rain"} 
                            ,{71, "Slight snow fall"} ,{73, "Moderate snow fall"} ,{75, "Heavy intensity snow fall"} ,{77, "Snow grains"} 
                            ,{80, "Slight rain showers"} ,{81, "Moderate rain showers"} ,{82, "Violent rain showers"} ,{85, "Snow showers slight"}
                            ,{86, "Snow showers heavy"} ,{95, "Sligth thunderstorm"} ,{96, "Moderate thunderstorm"} ,{99, "Thunderstorm with slight and heavy hail"}
                             }
            }};
    std::ofstream out (config_name, std::ios::trunc);
    out<<data;
    out.close();
    return 0;
}

int file_config::read_json(){
    std::ifstream in(config_name);
    if (in.is_open()) {
        return -1;
    }
    json data;
    in>>data;
    this->towns = data["towns"].template get<std::vector<std::string>>();
    this->count_days = data["count_days"].template get<int>();
    this->ms_update = data["ms_update"].template get<int>();
    return 0;
}

town::town(std::string town_name){
    cpr::Response response = cpr::Get(cpr::Url{town::url},cpr::Header{{"X-Api-Key", town::api_key}}, cpr::Parameters{{"name", town_name}});
    int status_code = response.status_code;
    if (status_code == 0 || status_code >= 400) {
        throw Exception("Error [" + status_code + ']');
    } else if (response.text == "[]"){
        throw Exception("City is not found");
    }
    json data = json::parse(response.text.substr(1, response.text.size() - 2));
    this->name = data["name"];
    this->coordinates = std::pair(data["latitude"], data["longitude"]);
    this->country = data["country"];
}
town::~town(){}
town::town(const town& other):name{other.name}, coordinates{other.coordinates}, country{other.country}{}

void town::operator=(const town& other){
    name = other.name;
    coordinates = other.coordinates;
    country = other.country;
}

void town::is_exist(const std::string& town_name){
    cpr::Response response = cpr::Get(cpr::Url{town::url}, cpr::Header{{"X-Api-Key", town::api_key}}, cpr::Parameters{{"name", town_name}});
    int status_code = response.status_code;
    if (status_code == 0 || status_code >= 400) {
        throw Exception("Error [" + status_code + ']');
    } else if (response.text == "[]"){
        throw Exception("City is not found");
    }
}

Exception::Exception(const std::string& _msg):msg{_msg}{}
const char* Exception::what() const noexcept{return msg.c_str();}

day::day(const town& city, int day_offset){
    std::ifstream in(file_config::config_name);
    json data = json::parse(in);
    std::map<int, std::string> wday {
        {0,"Sunday"},
        {1,"Monday"},
        {2,"Tuesday"},
        {3,"Wednesday"},
        {4,"Thursday"},
        {5,"Friday"},
        {6,"Saturday"}
    };
    std::map<int, std::string> name_mon{
        {0,"Jan"},
        {2,"Feb"},
        {3,"March"},
        {4,"April"},
        {5,"May"},
        {6,"June"},
        {7,"July"},
        {8,"Aug"},
        {9,"Sep"},
        {10,"Nov"},
        {11,"Dec"}
    };
    std::pair<std::string, tm*> from_get_date = get_date(day_offset);
    std::string date = from_get_date.first;
    json response_data = get_response_data(city, date);
    this->city = city;
    this->day_offset = day_offset;
    this->weather = get_weather(data, response_data);
    this->temperatures = get_temperature(response_data); // max and min
    this->wind_speed = get_wind_speed(response_data);
    this->wind_direction = get_wind_direction(response_data);
    this->precipitation = get_precipitation(response_data);
    this->precipitation_prob = get_precipitation_prob(response_data);
    this->day_of_week = wday[from_get_date.second->tm_wday] + " " + 
                        std::to_string(from_get_date.second->tm_mday) + " " + 
                        name_mon[from_get_date.second->tm_mon];
}
day::~day(){}
day::day(const day& other):
    city{other.city},day_offset{other.day_offset},weather{other.weather}, temperatures{other.temperatures}, wind_speed{other.wind_speed}, 
    wind_direction{other.wind_direction}, precipitation{other.precipitation}, 
    precipitation_prob{other.precipitation_prob}{}

json day::get_response_data(const town& city, std::string date){
    std::ifstream in(file_config::config_name);
    json data = json::parse(in);
    cpr::Response response = cpr::Get(cpr::Url{file_config::url}, 
    cpr::Parameters{
         {"latitude", std::to_string(city.get_coordinates().first)}
        ,{"longitude", std::to_string(city.get_coordinates().second)}
        ,{"timezone", "auto"}
        ,{"start_date", date}
        ,{"end_date", date}
        ,{"hourly", "temperature_2m"}
        ,{"hourly", "wind_speed_10m"}
        ,{"hourly", "wind_direction_10m"}
        ,{"hourly", "precipitation"}
        ,{"hourly", "precipitation_probability"}
        ,{"hourly", "weather_code"}
        }
    );
    json response_data = json::parse(response.text);
    return response_data;
}

std::pair<std::string, tm*> day::get_date(int day_offset){
    time_t ttime = time(0);
    tm* local_time = std::localtime(&ttime);
    local_time->tm_mday += day_offset;
    ttime = std::mktime(local_time);
    local_time = std::localtime(&ttime);
    std::string date = std::to_string(1900+local_time->tm_year)+ "-"
                        + (1 + local_time->tm_mon < 10 ?
                        "0" + std::to_string(1+local_time->tm_mon):
                        std::to_string(1 + local_time->tm_mon))+ "-"
                        +(local_time->tm_mday < 10 ?
                        "0" + std::to_string(local_time->tm_mday):
                        std::to_string(local_time->tm_mday));
    return std::pair<std::string, tm*>(date, local_time);
}

std::vector<std::pair<double, double>> day::get_temperature(json& data){
    std::vector<std::pair<double, double>> result;
    for (int i = 0; i < 24; i+=6) {
        double min = INFINITY;
        double max = -INFINITY;
        for (int j = i; j < i+7 && j < 24; j++) {
            double number = round(data["hourly"]["temperature_2m"][j].get<double>()*100)/100;
            if (min > number) {
                min = number;
            }
            if (max < number) {
                max = number;
            }
        }
        result.push_back(std::pair<double, double>(max, min));
    }
    return result;
}

std::vector<std::pair<int, int>> day::get_wind_speed(json& data){
    std::vector<std::pair<int, int>> result;
    for (int i = 0; i < 24; i+=6)
    {
        int min = INT32_MAX;
        int max = INT32_MIN;
        for (int j = i; i < i+7 && j < 24; j++) {
            int number = data["hourly"]["wind_speed_10m"][j]; 
            if (min > number) {
                min = number;
            }
            if (max < number) {
                max = number;
            }
        }
        result.push_back(std::pair<int, int>(max, min));
    }
    return result;
}

std::vector<std::string> day::get_weather(json& data, json& response_data){
    std::vector<std::string> result;
    for (int i = 0; i < 24; i+=6){
        std::map<int, int> count_weather_code;
        for (int j = i; j < i+7 && j < 24; j++) {
            int number = response_data["hourly"]["weather_code"][j];
            if (!count_weather_code.contains(number)) {
                count_weather_code.insert({number, 1});
            } else {
                count_weather_code[number]++;
            }
        }
        int key_max_entrance;
        int value_max_entrance = 0;
        for (auto& [key, value]: count_weather_code) {
            if (value_max_entrance<value) {
                key_max_entrance = key;
                value_max_entrance = value;
            }
        }
        result.push_back(data["weather_type"][std::to_string(key_max_entrance)].template get<std::string>());
    }
    return result;
}

std::vector<std::string> day::get_wind_direction(json& data){
    std::map<std::pair<double, double>, std::string> convert_to_direction 
    {
        {{337.5, 22.5}, "↑"},
        {{22.5, 67.5}, "↗"},
        {{67.5, 112.5}, "→"},
        {{112.5, 157.5}, "↘"},
        {{157.5, 202.5}, "↓"},
        {{202.5, 247.5}, "↙"},
        {{247.5, 292.5}, "←"},
        {{292.5, 337.5},"↖"}
    };
    std::vector<std::string> result;
    for (int i = 0; i < 24; i+=6)
    {
        std::map<std::string, int> count_wind_direction;
        for (int j = i; j < i+7 && j < 24; j++){
            for (auto& [key, value]: convert_to_direction) {
                if (key.first < data["hourly"]["wind_direction_10m"][j] && data["hourly"]["wind_direction_10m"][j] < key.second) {
                    if (count_wind_direction.contains(value)) {
                        count_wind_direction.insert({value, 1});
                    } else {
                        count_wind_direction[value]++;
                    }
                }   
            }
        }
        std::string key_max_entrance;
        int value_max_entrance = 0;
        for (auto& [key, value]: count_wind_direction) {
            if (value_max_entrance<value) {
                key_max_entrance = key;
                value_max_entrance = value;
            }
        }
        result.push_back(key_max_entrance);
    }
    return result;
}

std::vector<double> day::get_precipitation(json& data){
    std::vector<double> result;
    for (int i = 0; i < 24; i+=6) {
        double summ = 0;
        int cnt = 0;
        for (int j = i; j< i+7 && j < 24; j++) {
            ++cnt;
            summ = summ + round(data["hourly"]["precipitation"][j].get<double>()*100)/100;
        }
        result.push_back(round(summ/cnt*100)/100);
    }
    return result;
}

std::vector<int> day::get_precipitation_prob(json& data){
    std::vector<int> result;
    for (int i = 0; i < 24; i+=6) {
        int summ = 0;
        int cnt = 0;
        for (int j = i; j< i+7 && j < 24; j++) {
            ++cnt;
            summ = summ + data["hourly"]["precipitation_probability"][j].get<int>();
        }
        result.push_back(round(summ/cnt*100)/100);
    }
    return result;
}

ftxui::Element day::create_element(int index){
    std::ifstream in(file_config::config_name);
    json data = json::parse(in);

    std::map<int, std::string> day_part
    {
        {0,"Night"},
        {1,"Morning"},
        {2,"Noon"},
        {3,"Evening"}
    };
    ftxui::Elements weather_design_vbox;
    for (auto& i: data["weather_design"][weather[index]].get<std::vector<std::string>>()) {
        weather_design_vbox.push_back(ftxui::text(i)|ftxui::center);
    }
    ftxui::Element new_element = ftxui::window
    (
        ftxui::text(day_part[index]) | ftxui::center,
        ftxui::hbox(
            ftxui::vbox(weather_design_vbox) | ftxui::flex,
            ftxui::vbox(
                ftxui::text(weather[index]) | ftxui::center,
                ftxui::text(
                    round_double_in_string(std::to_string(temperatures[index].first), 1)+"("+
                    round_double_in_string(std::to_string(temperatures[index].second), 1)+") "+
                    temperatures_unit
                    ) | ftxui::center,
                ftxui::text(
                    wind_direction[index] + " " +
                    round_double_in_string(std::to_string(wind_speed[index].second), 1)+"-"+
                    round_double_in_string(std::to_string(wind_speed[index].first), 1)+" "+
                    wind_speed_unit
                    ) | ftxui::center,
                ftxui::text(
                    round_double_in_string(std::to_string(precipitation[index]), 1) + " " +
                    precipitation_unit + " | " +
                    std::to_string(precipitation_prob[index]) + " " +
                    precipitation_prob_unit
                    ) | ftxui::center
                ) | ftxui::flex
            )
    );
    return new_element; 
}

std::string day::round_double_in_string(std::string number, int count){
    std::string result;
    for (int i = 0; i < number.size(); i++){
        if (number[i]!='.') {
            result.push_back(number[i]);
        } else {
            for (int j = i; j < i+count+1; j++) {
                result.push_back(number[j]);
            }
            break;
        }
    }
    return result;
}

ftxui::Element day::create_day_forecast(){
    ftxui::Element window_content = ftxui::hbox
    (
        create_element(0) | ftxui::flex,
        create_element(1) | ftxui::flex,
        create_element(2) | ftxui::flex,
        create_element(3) | ftxui::flex
    );
    return ftxui::window(ftxui::text(day_of_week), window_content);    
}

void day::update_day_forecast(){
    std::map<int, std::string> wday {
        {0,"Sunday"},
        {1,"Monday"},
        {2,"Tuesday"},
        {3,"Wednesday"},
        {4,"Thursday"},
        {5,"Friday"},
        {6,"Saturday"}
    };
    std::map<int, std::string> name_mon{
        {0,"Jan"},
        {2,"Feb"},
        {3,"March"},
        {4,"April"},
        {5,"May"},
        {6,"June"},
        {7,"July"},
        {8,"Aug"},
        {9,"Sep"},
        {10,"Nov"},
        {11,"Dec"}
    };
    std::ifstream in(file_config::config_name);
    json data = json::parse(in);
    std::string date = get_date(day_offset).first;
    json response_data = get_response_data(city, date);
    this->weather = get_weather(data, response_data);
    this->temperatures = get_temperature(response_data); // max and min
    this->wind_speed = get_wind_speed(response_data); // max and min
    this->wind_direction = get_wind_direction(response_data);
    this->precipitation = get_precipitation(response_data);
    this->precipitation_prob = get_precipitation_prob(response_data);
}

void forecast::print_forecast(){
    std::ifstream in(file_config::config_name);
    json data = json::parse(in);
    std::vector<std::string> towns = data["towns"].get<std::vector<std::string>>();
    auto screen = ftxui::ScreenInteractive::TerminalOutput();
    int cnt = data["count_days"].get<int>();
    int index = 0;
    auto renderer = ftxui::Renderer([&]{
        ftxui::Elements a;
        if (!forecast.contains(towns[index])){
            std::vector<day> days;
            for (int i = 0; i < cnt; i++) {
                days.push_back(day(town(towns[0]), i));
            }
            forecast.insert({towns[0], days});
        }
        if (forecast[towns[0]].size()!=cnt) {
            for (int i = forecast[towns[index]].size(); i < cnt; i++) {
                forecast[towns[index]].push_back(day(town(towns[index]), i));
            }
        }   
        for (int i = 0; i < cnt; ++i) {
            a.push_back(forecast[towns[index]][i].create_day_forecast());
        }   
        return ftxui::window(ftxui::text(towns[index]),vbox(a));
    });
    ftxui::Component new_comp = CatchEvent(renderer, [&](ftxui::Event event){
        if(event == ftxui::Event::Escape){
            screen.ExitLoopClosure();
            return true;
        }
        else if (event == ftxui::Event::Character('+')) {
            if (cnt<=31){
                ++cnt;
            }
            screen.PostEvent(ftxui::Event::Custom);
            return true;
        }
        else if (event == ftxui::Event::Character('-')) {
            if(cnt>=1){
                --cnt;
            }
            screen.PostEvent(ftxui::Event::Custom);
            return true;
        }
        else if (event == ftxui::Event::Character('n')){
            index = (index == towns.size()-1) ? 0: index+1;
            screen.PostEvent(ftxui::Event::Custom);
            return true;
        }
        else if (event == ftxui::Event::Character('p')){
            index = (index == 0) ? towns.size()-1: index-1;
            screen.PostEvent(ftxui::Event::Custom);
            return true;
        }
        return false;
    });

    std::thread update([&]{
        while(true){
            std::this_thread::sleep_for(std::chrono::milliseconds(data["ms_update"].get<int>()));
            for(int i = 0; i < cnt; ++i){
                forecast[towns[index]][i].update_day_forecast();
            }
            screen.Post(ftxui::Event::Custom);
        }
    });

    screen.Loop(new_comp);
    update.detach();
}