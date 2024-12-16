#include <std_include.hpp>
#include "loader/component_loader.hpp"
#include "game_module.hpp"

#include <game/game.hpp>

#include <utils/hook.hpp>
#include <utils/string.hpp>

namespace patches
{
	namespace
	{
		std::string GetExecutableName()
		{
			char path[MAX_PATH];
			if (GetModuleFileNameA(NULL, path, MAX_PATH) > 0)
			{
				std::string fullPath(path);
				size_t pos = fullPath.find_last_of("\\/");
				return (pos == std::string::npos) ? fullPath : fullPath.substr(pos + 1);
			}
			return "";
		}

		utils::hook::detour relaunch_hook;
		void relaunch_stub(const char* filename, const char* params)
		{
			if (filename == "iw4sp.exe"s || filename == "iw4mp.exe"s)
			{
				static char buf[MAX_PATH];
				memset(buf, 0, sizeof(buf));
				
				auto exe_name = GetExecutableName();
				assert(exe_name.size() < MAX_PATH);
				memcpy(buf, exe_name.data(), exe_name.size());
				
				filename = buf;

				params = utils::string::va("%s -%s", params, game::environment::is_sp() ? "mp" : "sp");

				relaunch_hook.invoke<void>(filename, params);
				return;
			}

			relaunch_hook.invoke<void>(filename, params);
		}
	}

	class component final : public component_interface
	{
	public:
		void post_load() override
		{
			relaunch_hook.create(SELECT_VALUE(0x140295D20, 0x1402A8D20), relaunch_stub);
		}
	};
}

REGISTER_COMPONENT(patches::component)
