using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using System.IO;
using System.Diagnostics;
using System.IO.Compression;
using System.Threading;
using BH.WCF;
using BH.BoobenRobot;

namespace BH.Tests
{
    public partial class MainForm : Form
    {
        public MainForm()
        {
            InitializeComponent();
        }

        private long readBytes;
        private int amountDocuments;

        private Stopwatch readTime = new Stopwatch();
        private Stopwatch indexedTime = new Stopwatch();
        private Stopwatch totalTime = new Stopwatch();

        private FTService fts = null;// new FTService();

        private void UpdateScanSpeed()
        {
            if (totalTime.ElapsedMilliseconds > 0)
            {
                int speedBytes = (int)(readBytes * 1000 / (totalTime.ElapsedMilliseconds - readTime.ElapsedMilliseconds - indexedTime.ElapsedMilliseconds));

                if (speedBytes > this.pbScanSpeed.Maximum)
                {
                    this.pbScanSpeed.Maximum = speedBytes;

                    if (speedBytes < 1024)
                    {
                        lbScanSpeedMin.Text = "0 bs";
                        lbScanSpeedMax.Text = speedBytes.ToString() + " bs";
                    }
                    else if (speedBytes < 1024 * 1024)
                    {
                        lbScanSpeedMin.Text = "0 kb";
                        lbScanSpeedMax.Text = (speedBytes / 1024).ToString() + " kb";
                    }
                    else
                    {
                        lbScanSpeedMin.Text = "0 mb";
                        lbScanSpeedMax.Text = (speedBytes / 1024 / 1024).ToString() + " mb";
                    }
                }

                this.pbScanSpeed.Value = speedBytes;

                this.pbScanSpeed.Update();
            }
        }

        private void UpdateIOSpeed()
        {
            if (readTime.ElapsedMilliseconds > 0)
            {
                int speedBytes = (int)(readBytes * 1000 / readTime.ElapsedMilliseconds);

                if (speedBytes > this.pbIOSpeed.Maximum)
                {
                    this.pbIOSpeed.Maximum = speedBytes;

                    if (speedBytes < 1024)
                    {
                        lbIOSpeedMin.Text = "0 bs";
                        lbIOSpeedMax.Text = speedBytes.ToString() + " bs";
                    }
                    else if (speedBytes < 1024 * 1024)
                    {
                        lbIOSpeedMin.Text = "0 kb";
                        lbIOSpeedMax.Text = (speedBytes / 1024).ToString() + " kb";
                    }
                    else
                    {
                        lbIOSpeedMin.Text = "0 mb";
                        lbIOSpeedMax.Text = (speedBytes / 1024 / 1024).ToString() + " mb";
                    }
                }

                this.pbIOSpeed.Value = speedBytes;

                this.pbIOSpeed.Update();
            }
        }

        private void UpdateIndexSpeed()
        {
            if (indexedTime.ElapsedMilliseconds > 0)
            {
                int speedBytes = (int)(readBytes * 1000 / indexedTime.ElapsedMilliseconds);

                if (speedBytes > this.pbIndexSpeed.Maximum)
                {
                    this.pbIndexSpeed.Maximum = speedBytes;

                    if (speedBytes < 1024)
                    {
                        lbIndexSpeedMin.Text = "0 bs";
                        lbIndexSpeedMax.Text = speedBytes.ToString() + " bs";
                    }
                    else if (speedBytes < 1024 * 1024)
                    {
                        lbIndexSpeedMin.Text = "0 kb";
                        lbIndexSpeedMax.Text = (speedBytes / 1024).ToString() + " kb";
                    }
                    else
                    {
                        lbIndexSpeedMin.Text = "0 mb";
                        lbIndexSpeedMax.Text = (speedBytes / 1024 / 1024).ToString() + " mb";
                    }
                }

                this.pbIndexSpeed.Value = speedBytes;

                this.pbIndexSpeed.Update();
            }
        }

        private void UpdateAmount()
        {
            if (readBytes < 1024)
            {
                lbAmountFiles.Text = string.Format("Amount documents: {0} \t/ Total size: {1} {2}", amountDocuments.ToString(), readBytes, "bt");
            }
            else if (readBytes < 1024 * 1024)
            {
                lbAmountFiles.Text = string.Format("Amount documents: {0} \t/ Total size: {1} {2}", amountDocuments.ToString(), readBytes / 1024, "kb");
            }
            else if (readBytes < 1024 * 1024 * 1024)
            {
                lbAmountFiles.Text = string.Format("Amount documents: {0} \t/ Total size: {1} {2}", amountDocuments.ToString(), readBytes / 1024 / 1024, "mb");
            }
            else
            {
                lbAmountFiles.Text = string.Format("Amount documents: {0} \t/ Total size: {1} {2}", amountDocuments.ToString(), (double)(readBytes / 1024 / 1024) / 1024, "gb");
            }

            lbAmountFiles.Update();
        }

