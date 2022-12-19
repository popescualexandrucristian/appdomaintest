
using AppDomainTest;
using System;

namespace Example
{
    public class NormalPlugin : AppDomainTest.IPlugin
    {
        public void Execute(Context context)
        {
            string data = context.GetData();
            context.SetData(data.ToUpper());
            Console.WriteLine($"Executed normal plugin with {context.FileName}");
        }
    }
}