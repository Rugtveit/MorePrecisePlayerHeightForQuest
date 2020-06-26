#include <dlfcn.h>
#include <iomanip>
#include <sstream>
#include "../extern/beatsaber-hook/shared/utils/utils.h"
#include "../extern/beatsaber-hook/shared/utils/logging.hpp"
#include "../extern/beatsaber-hook/include/modloader.hpp"
#include "../extern/beatsaber-hook/shared/utils/typedefs.h"
#include "../extern/beatsaber-hook/shared/utils/il2cpp-utils.hpp"
#include "../extern/beatsaber-hook/shared/utils/il2cpp-functions.hpp"
#include "../extern/beatsaber-hook/shared/config/config-utils.hpp"




static ModInfo modInfo;


static Configuration& getConfig() {
    static Configuration config(modInfo);
    return config;
}

const Logger& getLogger() {
  static const Logger& logger(modInfo);
  return logger;
}


std::string format = "{0:0.00}m";
Il2CppObject* _playerSettings = nullptr; 
float playerHeight = 0.0f;
Il2CppObject* _text = nullptr; 

MAKE_HOOK_OFFSETLESS(PlayerHeightSettingsController_RefreshUI, void, Il2CppObject* self)
{
    _playerSettings = CRASH_UNLESS(il2cpp_utils::GetFieldValue(self, "_playerSettings"));
    _text = CRASH_UNLESS(il2cpp_utils::GetFieldValue(self, "_text"));
    playerHeight = CRASH_UNLESS(il2cpp_utils::GetPropertyValue<float>(_playerSettings, "playerHeight"));
    
    std::stringstream stream;
    stream << std::fixed << std::setprecision(2) << playerHeight;
    std::string playerHeightFloat = stream.str();
    std::string preciseHeightString = playerHeightFloat + "m";
    
    CRASH_UNLESS(il2cpp_utils::SetPropertyValue(_text, "text", il2cpp_utils::createcsstr(preciseHeightString)));
}


extern "C" void setup(ModInfo &info)
{
    info.id = "MorePrecisePlayerHeight";
    info.version = "0.1.0";
    modInfo = info;
    getConfig();
    getLogger().info("Completed setup!");
    getLogger().info("Modloader name: %s", Modloader::getInfo().name.c_str());
}  


// This function is called when the mod is loaded for the first time, immediately after il2cpp_init.
extern "C" void load()
{
    getLogger().debug("Installing MorePrecisePlayerHeight!");
    INSTALL_HOOK_OFFSETLESS(PlayerHeightSettingsController_RefreshUI, il2cpp_utils::FindMethodUnsafe("", "PlayerHeightSettingsController", "RefreshUI", 0));
    getLogger().debug("Installed MorePrecisePlayerHeight!");
}

