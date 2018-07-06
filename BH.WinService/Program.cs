using System;
using System.Collections.Generic;
using System.Linq;
using System.ServiceProcess;
using System.Text;
using System.Threading.Tasks;
using System.Threading;
using System.Diagnostics;
using System.Configuration;
using System.Reflection;

namespace BH.WinService
{
    static class Program
    {
        /// <summary>
        /// The main entry point for the application.
        /// </summary>
        static void Main(string[] args)
        {
            if(args.Length > 0)
            {
                var proc = new System.Diagnostics.Process();
                proc.StartInfo = new System.Diagnostics.ProcessStartInfo();
                proc.StartInfo.FileName = ConfigurationManager.AppSettings["InstallutilPath"];

                switch (args[0])
                {
                    case "-i":
                        proc.StartInfo.Arguments = $" \"{Assembly.GetEntryAssembly().Location}\"";
                        break;
                    case "-u":
                        proc.StartInfo.Arguments = $" -u \"{Assembly.GetEntryAssembly().Location}\"";
                        break;
                    default:
                        throw new Exception("Command doesn't recornized. Choose -i for install service and -u for uninstall.");
                }
                
                //proc.StartInfo.UseShellExecute = false;
                //proc.StartInfo.CreateNoWindow = true;
                //proc.StartInfo.WindowStyle = ProcessWindowStyle.Hidden;
                proc.StartInfo.Verb = "runas";

                proc.Start();
                proc.WaitForExit();

                return;
            }

#if DEBUG
            //run console

            var ws = new WinService();
            ws.TestStart();

            while (true)
            {
                Thread.Sleep(60000);
            }

            //ws.TestStop();

#else
            ServiceBase[] ServicesToRun;

                ServicesToRun = new ServiceBase[]
                {
                new WinService()
                };

                ServiceBase.Run(ServicesToRun);
#endif
        }
    }
}
