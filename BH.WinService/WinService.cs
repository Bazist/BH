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
using System.ComponentModel;
using System.Data;
using System.Diagnostics;
using System.Linq;
using System.ServiceProcess;
using System.Text;
using System.Threading.Tasks;
using System.Threading;
using BH.WCF;
using System.IO;
using System.Configuration;
using BH.BaseRobot;
using System.Reflection;
using BH.BoobenRobot;

namespace BH.WinService
{
    public partial class WinService : ServiceBase
    {
        public WinService()
        {
            if (!Environment.UserInteractive)
            {
                InitializeComponent();

                this.ServiceName = @"BH.FTSearch";
                this.EventLog.Source = this.ServiceName;
                this.EventLog.Log = "Application";

                AppDomain.CurrentDomain.UnhandledException += CurrentDomain_UnhandledException;
            }
        }

        private void CurrentDomain_UnhandledException(object sender, UnhandledExceptionEventArgs e)
        {
            Exception ex = (Exception)e.ExceptionObject;

            WriteLog(ex.Message + ex.StackTrace, EventLogEntryType.Error);
        }

        private static FTService _fts = new FTService();
        private static IEnumerable<IBaseRobot> _robots;
        private string CurrentDirectory => Path.GetDirectoryName(Assembly.GetEntryAssembly().Location);

        public void TestStart()
        {
            OnStart(null);
        }

        public void TestStop()
        {
            OnStop();
        }

        protected override void OnSessionChange(SessionChangeDescription changeDescription)
        {
            WriteLog("Session change Service", EventLogEntryType.Information);

            base.OnSessionChange(changeDescription);
        }

        protected override void OnPause()
        {
            WriteLog("Pause Service", EventLogEntryType.Information);

            base.OnPause();
        }

        private void StartService()
        {
            WriteLog("Start Service", EventLogEntryType.Information);

            //start web service
            try
            {
                var conf = FTService.GetDefaultConfiguration();

                var directory = ConfigurationManager.AppSettings["IndexPath"];

                if (!Directory.Exists(directory))
                {
                    WriteLog($"IndexPath = {directory} in config file is not found.", EventLogEntryType.Warning);

                    directory = Path.Combine(CurrentDirectory, directory);

                    WriteLog($"IndexPath built as {directory}.", EventLogEntryType.Warning);
                }

                conf.SetIndexPath(directory);
                conf.LimitUsedMemory = ulong.Parse(ConfigurationManager.AppSettings["LimitUsedMemory"]);
                conf.MemoryMode = bool.Parse(ConfigurationManager.AppSettings["InMemoryMode"]) ? (uint)FTSearch.MemoryMode.InMemory : (uint)FTSearch.MemoryMode.HDD;

                _fts.SetConfiguration(conf);

                ThreadPool.QueueUserWorkItem
                (
                    x =>
                    {
                        WriteLog("Start FTS Service", EventLogEntryType.Information);

                        _fts.Start();
                        
                        if (bool.Parse(ConfigurationManager.AppSettings["BoobenMode"]))
                        {
                            StartBoobenRobots();
                        }
                        else
                        {
                            StartRobots();
                        }

                        if (bool.Parse(ConfigurationManager.AppSettings["EnableRelSearch"]))
                        {
                            _fts.InitSearchRel();
                        }
                    }
                );

                FTService.StartWebservice(_fts,
                                          ConfigurationManager.AppSettings["URL"],
                                          LoadDocumentContent,
                                          ex =>
                                          {
                                              WriteLog(ex.Message + ex.StackTrace, EventLogEntryType.Error);
                                          });

                WriteLog("Service started.", EventLogEntryType.Information);
            }
            catch (Exception ex)
            {
                WriteLog(ex.Message + ex.StackTrace, EventLogEntryType.Error);

                Stop();

                throw ex;
            }
        }

        private string LoadDocumentContent(string documentName,
                                           string documentVersion,
                                           string robotName)
        {
            var robot = _robots.FirstOrDefault(x => x.Name == robotName);

            if (robot != null)
            {
                return robot.LoadDocumentContent(documentName, documentVersion);
            }
            else
            {
                return null;
            }
        }

        private void StartBoobenRobots()
        {
            var enableRobots = bool.Parse(ConfigurationManager.AppSettings["EnableRobots"]);
            var indexCurrentMonth = bool.Parse(ConfigurationManager.AppSettings["IndexCurrentMonth"]);

            if (enableRobots || indexCurrentMonth)
            {
                WriteLog("Start Job", EventLogEntryType.Information);

                try
                {
                    IndexService indexService = new IndexService(_fts,
                                                                 x => WriteLog(x, EventLogEntryType.Error),
                                                                 x => Debug.WriteLine(x));

                    if (indexCurrentMonth)
                    {
                        WriteLog("Index current month", EventLogEntryType.Information);

                        indexService.IndexCurrentMonth();
                    }

                    if (enableRobots)
                    {
                        WriteLog("Start Booben robots", EventLogEntryType.Information);

                        indexService.RunRobots();
                    }
                }
                catch (Exception ex)
                {
                    WriteLog(ex.Message + ex.StackTrace, EventLogEntryType.Error);

                    Stop();

                    throw ex;
                }

                WriteLog("Job Started.", EventLogEntryType.Information);
            }
        }

