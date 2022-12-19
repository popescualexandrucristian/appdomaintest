namespace AppDomainTest
{
    public interface class Context
    {
        property System::String^ FileName;

        void SetData(System::String^ newData);
        System::String^ GetData();
    };

    public interface class IPlugin {
        void Execute(Context^ context);
    };
};