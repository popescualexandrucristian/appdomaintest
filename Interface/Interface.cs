
namespace AppDomainTest
{
    public interface IContext
    {
        string Owner { get; }
        string Data { get; set; }

        void Call(ref string info);
    }

    public interface IPlugin
    {
        void Execute(IContext context);
    };
};