        private void StartRobots()
        {
            if (bool.Parse(ConfigurationManager.AppSettings["EnableRobots"]))
            {
                WriteLog("Start Robots", EventLogEntryType.Information);

                var factory = RobotFactory.CreateFactory();

                _robots = factory.CreateRobots();

                foreach (var robot in _robots)
                {
                    robot.Start(_fts);
                }

                WriteLog("Robots Started.", EventLogEntryType.Information);
            }
        }

        protected override void OnStart(string[] args)
        {
            StartService();
        }

        protected override void OnStop()
        {
            WriteLog("Stop Service", EventLogEntryType.Information);

            //start web service
            try
            {
                if (_robots != null)
                {
                    foreach (var robot in _robots)
                    {
                        robot.Stop();
                    }
                }

                FTService.StopWebService();

                _fts.Stop();

                WriteLog("Service stoped.", EventLogEntryType.Information);
            }
            catch (Exception ex)
            {
                WriteLog(ex.Message + ex.StackTrace, EventLogEntryType.Error);

                throw ex;
            }
        }

        private void WriteLog(string message, EventLogEntryType eventType)
        {
            message = $"[{DateTime.Now.ToString()}] {message}; Memory: {System.Diagnostics.Process.GetCurrentProcess().WorkingSet64 / 1024 / 1024} mb.\r\n";

            File.AppendAllText(Path.Combine(CurrentDirectory, "log.txt"), message);

            if (Environment.UserInteractive)
            {
                Trace.WriteLine(message);
            }
            else
            {
                ((ISupportInitialize)(this.EventLog)).BeginInit();
                if (!EventLog.SourceExists(this.EventLog.Source))
                {
                    EventLog.CreateEventSource(this.EventLog.Source, this.EventLog.Log);
                }
                ((ISupportInitialize)(this.EventLog)).EndInit();

                this.EventLog.WriteEntry(message, eventType);
            }
        }

        //#region Index Files

        //private bool IndexFile(string name, string text)
        //{
        //    bool res = false;

        //    if (name.Length > FTSearch.DOC_NAME_LENGTH - 1)
        //    {
        //        name = name.Substring(0, FTSearch.DOC_NAME_LENGTH - 1);
        //    }

        //    _readBytes += text.Length;

        //    _amountDocuments += 1;

        //    //string alias = "dou-" + fi.Name.Split('.')[0].Split('_')[1]; //tid.ToString("00000000") + "-" + pg.ToString("00000");
        //    //string alias = "lib-" + fi.Name.Split('.')[0]; //tid.ToString("00000000") + "-" + pg.ToString("00000");

        //    //string alias = "skc-" + fi.Name.Split('_')[1] + "-" + fi.Name.Split('_')[3]; //tid.ToString("00000000") + "-" + pg.ToString("00000");

        //    //index
        //    if (!string.IsNullOrEmpty(text))
        //    {
        //        res = _fts.IndexText(name, text);
        //    }

        //    return res;
        //}

        //static void Unpack(string src, string dest)
        //{
        //    var proc = new System.Diagnostics.Process();
        //    proc.StartInfo = new System.Diagnostics.ProcessStartInfo();
        //    proc.StartInfo.FileName = @"C:\Program Files\7-Zip\7z.exe";
        //    proc.StartInfo.Arguments = " x " + src + " -o\"" + dest + "\" -r";
        //    proc.StartInfo.UseShellExecute = false;
        //    proc.StartInfo.CreateNoWindow = true;
        //    proc.StartInfo.WindowStyle = ProcessWindowStyle.Hidden;
        //    //proc.StartInfo.Verb = "runas";

        //    proc.Start();
        //    proc.WaitForExit();

        //    //Console.WriteLine(proc.StandardError.ReadToEnd());
        //    //Console.WriteLine(proc.StandardOutput.ReadToEnd());
        //}

        //private bool IndexFiles(FTService fts, string rootPath, string currPath)
        //{
        //    bool res = false;

        //    DirectoryInfo di = new DirectoryInfo(currPath);
        //    foreach (DirectoryInfo subDi in di.GetDirectories("*", SearchOption.AllDirectories))
        //    {
        //        res |= IndexFiles(fts, rootPath, subDi.FullName);
        //    }

