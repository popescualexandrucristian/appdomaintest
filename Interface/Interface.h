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
};