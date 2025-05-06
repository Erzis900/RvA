#pragma once

#include <functional>
#include <map>
#include <memory>

struct CallbackHandle {
	std::shared_ptr<int> token; // Owning pointer to track lifetime
};

template<typename... Args> class CallbackRegistry {
public:
	using Callback = std::function<void(Args...)>;

	[[nodiscard]] CallbackHandle registerCallback(Callback cb) {
		auto token = std::make_shared<int>(0);
		callbacks[token] = std::move(cb);

		return CallbackHandle{token};
	}

	template<typename... CallArgs> void executeCallbacks(CallArgs&&... args) {
		for (auto it = callbacks.begin(); it != callbacks.end();) {
			if (it->first.expired()) {
				it = callbacks.erase(it);
			} else {
				it->second(std::forward<CallArgs>(args)...);
				++it;
			}
		}
	}

private:
	std::map<std::weak_ptr<void>, Callback, std::owner_less<std::weak_ptr<void>>> callbacks;
};
