using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading;
using System.IO;
using System.Text.RegularExpressions;
using System.Data.SqlClient;
using System.IO.Compression;
using System.Collections.Concurrent;
using BH.WCF;

namespace BH.BoobenRobot
{
    public partial class IndexService
    {
        private FTService _service;
        private Action<string> _logError;
        private Action<string> _logInfo;

        public IndexService(FTService service, Action<string> logError, Action<string> logInfo, bool debugMode = false)
        {
            _service = service;
            _logError = logError;
            _logInfo = logInfo;

            BH.BoobenRobot.Site.Sites = new List<Site>();

            BH.BoobenRobot.Site.Sites.Add(new PravdaSite(service));
            BH.BoobenRobot.Site.Sites.Add(new KhafSite(service));
            BH.BoobenRobot.Site.Sites.Add(new SearchengineSite(service));
            BH.BoobenRobot.Site.Sites.Add(new DouSite(service));
            BH.BoobenRobot.Site.Sites.Add(new GamedevSite(service));
            BH.BoobenRobot.Site.Sites.Add(new HabrSite(service));
            BH.BoobenRobot.Site.Sites.Add(new GotaiSite(service));
            BH.BoobenRobot.Site.Sites.Add(new SqlSite(service));
            BH.BoobenRobot.Site.Sites.Add(new SevasSite(service));
            BH.BoobenRobot.Site.Sites.Add(new SkcSite(service));
            BH.BoobenRobot.Site.Sites.Add(new VeloSite(service));
            BH.BoobenRobot.Site.Sites.Add(new IxbtSite(service));
            BH.BoobenRobot.Site.Sites.Add(new AutouaSite(service));
            BH.BoobenRobot.Site.Sites.Add(new FinSite(service));
            BH.BoobenRobot.Site.Sites.Add(new GratisSite(service));
            BH.BoobenRobot.Site.Sites.Add(new CyberSite(service));
        }

        private class IndexContent
        {
            public string AliasName;
            public string ContentText;
        }

        private ConcurrentQueue<IndexContent> _cq = new ConcurrentQueue<IndexContent>();
        private int _curr = 0;

        private void IndexFile(ZipArchive archive, int curr, SqlDataReader reader)
        {
            string file = Site.FTRobot_PATH + (string)reader["URL"];

            IndexContent ic = new IndexContent();

            ic.AliasName = Path.GetFileName(file).Replace(".txt", "");
            ic.ContentText = null;

            //read file
            if (archive != null)
            {
                string path = file.ToLower().Replace(Site.FTRobot_PATH.ToLower(), string.Empty);

                ZipArchiveEntry zipEntry = archive.GetEntry(path);

                if (zipEntry != null)
                {
                    using (StreamReader sr = new StreamReader(zipEntry.Open(), Archive.Encoding))
                    {
                        ic.ContentText = sr.ReadToEnd();
                    }
                }
            }
            else
            {
                if (File.Exists(file))
                {
                    ic.ContentText = File.ReadAllText(file, Archive.Encoding);
                }
                else
                {
                    ic.ContentText = null;
                }
            }

            //index file
            if (ic.ContentText != null)
            {
                while (_cq.Count > 10000)
                {
                    Thread.Sleep(100);
                }

                _cq.Enqueue(ic);
            }
        }

        private void IndexEntry(ZipArchiveEntry zipEntry, int curr)
        {

            string file = Site.FTRobot_PATH + zipEntry.FullName;

            IndexContent ic = new IndexContent();

            ic.AliasName = Path.GetFileName(file).Replace(".txt", "");
            ic.ContentText = null;

            //read file
            if (zipEntry != null)
            {
                using (StreamReader sr = new StreamReader(zipEntry.Open(), Archive.Encoding))
                {
                    ic.ContentText = sr.ReadToEnd();
                }
            }

            //index file
            if (ic.ContentText != null)
            {
                while (_cq.Count > 10000)
                {
                    Thread.Sleep(100);
                }

                _cq.Enqueue(ic);
            }
        }

