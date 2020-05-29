/*
# Copyright(C) 2010-2020 Viacheslav Makoveichuk (email: slv709@gmail.com, skype: vyacheslavm81)
# This file is part of Black Hole FTS.
#
# Black Hole FTS is free software : you can redistribute it and / or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# Black Hole FTS is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with Foobar.  If not, see <http://www.gnu.org/licenses/>.
*/

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
using System.IO;
using System.Runtime.InteropServices;

namespace BH.WinService
{
    static class Program
    {
        /// <summary>
        /// The main entry point for the application.
        /// </summary>
        static void Main(string[] args)
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

            if (Environment.UserInteractive)
            {
                //run console

                var ws = new WinService();
                ws.TestStart();

                while (true)
                {
                    Thread.Sleep(60000);
                }
            }
            else
            {
                ServiceBase[] ServicesToRun;

                ServicesToRun = new ServiceBase[]
                {
                new WinService()
                };

                ServiceBase.Run(ServicesToRun);
            }
        }
    }
}
