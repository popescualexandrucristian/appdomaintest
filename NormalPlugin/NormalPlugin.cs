
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
            Console.WriteLine($"Executed normal plugin with {context.Owner}");
        }
    }
}