        private void IndexEqueue()
        {
            while (true)
            {
                int count = 0;

                IndexContent ic;
                while (!_cq.TryDequeue(out ic))
                {
                    Thread.Sleep(100);

                    count++;

                    if (count > 100)
                        return;
                }

                if (ic.ContentText.Length > 65000)
                {
                    ic.ContentText = ic.ContentText.Substring(0, 65000);
                }

                _service.IndexText(ic.AliasName, null, ic.ContentText, null);

                if (_curr % 100 == 0)
                {
                    var text = "Index files: " + _curr.ToString() + " of ???";

                    _logInfo(text);

                    File.WriteAllText(Site.FTRobot_PATH + "stat.txt", text);
                }

                _curr++;
            }
        }

        private void IndexFiles()
        {
            var info = _service.GetInfo();

            if (info.LastNameIDRAM <= 80) //empty instance
            {
                /*
                string logPath = BH.BoobenRobot.Site.FTRobot_PATH + "Log.txt";

                if (File.Exists(logPath))
                {
                    StreamReader sr = new StreamReader(logPath);

                    //read files
                    List<string> files = new List<string>();

                    while (!sr.EndOfStream)
                    {
                        string file = sr.ReadLine();
                        files.Add(file);
                    }

                    sr.Close();

                    //distinct files
                    List<string> disFiles = new List<string>();

                    Dictionary<string, bool> dic = new Dictionary<string, bool>();

                    for (int i = files.Count - 1; i >= 0; i--)
                    {
                        string file = files[i];

                        if (!dic.ContainsKey(file))
                        {
                            disFiles.Insert(0, file);

                            dic.Add(file, true);
                        }
                    }

                    StringBuilder log = new StringBuilder();

                    //index files
                    int count = 1;
                    foreach (string file1 in disFiles)
                    {
                        string file = file1.Replace("ixbt-", "ixbt_");
                        
                        if (File.Exists(file))
                        {
                            string aliasName = Path.GetFileName(file).Replace(".txt", "");
                            string contentText = File.ReadAllText(file, BH.BoobenRobot.Site.ASC2Encoding);

                            //string[] parts = contentText.Split(new char[] { '.', '?', '!' });
                            //foreach (string part in parts)
                            //{
                            //    if (part.Trim().Length > 3 && part.Trim().Length < 65535)
                            //    {

                            if (contentText.Length > 65000)
                            {
                                contentText = contentText.Substring(0, 65000);
                            }

                            client.IndexText(aliasName, contentText);
                            
                            //    }
                            //}

                            if (count % 100 == 0)
                            {
                                tbLog.Text = "Index files: " + count.ToString() + " of " + disFiles.Count.ToString();
                                tbLog.Update();

                                Application.DoEvents();
                            }

                            count++;

                            //save new log
                            log.AppendLine(file);
                        }
                    }

                    //save log
                    File.WriteAllText(logPath, log.ToString());
                }
                */

                //index previous periods
                DateTime startDate;

                ThreadPool.QueueUserWorkItem(x =>
                {
                    startDate = new DateTime(2015, 6, 1);

                    while (true)
                    {
                        string zipPath;

                        for (int archiveIndex = 1; ; archiveIndex++)
                        {
                            if (archiveIndex == 1)
                            {
                                zipPath = Site.FTRobot_PATH + startDate.Year.ToString("0000") + startDate.Month.ToString("00") + ".zip";
                            }
                            else
                            {
                                zipPath = Site.FTRobot_PATH + startDate.Year.ToString("0000") + startDate.Month.ToString("00") + "_" + archiveIndex.ToString() + ".zip";
                            }

                            if (File.Exists(zipPath))
                            {
                                using (FileStream zipToOpen = new FileStream(zipPath, FileMode.Open))
                                {
                                    using (ZipArchive archive = new ZipArchive(zipToOpen, ZipArchiveMode.Read))
                                    {
                                        DBHelpers.GetPages(IndexFile, archive, startDate, startDate.AddMonths(1));
                                    }
                                }
                            }
                            else
                            {
                                break;
                            }
                        }

                        if (startDate < new DateTime(DateTime.Now.Year, DateTime.Now.Month, 1))
                        {
                            startDate = startDate.AddMonths(1);
                        }
                        else
                        {
                            break;
                        }
                    }
                });

                Thread.Sleep(100);

                IndexEqueue();

                ////index current period
                //startDate = new DateTime(DateTime.Now.Year, DateTime.Now.Month, 1);

                //DBHelpers.GetPages(IndexFile, null, startDate, startDate.AddMonths(1));

                //save index
                //client.SaveIndex();
            }
        }

