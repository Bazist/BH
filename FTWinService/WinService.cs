﻿using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Diagnostics;
using System.Linq;
using System.ServiceProcess;
using System.Text;
using System.Threading.Tasks;
using System.Threading;
using FTServiceWCF;
using System.IO;
using System.Configuration;
using FTBoobenRobot;

namespace FTWinService
{
    public partial class WinService : ServiceBase
    {
        public WinService()
        {
#if !DEBUG
            InitializeComponent();

            this.ServiceName = @"BH.FTSearch";
            this.EventLog.Source = this.ServiceName;
            this.EventLog.Log = "Application";

            AppDomain.CurrentDomain.UnhandledException += CurrentDomain_UnhandledException;
#endif
        }

        private void CurrentDomain_UnhandledException(object sender, UnhandledExceptionEventArgs e)
        {
            Exception ex = (Exception)e.ExceptionObject;

            WriteLog(ex.Message + ex.StackTrace, EventLogEntryType.Error);
        }

        private static FTService _fts = new FTService();
        private static string _indexedArchives = string.Empty;
        //private static long _readBytes = 0;
        //private static int _amountDocuments = 0;
        private static Thread _job;

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

                conf.SetIndexPath(ConfigurationManager.AppSettings["IndexPath"]);
                conf.LimitUsedMemory = ulong.Parse(ConfigurationManager.AppSettings["LimitUsedMemory"]);

                _fts.SetConfiguration(conf);

                _fts.Start();

                FTService.StartWebservice(_fts,
                                          ConfigurationManager.AppSettings["URL"],
                                          ex => {
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

        private void StartJob()
        {
            if (bool.Parse(ConfigurationManager.AppSettings["EnableJob"]))
            {
                WriteLog("Start Job", EventLogEntryType.Information);

                _job = new Thread(() =>
                {
                    try
                    {
                        IndexService indexService = new IndexService(_fts,
                                                                     x => WriteLog(x, EventLogEntryType.Error),
                                                                     x => Debug.WriteLine(x));

                        var jobType = ConfigurationManager.AppSettings["JobType"];

                        switch (jobType)
                        {
                            case "RunEverydayAtTime":
                                {
                                    var parts = ConfigurationManager.AppSettings["StartJobTime"].Split(':');

                                    DateTime dt = new DateTime(DateTime.Now.Year,
                                                               DateTime.Now.Month,
                                                               DateTime.Now.Day,
                                                               int.Parse(parts[0]),
                                                               int.Parse(parts[1]),
                                                               0);

                                    while (true)
                                    {
                                        if (DateTime.Now >= dt)
                                        {
                                            WriteLog("Start indexing ...", EventLogEntryType.Information);

                                            indexService.Run();

                                            WriteLog("Indexing completed.", EventLogEntryType.Information);

                                            dt = dt.AddDays(1);
                                        }
                                        else
                                        {
                                            Thread.Sleep(30000);
                                        }
                                    }

                                    break;
                                }
                            case "RunOnce":
                                {
                                    indexService.Run();

                                    break;
                                }
                            default:
                                throw new Exception(jobType + " is not found");
                        }
                    }
                    catch (Exception ex)
                    {
                        WriteLog(ex.Message + ex.StackTrace, EventLogEntryType.Error);

                        Stop();

                        throw ex;
                    }
                }
                );

                _job.Start();

                WriteLog("Job Started.", EventLogEntryType.Information);
            }
        }

        protected override void OnStart(string[] args)
        {
            StartService();

            StartJob();
        }

        protected override void OnStop()
        {
            WriteLog("Stop Service", EventLogEntryType.Information);

            //start web service
            try
            {
                if (_job != null)
                {
                    _job.Abort();
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
            message = $"[{DateTime.Now.ToString()}] {message}; Memory: {System.Diagnostics.Process.GetCurrentProcess().WorkingSet64 / 1024 / 1024} mb.";
#if DEBUG
            Trace.WriteLine(message);
#else
            ((ISupportInitialize)(this.EventLog)).BeginInit();
            if (!EventLog.SourceExists(this.EventLog.Source))
            {
                EventLog.CreateEventSource(this.EventLog.Source, this.EventLog.Log);
            }
            ((ISupportInitialize)(this.EventLog)).EndInit();

            this.EventLog.WriteEntry(message, eventType);
#endif
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
