using System;
using System.IO;
using System.Reflection;
using System.Runtime.InteropServices;
using Microsoft.Extensions.DependencyInjection;
using Microsoft.Extensions.Hosting;
using Microsoft.Extensions.Logging;

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
            .ConfigureLogging(logging =>
            {
                logging.AddEventLog(eventLogSettings =>
                {
                    eventLogSettings.SourceName = "BH";
                });
            })
                .ConfigureServices((hostContext, services) =>
                {
                    services.AddHostedService<Worker>();
                });
    }
}
