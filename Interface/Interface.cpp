#include "Interface.h"

namespace AppDomainTest
{
	Sandbox::Sandbox()
	{
	}

	Sandbox^ Sandbox::Create(System::Collections::Generic::Dictionary<System::String^, array<unsigned char>^>^ untrastedPluginData)
	{
		System::AppDomain^ restrictedDomain = Sandbox::CreateRestrictedAppDomainForSandbox();

		System::Type^ strongType = Sandbox::typeid;
		Sandbox^ newDomainInstance = safe_cast<Sandbox^>(restrictedDomain->CreateInstanceFromAndUnwrap(strongType->Assembly->Location, strongType->FullName));

		newDomainInstance->untrastedPluginData = untrastedPluginData;
		newDomainInstance->restrictedDomain = restrictedDomain;

		return newDomainInstance;
	}

	void Sandbox::Destroy()
	{
		if (restrictedDomain)
			System::AppDomain::Unload(restrictedDomain);
		restrictedDomain = nullptr;

		if (untrustedPlugins)
			untrustedPlugins->Clear();
		untrustedPlugins = nullptr;
	}

	void Sandbox::ExecuteUntrustedCode(AppDomainTest::Context^ data)
	{
		try
		{
			if (untrustedPlugins)
				for each (auto byteStream in untrastedPluginData)
					untrustedPlugins[byteStream.Key] = System::Reflection::Assembly::Load(byteStream.Value);

			for each (auto plugin in untrustedPlugins)
			{
				array<System::Type^>^ types = plugin.Value->GetTypes();
				for each (auto type in types)
				{

					bool isAssignableFrom = AppDomainTest::IPlugin::typeid->IsAssignableFrom(type);
					if (isAssignableFrom)
					{
						AppDomainTest::IPlugin^ plugin = safe_cast<AppDomainTest::IPlugin^>(System::Activator::CreateInstance(type));
						plugin->Execute(data);
					}
				}
			}
		}
		catch (System::Exception^ ex)
		{
			System::Console::WriteLine(ex->Message);
		}

	}

	System::AppDomain^ Sandbox::CreateRestrictedAppDomainForSandbox()
	{
		System::Security::PermissionSet^ permSet = gcnew System::Security::PermissionSet(System::Security::Permissions::PermissionState::None);

		permSet->AddPermission(gcnew System::Security::Permissions::SecurityPermission(System::Security::Permissions::SecurityPermissionFlag::Execution));

		permSet->AddPermission(gcnew System::Security::Permissions::SecurityPermission(System::Security::Permissions::SecurityPermissionFlag::Infrastructure));

		permSet->AddPermission(gcnew System::Security::Permissions::SecurityPermission(System::Security::Permissions::SecurityPermissionFlag::RemotingConfiguration));

		/*permSet->AddPermission(gcnew System::Security::Permissions::FileIOPermission(
			System::Security::Permissions::FileIOPermissionAccess::Read |
			System::Security::Permissions::FileIOPermissionAccess::PathDiscovery, System::AppDomain::CurrentDomain->SetupInformation->ApplicationBase));
			*/

		permSet->AddPermission(gcnew System::Security::Permissions::FileIOPermission(System::Security::Permissions::PermissionState::Unrestricted));

		System::AppDomainSetup^ appDomainSetup = gcnew System::AppDomainSetup();
		appDomainSetup->ApplicationBase = System::AppDomain::CurrentDomain->SetupInformation->ApplicationBase;

		System::AppDomain^ newDomain = System::AppDomain::CreateDomain("Sandbox", nullptr, appDomainSetup, permSet);
		return newDomain;
	}
};