
using AppDomainTest;
using System;

namespace Example
{
   public class NormalPlugin : IPlugin
   {
      public void Execute(IContext context)
      {
         string newData = context.Data.ToUpper();
         context.Data = newData;
         string info = "normal plugin";
         context.Call(ref info);
         context.Call(ref info);
         System.Threading.Thread.Sleep(2000);
         Console.WriteLine($"Executed normal plugin with {context.Owner}");
      }
   }
}