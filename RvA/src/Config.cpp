#include "Config.h"

#include "constants.h"

#include <fstream>
#include <iostream>

#ifdef __EMSCRIPTEN__
#include <emscripten/emscripten.h>

// clang-format off
extern "C" {
EM_JS(void, js_mount_persistent, (), {
  Asyncify.handleSleep(wakeUp => {
    // only mkdir if it doesn't already exist
    if (!FS.analyzePath('/persistent').exists) {
      FS.mkdir('/persistent');
    }
    FS.mount(IDBFS, {}, '/persistent');
    FS.syncfs(true, err => {
      if (err) console.error('syncfs load error', err);
      wakeUp();
    });
  });
});

EM_JS(void, js_sync_persistent, (), {
  Asyncify.handleSleep(wakeUp => {
    FS.syncfs(false, err => {
      if (err) console.error("syncfs save error:", err);
      else console.log("Save complete!");
      wakeUp();
    });
  });
});
}
// clang-format on
#endif

Config::Config() {
#ifdef __EMSCRIPTEN__
	js_mount_persistent();
	std::string configFilePath("/persistent/options.json");
#else
	std::string configFilePath("assets/config/options.json");
#endif
	std::ifstream configFile(configFilePath);
	std::string contents;

	if (configFile.is_open()) {
		contents.assign(std::istreambuf_iterator<char>(configFile), std::istreambuf_iterator<char>());
	}

	bool needDefaults = contents.empty() || !nlohmann::json::accept(contents);
	if (needDefaults) {
		m_data = {{"fullscreen", false},
				  {"musicEnabled", true},
				  {"soundEnabled", true},
				  {"tutorialEnabled", true},
				  {"sfxVolume", 0.5},
				  {"musicVolume", 0.5},
				  {"masterVolume", 0.5},
				  {"cheatEnabled", false},
				  {"levelId", "none"}};

		std::ofstream ofs(configFilePath);
		ofs << m_data.dump(4);
		ofs.close();
#ifdef __EMSCRIPTEN__
		js_sync_persistent();
#endif
	} else {
		m_data = nlohmann::json::parse(contents);
	}

	options.isFullscreen = m_data["fullscreen"];
	options.isMusicEnabled = m_data["musicEnabled"];
	options.isSoundEnabled = m_data["soundEnabled"];
	options.isTutorialEnabled = m_data["tutorialEnabled"];
	options.sfxVolume = m_data["sfxVolume"];
	options.musicVolume = m_data["musicVolume"];
	options.masterVolume = m_data["masterVolume"];
	options.cheatEnabled = m_data["cheatEnabled"];
	options.levelId = m_data["levelId"];
}

void Config::save() {
#ifdef __EMSCRIPTEN__
	std::ofstream configFile("/persistent/options.json");
#else
	std::ofstream configFile("assets/config/options.json");
#endif
	if (!configFile.is_open()) {
		std::cerr << "Failed to open config file for writing" << std::endl;
		return;
	}

	m_data["fullscreen"] = options.isFullscreen;
	m_data["musicEnabled"] = options.isMusicEnabled;
	m_data["soundEnabled"] = options.isSoundEnabled;
	m_data["tutorialEnabled"] = options.isTutorialEnabled;
	m_data["sfxVolume"] = options.sfxVolume;
	m_data["musicVolume"] = options.musicVolume;
	m_data["masterVolume"] = options.masterVolume;
	m_data["cheatEnabled"] = options.cheatEnabled;
	m_data["levelId"] = options.levelId;

	configFile << m_data.dump(4);
	configFile.close();
#ifdef __EMSCRIPTEN__
	js_sync_persistent();
#endif
}

bool Config::hasLevel() const {
	return !options.levelId.empty() && options.levelId != "none";
}
