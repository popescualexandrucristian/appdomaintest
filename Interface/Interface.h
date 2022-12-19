namespace AppDomainTest
{
    public interface class Context
    {
        property System::String^ Owner { System::String^ get(); };

        void SetData(System::String^ newData);
        System::String^ GetData();
    };

    public interface class IPlugin {
        void Execute(Context^ context);
    };

    public ref class Sandbox : public System::MarshalByRefObject
    {
    public:
        Sandbox();

        static Sandbox^ Create(System::Collections::Generic::Dictionary<System::String^, array<unsigned char>^>^ untrastedPluginData);
        void Destroy();
        void ExecuteUntrustedCode(AppDomainTest::Context^ data);
    private:
        static System::AppDomain^ Sandbox::CreateRestrictedAppDomainForSandbox();

        System::AppDomain^ restrictedDomain;
        System::Collections::Generic::Dictionary<System::String^, array<unsigned char>^>^ untrastedPluginData;
        System::Collections::Generic::Dictionary<System::String^, System::Reflection::Assembly^>^ untrustedPlugins;
    };
};