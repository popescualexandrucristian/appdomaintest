
using AppDomainTest;
using System;

namespace Example
{
    public class NormalPlugin : AppDomainTest.IPlugin
    {
        public void Execute(IContext context)
        {
            string newData = context.Data.ToUpper();
            context.Data = newData;
            string info = "normal plugin";
            context.Call(ref info);
            context.Call(ref info);
            Console.WriteLine($"Executed normal plugin with {context.Owner}");
        }
    }
}