        private void UpdateStat(object data)
        {
            while (true)
            {
                this.UpdateStat();

                Thread.Sleep(TimeSpan.FromSeconds(15));
            }
        }

        private void UpdateStat()
        {
            try
            {
                string tbLogText = string.Empty;

                foreach (Site site in BH.BoobenRobot.Site.Sites)
                {
                    tbLogText += "======== ONLINE SPIDER INFO FOR " + site.Code + "========;\r\n";
                    tbLogText += "TotalSizeHTML : " + site.TotalSize.ToString() + " bytes;\r\n";
                    tbLogText += "AmountPages: " + site.AmountPages.ToString() + ";\r\n";
                    tbLogText += "NextUpdate: " + site.NextUpdate.ToString() + ";\r\n";

                    if (site.CurrentPage != null)
                    {
                        tbLogText += "Progress: " + site.Progress + ";\r\n";
                        tbLogText += site.CurrentPage.ToString() + ";\r\n";
                    }

                    if (site.LastErrorMessage != null)
                    {
                        tbLogText += "LastErrorMessage: " + site.LastErrorMessage + ";\r\n";
                    }
                }

                tbLogText += "========== ONLINE DATABASE INFO =============;\r\n";

                FTService.Info info;

                try
                {
                    info = _service.GetInfo();

                    tbLogText += "Version: " + info.Version.ToString() + ";\r\n" +
                                  "UsedMemory: " + info.UsedMemory.ToString() + ";\r\n" +
                                  "TotalMemory: " + info.TotalMemory.ToString() + ";\r\n" +
                                  "LastNameIDRAM: " + info.LastNameIDRAM.ToString() + ";\r\n" +
                                  "LastNameIDHDD: " + info.LastNameIDHDD.ToString() + ";\r\n" +
                                  "CountWordsRAM: " + info.CountWordsRAM.ToString() + ";\r\n" +
                                  "CountWordsHDD: " + info.CountWordsHDD.ToString() + ";\r\n" +
                                  "DocumentNameSize: " + info.DocumentNameSize.ToString() + ";\r\n" +
                                  "WordsHeaderBase: " + info.WordsHeaderBase.ToString() + ";\r\n" +
                                  "LastErrorMessage: " + info.LastErrorMessage.ToString() + ";\r\n";
                }
                catch (Exception ex)
                {
                    tbLogText += "LastErrorMessage: " + ex.Message + ";\r\n";
                }

                _logInfo(tbLogText);

                File.WriteAllText(Site.FTRobot_PATH + "stat.txt", tbLogText);
            }
            catch
            {
            }
        }

        private bool _isSaved = false;

        private void Save()
        {
            try
            {
                if (!_isSaved)
                {
                    //save index
                    //FTSearchService7.FTSearchServiceClient client = new FTSearchService7.FTSearchServiceClient();
                    //client.SaveIndex();

                    //foreach (Site site in sites)
                    //{
                    //    site.SaveLabels();
                    //}

                    _isSaved = true;
                }
            }
            catch (Exception ex)
            {
                _logError(ex.Message);
            }
        }

        private void ProcessPage(ZipArchive archive, int curr, SqlDataReader reader)
        {
            while (true)
            {
                int year;
                int month;

                string zipPath;

                while (true)
                {
                    year = ((DateTime)reader["OnDate"]).Year;
                    month = ((DateTime)reader["OnDate"]).Month;

                    zipPath = Site.FTRobot_PATH + year.ToString("0000") + month.ToString("00");

                    if (File.Exists(zipPath))
                    {
                        if (!reader.Read())
                        {
                            return;
                        }
                    }
                    else
                    {
                        break;
                    }
                }

                int archiveIdx = 1;
                int portion = 200000;
                string fullZipPath = null;

                List<string> filePaths = new List<string>();

                while (true)
                {
                    string url = (string)reader["URL"];
                    DateTime onDate = (DateTime)reader["OnDate"];

                    if (month != onDate.Month)
                        break;

                    string file = url;

                    filePaths.Add(file);

                    if (archiveIdx == 1)
                    {
                        fullZipPath = zipPath + ".zip";
                    }
                    else
                    {
                        fullZipPath = zipPath + "_" + archiveIdx.ToString() + ".zip";
                    }

                    if (filePaths.Count >= portion)
                    {
                        Archive.AddFiles(fullZipPath, Site.FTRobot_PATH, filePaths.ToArray(), null, AddLog);

                        filePaths.Clear();

                        archiveIdx++;
                    }

                    if (!reader.Read())
                    {
                        Archive.AddFiles(fullZipPath, Site.FTRobot_PATH, filePaths.ToArray(), null, AddLog);

                        return;
                    }
                }

                Archive.AddFiles(fullZipPath, Site.FTRobot_PATH, filePaths.ToArray(), null, AddLog);
            }
        }

