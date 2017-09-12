using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading;
using System.Threading.Tasks;

namespace FTContent
{
    class Program
    {
        static void Main(string[] args)
        {
            Loader.StartWebservice();

            Console.WriteLine("Service started ...");

            //infinity wait
            while (true)
            {
                Thread.Sleep(60000);
            }
        }
    }
}
