#include <dlfcn.h>
#include <iomanip>
#include <sstream>
#include "../extern/beatsaber-hook/shared/utils/utils.h"
#include "../extern/beatsaber-hook/shared/utils/logging.hpp"
#include "../extern/modloader/shared/modloader.hpp"
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

static struct Config_t 
{
    bool enabled = true;
    bool measureInFeet = false; 
} Config; 

Il2CppObject* playerSettings = nullptr; 
float playerHeight = 0.0f;
Il2CppObject* heightText = nullptr; 

MAKE_HOOK_OFFSETLESS(PlayerHeightSettingsController_RefreshUI, void, Il2CppObject* self)
{
    if(Config.enabled) 
    {
        heightText = CRASH_UNLESS(il2cpp_utils::GetFieldValue(self, "_text"));
        playerHeight = CRASH_UNLESS(il2cpp_utils::GetFieldValue<float>(self, "_value"));
        std::string heightString; 
        
        if(Config.measureInFeet)
        {
            float inchFeet = playerHeight / 0.3048;
            int wholeFeet = (int) inchFeet;
            float inches = round((inchFeet - wholeFeet) / 0.0833);
            std::stringstream stream;
            stream << std::fixed << std::setprecision(1) << inches;
            std::string precisionInchesFloat = stream.str();
            heightString = std::to_string(wholeFeet) + "ft" + "\n" + precisionInchesFloat + "\"";
        } else 
        {
            std::stringstream stream;
            stream << std::fixed << std::setprecision(2) << playerHeight;
            std::string precisionHeightFloat = stream.str();
            heightString = "<size=90%>" + precisionHeightFloat + "m" + "</size>";
        }
        
        CRASH_UNLESS(il2cpp_utils::SetPropertyValue(heightText, "text", il2cpp_utils::createcsstr(heightString)));
    } 
    else PlayerHeightSettingsController_RefreshUI(self); 
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

bool LoadConfig();
void SaveConfig();
// This function is called when the mod is loaded for the first time, immediately after il2cpp_init.
extern "C" void load()
{
    if(!LoadConfig()) SaveConfig();
    getLogger().debug("Installing MorePrecisePlayerHeight!");
    INSTALL_HOOK_OFFSETLESS(PlayerHeightSettingsController_RefreshUI, il2cpp_utils::FindMethodUnsafe("", "PlayerHeightSettingsController", "RefreshUI", 0));
    getLogger().debug("Installed MorePrecisePlayerHeight!");
}

void SaveConfig()
{
    getConfig().config.RemoveAllMembers();
    getConfig().config.SetObject();
    auto& allocator = getConfig().config.GetAllocator();
    getConfig().config.AddMember("enabled", Config.enabled, allocator);
    getConfig().config.AddMember("measureInFeet", Config.measureInFeet, allocator);
    getConfig().Write();
}

bool LoadConfig()
{
    getConfig().Load();
    if(getConfig().config.HasMember("enabled") && getConfig().config["enabled"].IsBool()) Config.enabled = getConfig().config["enabled"].GetBool(); 
    else return false;
    if(getConfig().config.HasMember("measureInFeet") && getConfig().config["measureInFeet"].IsBool()) Config.measureInFeet = getConfig().config["measureInFeet"].GetBool(); 
    else return false;
    return true;
}