        private void AddLog(int i, int j)
        {
            var text = string.Format("File {0} of {1}", i, j);

            _logInfo(text);
        }

        public void ArchivePrevMonth()
        {
            var dt = DateTime.Now.AddMonths(-1);

            DateTime startDate = new DateTime(dt.Year, dt.Month, 1);

            while (startDate < DateTime.Now)
            {
                DateTime endDate = startDate.AddMonths(1).AddSeconds(-1);

                DBHelpers.GetPages(ProcessPage, null, startDate, endDate);

                startDate = startDate.AddMonths(1);

                break;
            }
        }

        public void Run()
        {
            //IndexPrevMonth();
            //ArchivePrevMonth();
            //return;

            //index files
            //IndexFiles();

            //index current period
            //ThreadPool.QueueUserWorkItem(x =>
            //{
            //    var startDate = new DateTime(DateTime.Now.Year, DateTime.Now.Month, 1);

            //    DBHelpers.GetPages(IndexFile, null, startDate, startDate.AddMonths(1));
            //});

            //Thread.Sleep(500);

            //this.IndexEqueue();

            _service.InitSearchRel();

            //try
            //{
            //    //run spiders
            //    foreach (Site site in BH.BoobenRobot.Site.Sites)
            //    {
            //        //site.LoadLabels();

            //        ThreadPool.QueueUserWorkItem(x => site.Run());
            //    }

            //    //refresh interface
            //    UpdateStat();

            //    ThreadPool.QueueUserWorkItem(new WaitCallback(this.UpdateStat));
            //}
            //catch (Exception ex)
            //{
            //    _logError(ex.Message);
            //}
        }

        private void IndexPrevMonth()
        {
            //index previous periods
            DateTime baseDate = new DateTime(2016, 11, 1); //DateTime.Now.AddMonths(-1); //baseDate.AddMonths(16);

            DateTime startDate = new DateTime(baseDate.Year, baseDate.Month, 1);
            DateTime endDate = new DateTime(DateTime.Now.Year, DateTime.Now.Month, 1); //startDate.AddMonths(1);

            ThreadPool.QueueUserWorkItem(x =>
            {
                while (true)
                {
                    string zipPath;

                    for (int archiveIndex = 1; ; archiveIndex++)
                    {
                        if (archiveIndex == 1)
                        {
                            zipPath = Site.FTRobot_PATH + startDate.Year.ToString("0000") + startDate.Month.ToString("00") + ".zip";
                        }
                        else
                        {
                            zipPath = Site.FTRobot_PATH + startDate.Year.ToString("0000") + startDate.Month.ToString("00") + "_" + archiveIndex.ToString() + ".zip";
                        }

                        if (File.Exists(zipPath))
                        {
                            using (FileStream zipToOpen = new FileStream(zipPath, FileMode.Open))
                            {
                                using (ZipArchive archive = new ZipArchive(zipToOpen, ZipArchiveMode.Read))
                                {
                                    foreach (var ent in archive.Entries)
                                    {
                                        IndexEntry(ent, 0);
                                        DBHelpers.SavePage("", ent.FullName);
                                    }

                                    //DBHelpers.GetPages(IndexFile, archive, startDate, startDate.AddMonths(1));
                                }
                            }

                            break;
                        }
                        else
                        {
                            break;
                        }
                    }

                    startDate = startDate.AddMonths(1);

                    if (startDate >= endDate)
                    {
                        break;
                    }
                }
            });

            Thread.Sleep(100);

            IndexEqueue();

            ////index current period
            //startDate = new DateTime(DateTime.Now.Year, DateTime.Now.Month, 1);

            //DBHelpers.GetPages(IndexFile, null, startDate, startDate.AddMonths(1));

            //save index
            _service.SaveIndex();
        }
    }
}
