#include <std_include.hpp>
#include "loader/component_loader.hpp"

#include "game/game.hpp"

#include <utils/hook.hpp>
#include <utils/string.hpp>

namespace UWP
{
	namespace XStore
	{
#define IN_APP_OFFER_TOKEN_MAX_SIZE (64)
#define PRICE_MAX_SIZE (16)
#define TRIAL_UNIQUE_ID_MAX_SIZE (64)
#define SKU_ID_SIZE (5)
#define STORE_SKU_ID_SIZE (18)

		typedef struct XStoreGameLicense {
			char skuStoreId[STORE_SKU_ID_SIZE];
			bool isActive;
			bool isTrialOwnedByThisUser;
			bool isDiscLicense;
			bool isTrial;
			uint32_t trialTimeRemainingInSeconds;
			char trialUniqueId[TRIAL_UNIQUE_ID_MAX_SIZE];
			time_t expirationDate;
		} XStoreGameLicense;
	}

	using namespace XStore;

	enum ErrorCode : unsigned int
	{
		E_GAMERUNTIME_OK = 0,
		E_GAMERUNTIME_NOT_INITIALIZED = 0x89240100,
		E_GAMERUNTIME_DLL_NOT_FOUND = 0x89240101,
		E_GAMERUNTIME_VERSION_MISMATCH = 0x89240102,
		E_GAMERUNTIME_WINDOW_NOT_FOREGROUND = 0x89240103,
		E_GAMERUNTIME_SUSPENDED = 0x89240104,
		E_GAMERUNTIME_UNINITIALIZE_ACTIVEOBJECTS = 0x89240105,
		E_GAMERUNTIME_MULTIPLAYER_NOT_CONFIGURED = 0x89240106,
		E_GAMERUNTIME_OPTIONS_MISMATCH = 0x89240109,
		E_GAMERUNTIME_OPTIONS_NOT_SUPPORTED = 0x8924010A,
		E_GAMERUNTIME_GAMECONFIG_BAD_FORMAT = 0x8924010B,
		E_GAMERUNTIME_INVALID_HANDLE = 0x8924010C,
		E_GAMEUSER_MAX_USERS_ADDED = 0x89245100,
		E_GAMEUSER_SIGNED_OUT = 0x89245101,
	};

	HRESULT XStoreQueryGameLicenseResult(void* async, XStoreGameLicense* license)
	{
		utils::hook::invoke<HRESULT>(SELECT_VALUE(0x1402BD204, 0x1402D0A9C), async, license);
		license->isActive = true;
		license->isTrial = false;

		return S_OK;
	}

	void XErrorReport(HRESULT error_code, const char* error_msg)
	{
		printf("XGameRuntime Error: %d, %s\n", error_code, error_msg);
	}

	class component final : public component_interface
	{
	public:
		void post_load() override
		{
			utils::hook::set<FARPROC>(SELECT_VALUE(0x14410D938, 0x148C9EC40), (FARPROC)XErrorReport);

			// patch license check
			utils::hook::call(SELECT_VALUE(0x1401BD6CA, 0x1401B434A), XStoreQueryGameLicenseResult);


			// patch runtime check
			utils::hook::set<BYTE>(0x1401B2FD1 + 1, 0x88);
			utils::hook::set<BYTE>(0x1401B3096, 0x78);
			utils::hook::nop(0x1402A6A4B, 5);
			utils::hook::nop(0x1402A6368, 5);

			MessageBoxA(0, "", "", MB_OK);
		}

		component_priority priority() override
		{
			return component_priority::uwp;
		}
	};
}

REGISTER_COMPONENT(UWP::component)
