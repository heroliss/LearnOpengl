#pragma once
#include <functional>
#include <vector>

template<typename... Args>
class Event {
public:
    using EventHandler = std::function<void(Args...)>;

    void AddHandler(const EventHandler& handler) {
        handlers.push_back(handler);
    }

    void RemoveHandler(const EventHandler& handler) {
        handlers.erase(std::remove(handlers.begin(), handlers.end(), handler), handlers.end());
    }

    void Invoke(Args... args) {
        for (const auto& handler : handlers) {
            handler(args...);
        }
    }

private:
    std::vector<EventHandler> handlers;
};