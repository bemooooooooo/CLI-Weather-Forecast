add_library(requests requests.cpp requests.h)

target_link_libraries(requests PUBLIC cpr::cpr)
target_link_libraries(requests PUBLIC nlohmann_json::nlohmann_json)
target_link_libraries(requests PUBLIC ftxui::screen PUBLIC ftxui::dom PUBLIC ftxui::component)