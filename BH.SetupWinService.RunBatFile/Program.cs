using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.IO;
using System.Linq;
using System.Reflection;
using System.Text;
using System.Threading.Tasks;

namespace BH.SetupWinService.RunBatFile
{
    class Program
    {
        static void Main(string[] args)
        {
            if(args.Length == 0)
            {
                Console.WriteLine("Bat file name is not passed.");
                return;
            }

            var proc = new Process();

            proc.StartInfo = new ProcessStartInfo();

            var servicePath = Assembly.GetEntryAssembly().Location;

            var batFile = Path.Combine(Path.GetDirectoryName(servicePath), args[0]);

            if(!File.Exists(batFile))
            {
                throw new FileNotFoundException(batFile);
            }

            proc.StartInfo.FileName = batFile;

            proc.StartInfo.Verb = "runas";

            proc.Start();
            proc.WaitForExit();
        }
    }
}
