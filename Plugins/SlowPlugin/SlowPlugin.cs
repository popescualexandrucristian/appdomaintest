
using AppDomainTest;
using System;

namespace Example
{
   public class NormalPlugin : IPlugin
   {
      public void Execute(IContext context)
      {
         Console.WriteLine($"Started slow plugin execution with {context.Owner}");
         //System.Threading.Thread.Sleep(5000);
         while (true) ;
         Console.WriteLine($"Executed slow plugin with {context.Owner}");
      }
   }
}