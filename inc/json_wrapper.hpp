#ifndef GL_JSON_PARSER_HPP
#define GL_JSON_PARSER_HPP

#include "user.hpp"
#include "json.hpp"

#include <map>
#include <vector> 
#include <iostream>

void to_json(nlohmann::json &j, const User &u)
{
    /*
        Used by json.hpp as serializer
    */
    j = nlohmann::json{
        {"uid", u.uid},
        {"ip", u.ip},
        {"coords",
         {{"x", u.coords.x},
          {"y", u.coords.y}}}};
}

void from_json(const nlohmann::json &j, User &u)
{
    /*
        Used by json.hpp as DEserializer
    */
    j.at("uid").get_to(u.uid);
    j.at("ip").get_to(u.ip);
    j.at("/coords/x"_json_pointer).get_to(u.coords.x);
    j.at("/coords/y"_json_pointer).get_to(u.coords.y);
}


std::vector<std::uint8_t> map_to_json(std::map<int,User> users)
{
    /*
        Convert from map<int id, User> to binary array
    */
    std::map<std::string, User> temp;

    for (auto const & pair : users) {
        temp.emplace(std::to_string(pair.first), pair.second);
    }
    nlohmann::json serialize = temp;
    return  nlohmann::json::to_cbor(serialize);
}

std::map<int,User> json_to_map(std::vector<std::uint8_t> bin_json)
{
    /*
        Convert from binary array to map<int id, User>
    */
    nlohmann::json deserialize;
    try{
        deserialize = nlohmann::json::from_cbor(bin_json.begin(),bin_json.end(),false,true);
    }catch(nlohmann::json::parse_error& e){
        std::cout << e.what() << "\nid - "<<  e.id << "\nbyte - " << e.byte << std::endl;
    }
    std::map<std::string, User> temp = deserialize;
    std::map<int, User> output;
    std::stringstream ss;
    int temp_id = 0;
    for (auto &pair : temp)
    {
        ss << pair.first;
        ss >> temp_id;
        output.emplace(temp_id, std::move(pair.second));
        ss.clear();
        temp_id = 0;
    }
    return output;
}

#endif