        private void UpdateTotalSpeed()
        {
            if (totalTime.ElapsedMilliseconds > 0)
            {
                int speedBytes = (int)(readBytes * 1000 / totalTime.ElapsedMilliseconds);

                if (speedBytes > this.pbTotalSpeed.Maximum)
                {
                    this.pbTotalSpeed.Maximum = speedBytes;

                    if (speedBytes < 1024)
                    {
                        lbTotalSpeedMin.Text = "0 bs";
                        lbTotalSpeedMax.Text = speedBytes.ToString() + " bs";
                    }
                    else if (speedBytes < 1024 * 1024)
                    {
                        lbTotalSpeedMin.Text = "0 kb";
                        lbTotalSpeedMax.Text = (speedBytes / 1024).ToString() + " kb";
                    }
                    else
                    {
                        lbTotalSpeedMin.Text = "0 mb";
                        lbTotalSpeedMax.Text = (speedBytes / 1024 / 1024).ToString() + " mb";
                    }
                }

                this.pbTotalSpeed.Value = speedBytes;

                this.pbTotalSpeed.Update();
            }
        }

        private void UpdateFolder(string folder)
        {
            lbIndexFolder.Text = folder;
            lbIndexFolder.Update();
        }

        private void UpdateInstanceInfo(FTService fts)
        {
            var info = fts.GetInfo();

            lbInstanceInfo.Text = "Version: " + info.Version.ToString() + "\n" +
                                  "UsedMemory: " + info.UsedMemory.ToString() + "\n" +
                                  "TotalMemory: " + info.TotalMemory.ToString() + "\n" +
                                  "LastNameIDRAM: " + info.LastNameIDRAM.ToString() + "\n" +
                                  "LastNameIDHDD: " + info.LastNameIDHDD.ToString() + "\n" +
                                  "CountWordsRAM: " + info.CountWordsRAM.ToString() + "\n" +
                                  "CountWordsHDD: " + info.CountWordsHDD.ToString() + "\n" +
                                  //"DocumentNameSize: " + info.DocumentNameSize.ToString() + "\n" +
                                  "WordsHeaderBase: " + info.WordsHeaderBase.ToString() + "\n" +
                                  "LastErrorMessage: " + info.LastErrorMessage.ToString() + "\n";


            lbInstanceInfo.Update();

            if (info.HasError)
            {
                throw new ApplicationException(info.LastErrorMessage);
            }
        }

