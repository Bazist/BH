using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Reflection;
using System.Runtime.InteropServices;
using System.Threading.Tasks;
using Microsoft.Extensions.DependencyInjection;
using Microsoft.Extensions.Hosting;

namespace BH.WorkerService
{
    public class Program
    {
        public static void Main(string[] args)
        {
            if (args.Length > 0)
            {
                var proc = new System.Diagnostics.Process();
                proc.StartInfo = new System.Diagnostics.ProcessStartInfo();

                proc.StartInfo.FileName = Path.Combine(RuntimeEnvironment.GetRuntimeDirectory(), "InstallUtil.exe");

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

            CreateHostBuilder(args).Build().Run();
        }

        public static IHostBuilder CreateHostBuilder(string[] args) =>
            Host.CreateDefaultBuilder(args)
                .ConfigureServices((hostContext, services) =>
                {
                    services.AddHostedService<Worker>();
                });
    }
}
