//
// Created by Dariush Tomlinson on 12/08/2025.
//

#pragma once
#include <string>
#include <optional>

template <typename T>
class Expected {
private:
    std::optional<T> value;
    std::string error;
public:
    Expected(T value) : value(std::move(value)){};

    Expected(std::string error) : error(std::move(error)){};

    bool hasError(){return !error.empty();};

    T& getValue(){return *value;}

    [[nodiscard]] std::string getError() const {
        return error;
    }


};