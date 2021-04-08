using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.IO;
using System.Linq;
using System.Reflection;
using System.Threading;
using System.Threading.Tasks;
using BH.BaseRobot;
using BH.BoobenRobot;
using BH.FTServer;
using BH.REST;
using BH.WorkerService.Options;
using Microsoft.Extensions.Hosting;
using Microsoft.Extensions.Logging;
using Microsoft.Extensions.Options;

namespace BH.WorkerService
{
    public class Worker : BackgroundService
    {
        private readonly ILogger<Worker> _logger;
        private readonly WorkerOptions _options;

        private FTService _fts;
        private static IEnumerable<IBaseRobot> _robots;
        
        private string CurrentDirectory => Path.GetDirectoryName(Assembly.GetEntryAssembly().Location);

        public Worker(IOptionsMonitor<WorkerOptions> options, ILogger<Worker> logger)
        {
            _options = options.CurrentValue;

            _logger = logger;
            
            //this.ServiceName = @"BH.FTSearch";
            //this.EventLog.Source = this.ServiceName;
            //this.EventLog.Log = "Application";

            AppDomain.CurrentDomain.UnhandledException += CurrentDomain_UnhandledException;
        }

        private void CurrentDomain_UnhandledException(object sender, UnhandledExceptionEventArgs e)
        {
            Exception ex = (Exception)e.ExceptionObject;

            WriteLog(ex.Message + ex.StackTrace, EventLogEntryType.Error);
        }

        protected override Task ExecuteAsync(CancellationToken stoppingToken)
        {
            StartService();

            return Task.CompletedTask;
        }

        private void StartService()
        {
            WriteLog("Start Service", EventLogEntryType.Information);

            //start web service
            try
            {
                _fts = new FTService(LoadDocumentContent,
                                          ex =>
                                          {
                                              WriteLog(ex.Message + ex.StackTrace, EventLogEntryType.Error);
                                          });

                var conf = _fts.GetDefaultConfiguration();

                var directory = _options.IndexPath;

                if (!Directory.Exists(directory))
                {
                    WriteLog($"IndexPath = {directory} in config file is not found.", EventLogEntryType.Warning);

                    directory = Path.Combine(CurrentDirectory, directory);

                    WriteLog($"IndexPath built as {directory}.", EventLogEntryType.Warning);
                }

                conf.SetIndexPath(directory);
                conf.LimitUsedMemory = _options.LimitUsedMemory;
                conf.MemoryMode = _options.InMemoryMode ? (uint)FTSearch.MemoryMode.InMemory : (uint)FTSearch.MemoryMode.HDD;

                _fts.SetConfiguration(conf);

                ThreadPool.QueueUserWorkItem
                (
                    x =>
                    {
                        WriteLog("Start FTS Service", EventLogEntryType.Information);

                        _fts.Start();

                        if (_options.BoobenMode)
                        {
                            StartBoobenRobots();
                        }
                        else
                        {
                            StartRobots();
                        }

                        if (_options.EnableRelSearch)
                        {
                            _fts.InitSearchRel();
                        }
                    }
                );

                RESTService.StartWebService(_fts);

                WriteLog("Service started.", EventLogEntryType.Information);
            }
            catch (Exception ex)
            {
                WriteLog(ex.Message + ex.StackTrace, EventLogEntryType.Error);

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
            var enableRobots = _options.EnableRobots;
            var indexCurrentMonth = _options.IndexCurrentMonth;

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

                    throw ex;
                }

                WriteLog("Job Started.", EventLogEntryType.Information);
            }
        }

        private void StartRobots()
        {
            if (_options.EnableRobots)
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

        //protected void OnStop()
        //{
        //    WriteLog("Stop Service", EventLogEntryType.Information);

        //    //start web service
        //    try
        //    {
        //        if (_robots != null)
        //        {
        //            foreach (var robot in _robots)
        //            {
        //                robot.Stop();
        //            }
        //        }

        //        FTService.StopWebService();

        //        _fts.Stop();

        //        WriteLog("Service stoped.", EventLogEntryType.Information);
        //    }
        //    catch (Exception ex)
        //    {
        //        WriteLog(ex.Message + ex.StackTrace, EventLogEntryType.Error);

        //        throw ex;
        //    }
        //}

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
                switch(eventType)
                {
                    case EventLogEntryType.Error:
                        {
                            _logger.LogError(message);
                            break;
                        }
                    case EventLogEntryType.Warning:
                        {
                            _logger.LogWarning(message);
                            break;
                        }
                    case EventLogEntryType.Information:
                        {
                            _logger.LogInformation(message);
                            break;
                        }
                    default:
                        throw new NotImplementedException();
                }
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