        private bool IndexFile(string name, string text)
        {
            bool res = false;

            if (name.Length > FTSearch.MAX_DOC_NAME_LENGTH - 1)
            {
                name = name.Substring(0, FTSearch.MAX_DOC_NAME_LENGTH - 1);
            }

            try
            {


                //dou
                //text = GetMessages("<article class=\"b-typo\">", "</article>", text) +
                //       GetMessages("<div class=\"text b-typo\">", "</div>", text);

                //habr
                //text = GetMessages("<div class=\"content html_format\">", "</div>", "div", text) +
                //       GetMessages("<div class=\"message html_format \">", "</div>", "div", text);

                //search
                //text = GetMessages("<div id=\"post_message", "</div>", "div", text);

                //skoda
                //text = GetMessages("<div id=\"pid_", "</div>", "div", text);

                //game
                //text = GetMessages("<div class=\"block\">", "</div>", "div", text);

                //fxclub
                //text = GetMessages("<div class=\"content\">", "</div>", "div", text);

                //sevinfo
                //text = GetMessages("<div class=\"postbody\">", "</div>", "div", text);

                //text = RemoveTagBlock(text, "<title", "</title>");
                //text = RemoveTagBlock(text, "<script", "</script>");

                //int idx1 = text.IndexOf("<body>", StringComparison.OrdinalIgnoreCase);
                //int idx2 = text.IndexOf("</body>", StringComparison.OrdinalIgnoreCase);

                //if (idx1 > 0 && idx2 > 0)
                //{
                //    text = text.Substring(idx1, idx2 - idx1);
                //}
                //else
                //{
                //    text = "";
                //    readTime.Stop();

                //    continue;
                //}

                //if (text.IndexOf("К сожалению, запрошенная вами тема не найдена.") >= 0)
                //{
                //    readTime.Stop();
                //    continue;
                //}

                //contentBytes = FTSearch.GetContentBytes(text);

                //File.Delete(fi.FullName);

                //FileStream fs = new FileStream(fi.FullName, FileMode.CreateNew);
                //fs.Write(contentBytes, 0, contentBytes.Length);
                //fs.Close();

                readBytes += text.Length;
            }
            catch (Exception ex)
            {
                lbIndexFolder.Text = ex.Message;
            }
            finally
            {

                //if (sr != null)
                //{
                //    sr.Close();
                //}

            }

            amountDocuments += 1;

            //string alias = "dou-" + fi.Name.Split('.')[0].Split('_')[1]; //tid.ToString("00000000") + "-" + pg.ToString("00000");
            //string alias = "lib-" + fi.Name.Split('.')[0]; //tid.ToString("00000000") + "-" + pg.ToString("00000");

            //string alias = "skc-" + fi.Name.Split('_')[1] + "-" + fi.Name.Split('_')[3]; //tid.ToString("00000000") + "-" + pg.ToString("00000");

            //index
            indexedTime.Start();

            try
            {
                if (!string.IsNullOrEmpty(text))
                {
                    res = fts.IndexText(name, null, text, null);
                }
            }
            catch (Exception ex)
            {
                lbIndexFolder.Text = ex.Message;
            }

            indexedTime.Stop();

            this.UpdateFolder(name);
            this.UpdateScanSpeed();
            this.UpdateIOSpeed();
            this.UpdateIndexSpeed();
            this.UpdateAmount();
            this.UpdateTotalSpeed();

            this.UpdateInstanceInfo(fts);

            this.Refresh();

            Application.DoEvents();

            return res;
        }

        private bool IndexFiles(FTService fts, string rootPath, string currPath)
        {
            bool res = false;

            DirectoryInfo di = new DirectoryInfo(currPath);
            foreach (DirectoryInfo subDi in di.GetDirectories("*", SearchOption.AllDirectories))
            {
                res |= IndexFiles(fts, rootPath, subDi.FullName);
            }

            foreach (FileInfo fi in di.GetFiles())
            {
                string extension = fi.Extension.ToLower();

                //if(extension == ".zip")
                //{
                //    using (FileStream zipToOpen = new FileStream(fi.FullName, FileMode.Open))
                //    {
                //        using (ZipArchive archive = new ZipArchive(zipToOpen, ZipArchiveMode.Read))
                //        {
                //            foreach (ZipArchiveEntry zipEntry in archive.Entries)
                //            {
                //                using (StreamReader sr = new StreamReader(zipEntry.Open(), Encoding.UTF8))
                //                {
                //                    readTime.Start();

                //                    string text = sr.ReadToEnd();

                //                    readTime.Stop();

                //                    this.IndexFile(fi.FullName + "\\" + zipEntry.FullName, text);
                //                }

                //                GC.Collect();
                //            }
                //        }
                //    }
                //}
                //else 
                if ((extension == ".txt" || extension == ".html" ||
                          extension == ".cs" || extension == ".vb" ||
                          extension == ".cpp" || extension == ".fb2" ||
                          extension == ".log" || extension == ".csv" ||
                          extension == ".xml"))
                {
                    long maxSize = 1024 * 1024 * 10;

                    string fullName = fi.FullName.Replace(rootPath, "");

                    if (fi.Length < maxSize) //10mb
                    {
                        readTime.Start();

                        string text = FTSearch.ReadFileText(fi.FullName);

                        readTime.Stop();

                        res |= this.IndexFile(fullName, text);
                    }
                    else
                    {
                        using (StreamReader sr = new StreamReader(fi.FullName))
                        {
                            for (long i = 0; i < fi.Length; i += maxSize)
                            {
                                long size;

                                if (i + maxSize < fi.Length)
                                {
                                    size = maxSize;
                                }
                                else
                                {
                                    size = fi.Length - i;
                                }

                                char[] buffer = new char[size + 1];

                                readTime.Start();

                                sr.Read(buffer, 0, (int)size);

                                readTime.Stop();

                                res |= this.IndexFile(fullName, new string(buffer));
                            }
                        }
                    }
                }
            }

            return res;
        }

