
namespace AppDomainTest
{
    public interface IContext
    {
        string Owner { get; }
        string Data { get; set; }
    }

    public interface IPlugin
    {
        void Execute(IContext context);
    };
};