        //    foreach (FileInfo fi in di.GetFiles())
        //    {
        //        string extension = fi.Extension.ToLower();

        //        //if(extension == ".zip")
        //        //{
        //        //    using (FileStream zipToOpen = new FileStream(fi.FullName, FileMode.Open))
        //        //    {
        //        //        using (ZipArchive archive = new ZipArchive(zipToOpen, ZipArchiveMode.Read))
        //        //        {
        //        //            foreach (ZipArchiveEntry zipEntry in archive.Entries)
        //        //            {
        //        //                using (StreamReader sr = new StreamReader(zipEntry.Open(), Encoding.UTF8))
        //        //                {
        //        //                    readTime.Start();

        //        //                    string text = sr.ReadToEnd();

        //        //                    readTime.Stop();

        //        //                    this.IndexFile(fi.FullName + "\\" + zipEntry.FullName, text);
        //        //                }

        //        //                GC.Collect();
        //        //            }
        //        //        }
        //        //    }
        //        //}
        //        //else 
        //        if ((extension == ".txt" || extension == ".html" ||
        //                  extension == ".cs" || extension == ".vb" ||
        //                  extension == ".cpp" || extension == ".fb2" ||
        //                  extension == ".log" || extension == ".csv" ||
        //                  extension == ".xml"))
        //        {
        //            long maxSize = 1024 * 1024 * 10;

        //            string fullName = fi.FullName.Replace(rootPath, "");

        //            if (fi.Length < maxSize) //10mb
        //            {
        //                string text = FTSearch.ReadFileText(fi.FullName);

        //                res |= this.IndexFile(fi.FullName, text);
        //            }
        //            else
        //            {
        //                using (StreamReader sr = new StreamReader(fi.FullName))
        //                {
        //                    for (long i = 0; i < fi.Length; i += maxSize)
        //                    {
        //                        long size;

        //                        if (i + maxSize < fi.Length)
        //                        {
        //                            size = maxSize;
        //                        }
        //                        else
        //                        {
        //                            size = fi.Length - i;
        //                        }

        //                        char[] buffer = new char[size + 1];

        //                        sr.Read(buffer, 0, (int)size);

        //                        res |= this.IndexFile(fullName, new string(buffer));
        //                    }
        //                }
        //            }
        //        }
        //    }

        //    return res;
        //}

        //private void Index()
        //{
        //    var srcPath = ConfigurationManager.AppSettings["SourcePath"];
        //    var destPath = ConfigurationManager.AppSettings["DestinationPath"];
        //    var logPath = ConfigurationManager.AppSettings["LogPath"];

        //    string logText = string.Empty;

        //    if (File.Exists(logPath))
        //    {
        //        logText = File.ReadAllText(logPath);
        //    }

        //    var archives = Directory.GetFiles(srcPath, "*WS*.7z", SearchOption.AllDirectories).Concat(
        //                   Directory.GetFiles(srcPath, "*BS*.7z", SearchOption.AllDirectories)).ToList();

        //    //N:\Production-Logs
        //    foreach (string archive in archives)
        //    {
        //        if (logText.Contains(archive))
        //            continue;

        //        WriteLog($"Archive {(archives.IndexOf(archive) + 1).ToString()} of {archives.Count.ToString()}", EventLogEntryType.Information);

        //        string path = archive.Replace(srcPath, destPath);

        //        string dirName = Path.GetDirectoryName(path);
        //        if (!Directory.Exists(dirName))
        //        {
        //            Directory.CreateDirectory(dirName);
        //        }

        //        if (!File.Exists(path))
        //        {
        //            File.Copy(archive, path, true);
        //        }

        //        string rootPath = destPath + @"Unpacked\";

        //        string unpackPath = rootPath + Path.GetFileName(archive).Replace(".7z", "");

        //        if (!Directory.Exists(unpackPath))
        //        {
        //            Unpack(path, unpackPath);
        //        }

        //        _indexedArchives += archive + "\r\n";

        //        if (this.IndexFiles(_fts, rootPath, unpackPath)) //need save on disc
        //        {
        //            SaveIndex(logPath);
        //        }

        //        Directory.Delete(unpackPath, true);

        //        File.Delete(path);

        //        //File.AppendAllText(logPath, archive + "\r\n");

        //        //break;
        //    }

        //    SaveIndex(logPath);
        //}

        //private void SaveIndex(string logPath)
        //{
        //    if (!string.IsNullOrEmpty(_indexedArchives))
        //    {
        //        WriteLog($"Start save index ...", EventLogEntryType.Information);

        //        //save index
        //        _fts.SaveIndex();

        //        File.AppendAllText(logPath, _indexedArchives + "\r\n");

        //        WriteLog($"Index saved.", EventLogEntryType.Information);

        //        _indexedArchives = string.Empty;
        //    }
        //}

        //#endregion
    }
}