        static void Unpack(string src, string dest)
        {
            var proc = new System.Diagnostics.Process();
            proc.StartInfo = new System.Diagnostics.ProcessStartInfo();
            proc.StartInfo.FileName = @"C:\Program Files\7-Zip\7z.exe";
            proc.StartInfo.Arguments = " x " + src + " -o\"" + dest + "\" -r";
            proc.StartInfo.UseShellExecute = false;
            proc.StartInfo.CreateNoWindow = true;
            proc.StartInfo.WindowStyle = ProcessWindowStyle.Hidden;
            //proc.StartInfo.Verb = "runas";

            proc.Start();
            proc.WaitForExit();

            //Console.WriteLine(proc.StandardError.ReadToEnd());
            //Console.WriteLine(proc.StandardOutput.ReadToEnd());
        }

        private string indexedArchives = string.Empty;

        private void Index()
        {
            totalTime.Start();

            readBytes = 0;

            indexedTime = new Stopwatch();

            //string[] folders = { @"d:\Part0", @"d:\Part1", @"d:\Part2", @"d:\Part3", @"d:\Part4", @"d:\Part5", @"d:\Part6", @"d:\Part7", @"d:\Part8" };
            //string[] folders = { @"J:\Librusec\_Lib.rus.ec - Официальная\lib.rus.ec\" };
            //string[] folders = {@"C:\FTS\FVMas_Pages"};

            //string archive = @"C:\FTS\b2blogs\temp.7z";
            //string unarchive = @"C:\FTS\b2blogs\temp";

            //var files = Directory.GetFiles(@"N:\Production-Logs\");

            //foreach(var file in files)
            //{
            //    File.Copy(file, archive);

            //    string zPath = @"C:\Program Files\7-Zip\7zG.exe";// change the path and give yours 
            //    try
            //    {
            //        ProcessStartInfo pro = new ProcessStartInfo();
            //        pro.WindowStyle = ProcessWindowStyle.Hidden;
            //        pro.FileName = zPath;
            //        pro.Arguments = "x \"" + archive + "\" -o" + unarchive;
            //        Process x = Process.Start(pro);
            //        x.WaitForExit();

            //        this.IndexFiles(fts, unarchive);

            //        File.Delete(archive);
            //        Directory.Delete(unarchive);
            //    }
            //    catch (System.Exception Ex)
            //    {
            //        //DO logic here 
            //    }
            //}

            //return;

            string srcPath = @"\\srv3\fs\Production-Logs\";
            string destPath = @"C:\FTS\Logs\";
            string logPath = @"C:\FTS\archives.txt";

            string logText = string.Empty;

            if (File.Exists(logPath))
            {
                logText = File.ReadAllText(logPath);
            }

            var archives = Directory.GetFiles(srcPath, "*WS*.7z", SearchOption.AllDirectories).Concat(
                           Directory.GetFiles(srcPath, "*BS*.7z", SearchOption.AllDirectories)).ToList();

            //N:\Production-Logs
            foreach (string archive in archives)
            {
                try
                {
                    if (logText.Contains(archive))
                        continue;

                    lbArchive.Text = "Archive " + (archives.IndexOf(archive) + 1).ToString() + " of " + archives.Count.ToString();

                    string path = archive.Replace(srcPath, destPath);

                    string dirName = Path.GetDirectoryName(path);
                    if(!Directory.Exists(dirName))
                    {
                        Directory.CreateDirectory(dirName);
                    }

                    if (!File.Exists(path))
                    {
                        File.Copy(archive, path, true);
                    }

                    string rootPath = destPath + @"Unpacked\";

                    string unpackPath = rootPath + Path.GetFileName(archive).Replace(".7z", "");

                    if (!Directory.Exists(unpackPath))
                    {
                        Unpack(path, unpackPath);
                    }

                    indexedArchives += archive + "\r\n";

                    if (this.IndexFiles(fts, rootPath, unpackPath)) //need save on disc
                    {
                        fts.SaveIndex();

                        File.AppendAllText(logPath, indexedArchives + "\r\n");

                        indexedArchives = string.Empty;
                    }

                    Directory.Delete(unpackPath, true);

                    File.Delete(path);
                    
                    //File.AppendAllText(logPath, archive + "\r\n");
                }
                catch(Exception ex)
                {
                    if (MessageBox.Show(ex.Message + " Continue ?", "file", MessageBoxButtons.OKCancel) != DialogResult.OK)
                        break;
                }

                //break;
            }

            fts.SaveIndex();

            File.AppendAllText(logPath, indexedArchives + "\r\n");

            ////StringBuilder name = new StringBuilder("doc1");
            ////StringBuilder str = new StringBuilder("киевский политихнический институт");

            ////FTServer.indexText(name, str, str.Length);

            //fts.SaveIndex();

            //fts.StopInstance();

            //FTServer.startInstance();

            //FTServer.RelevantResult rr = FTServer.searchPhrase("капитан ушел обедать");

            //FTServer.saveIndex();

            //this.UpdateInstanceInfo();

            totalTime.Stop();
        }

        private void btnIndex_Click(object sender, EventArgs e)
        {
            //fts.GetConfiguration().LimitUsedMemory = 500000000;

            fts.Start();

            Index();

            fts.Stop();
        }

        private void MainForm_Load(object sender, EventArgs e)
        {
            //InitReadConsole();

            //ThreadPool.QueueUserWorkItem(x =>
            //           {
            //               while (true)
            //               {
            //                   ReadConsole();
            //                   Thread.Sleep(3000);
            //               }
            //           });

            //fts.SetConfiguration(FTService.GetDefaultConfiguration());

            //FTService.StartWebservice(fts, "http://localhost:8080");
        }

        private void button7_Click(object sender, EventArgs e)
        {

        }

        private void button6_Click(object sender, EventArgs e)
        {

        }

        private void button2_Click(object sender, EventArgs e)
        {
            if (!fts.IsStarted())
            {
                var conf = fts.GetConfiguration();
                conf.MemoryMode = (uint)FTSearch.MemoryMode.HDD;
                fts.SetConfiguration(conf);

                fts.Start();
            }

            fts.Stop();

            fts.Start();

            this.UpdateInstanceInfo(fts);

            var result = fts.SearchPhrase(tbSearchText.Text, string.Empty, 0, 25);

            rtbLog.Clear();

            StringBuilder sb = new StringBuilder();

            foreach (var res in result.Results)
            {
                sb.Append(res.Name).Append("\r\n");
            }

            rtbLog.Text = sb.ToString();
        }

        MemoryStream msConsole = new MemoryStream();
        StreamWriter swConsole;
        StreamReader srConsole;

        private void InitReadConsole()
        {
            swConsole = new StreamWriter(msConsole);
            srConsole = new StreamReader(msConsole);

            Console.SetOut(swConsole);
        }

        private void ReadConsole()
        {
            swConsole.Flush();

            if (msConsole.Length > 500)
            {
                msConsole.Position = msConsole.Length - 500;
            }
            else
            {
                msConsole.Position = 0;
            }

            string text = srConsole.ReadToEnd();

            rtbLog.Invoke(new Action(() => rtbLog.Text = text));
        }

        private void btnImportIndexes_Click(object sender, EventArgs e)
        {
            fts.Stop();

            fts.MergeIndexes();
        }

        private void btnCheckIndexes_Click(object sender, EventArgs e)
        {
            fts.Stop();

            fts.CheckIndexes();
        }

        private void btnStartJob_Click(object sender, EventArgs e)
        {
            fts.Start();
            
            IndexFiles(fts, @"C:\FTS\Logs", @"C:\FTS\Logs");

            

            fts.SaveIndex();

            IndexFiles(fts, @"C:\FTS\Logs", @"C:\FTS\Logs");

            fts.SaveIndex();

            fts.Stop();
            //ThreadPool.QueueUserWorkItem(x =>
            //{
            //    DateTime dt = new DateTime(DateTime.Now.Year, DateTime.Now.Month, DateTime.Now.Day, 3, 0, 0);

            //    while (true)
            //    {
            //        if(DateTime.Now >= dt)
            //        {
            //            btnIndex_Click(null, null);

            //            dt = dt.AddDays(1);
            //        }
            //        else
            //        {
            //            Thread.Sleep(1000);
            //        }
            //    }
            //}
            //);
        }

        private void button3_Click(object sender, EventArgs e)
        {
            //fts.Start();

            //fts.ClearInstance();

            var xx = BaseRobot.RobotFactory.CreateFactory();

            var xxx = xx.CreateRobots();

            foreach(var x in xxx)
            {
                var str = x.ToString();
            }
        }

        private void button3_Click_1(object sender, EventArgs e)
        {
            var hs = new HabrSite(null);

            hs.Run();
        }
    }
}