using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.IO;
using System.Linq;
using System.Security.Principal;
using System.ServiceModel;
using System.Text;
using System.Text.RegularExpressions;
using System.Threading.Tasks;

namespace FTServiceWCF
{
    [ServiceContract]
    [ServiceBehavior(InstanceContextMode = InstanceContextMode.Single)]
    public class FTService
    {
        #region Classes

        public struct Info
        {
            public uint Version;

            public uint AmountInstances;

            public ulong IndexSize;

            public ulong TextSize;

            public uint LastNameIDRAM;
            public uint LastNameIDHDD;

            public ulong CountWordsRAM;
            public ulong CountWordsHDD;

            public ulong UsedMemory;
            public ulong TotalMemory;

            public uint WordsHeaderBase;

            public uint DocumentNameSize;

            public string LastErrorMessage;
            public bool HasError;

            public uint RelevantLevel;
        }

        #endregion

        #region Members

        private List<FTSearch> Instances = new List<FTSearch>();

        private long MaxSizeActiveInstance = 200L * 1024 * 1024 * 1024;

        private FTSearch ActiveInstance = null;

        private FTSearch.ConfigurationDLL _configuration;

        private static ServiceHost _host;

        public const string DEFAULT_FTS_PATH = @"i:\FTS_Merged";

        #endregion

        #region Constructors

        public FTService()
        {
            Instances = new List<FTSearch>();

            _configuration = GetDefaultConfiguration();
        }

        #endregion

        #region Methods

        [OperationContract]
        public bool IsStarted()
        {
            lock (this)
            {
                return Instances.Count > 0;
            }
        }

        private string GetPath()
        {
            return Encoding.ASCII.GetString(GetConfiguration().IndexPath).Replace("\0", "");
        }

        private Tuple<string, long>[] GetInstances(int instanceNumber = 0)
        {
            string template = "Instance" + (instanceNumber > 0 ? instanceNumber.ToString() : string.Empty);

            var dirs = Directory.GetDirectories(GetPath()).Where(x => x.Contains(template)).ToArray();

            var dirsLen = dirs.Select(x => new Tuple<string, long>(x, Directory.GetFiles(x).Sum(y => new FileInfo(y).Length)));

            var dirsLenSort = dirsLen.OrderBy(x => x.Item2).ToArray();

            return dirsLenSort.ToArray();
        }

        [OperationContract]
        public FTSearch.ConfigurationDLL GetConfiguration()
        {
            lock (this)
            {
                return _configuration;
            }
        }

        [OperationContract]
        public FTSearch.ConfigurationDLL GetDefaultConfiguration()
        {
            lock (this)
            {
                FTSearch.ConfigurationDLL conf = new FTSearch.ConfigurationDLL();

                byte[] indexPath = new byte[1024];
                Array.Copy(Encoding.ASCII.GetBytes(DEFAULT_FTS_PATH), indexPath, DEFAULT_FTS_PATH.Length);

                conf.IndexPath = indexPath;

                conf.MemoryMode = (uint)FTSearch.MemoryMode.HDD;
                conf.AutoStemmingOn = 12;
                conf.MinLenWord = 3;
                conf.MaxLenWord = 64;
                conf.DocumentNameSize = FTSearch.DOC_NAME_LENGTH;
                conf.CountWordInPhrase = 1;
                conf.IsUseNumberAlphabet = true;

                conf.InstanceNumber = 1;
                conf.IsUseRussianAlphabet = true;
                conf.IsUseUkranianAlphabet = true;
                conf.IsUseEnglishAlphabet = true;
                conf.WordsHeaderBase = 24;
                conf.LimitTopResults = 100;
                conf.LimitUsedMemory = 8000000000; //8000 má by default
                conf.RelevantLevel = 0;
                conf.IsCreateNewInstanceOnUpdate = false;
                conf.IsCustomPath = false;
                conf.AutoSaveIndex = false;

                return conf;
            }
        }

        [OperationContract]
        public void SetConfiguration(FTSearch.ConfigurationDLL configuration)
        {
            lock (this)
            {
                _configuration = configuration;
            }
        }

        [OperationContract]
        public void Start(int instanceNumber = 0)
        {
            lock (this)
            {
                if (IsStarted())
                    Stop();

                var dirsLenSort = GetInstances(instanceNumber);

                uint maxInstanceNumber = 0;

                for (int i = 0; i < dirsLenSort.Length; i++)
                {
                    string[] parts = dirsLenSort[i].Item1.Split(new string[] { "Instance" }, StringSplitOptions.None);

                    string instance = parts[parts.Length - 1];

                    UInt32 currInstanceNumber = uint.Parse(instance);

                    FTSearch fts = new FTSearch();

                    var conf = GetConfiguration();

                    conf.InstanceNumber = currInstanceNumber;

                    fts.StartInstance(conf);

                    Instances.Add(fts);

                    if (ActiveInstance == null && dirsLenSort[i].Item2 < MaxSizeActiveInstance)
                    {
                        ActiveInstance = fts;
                    }

                    if (currInstanceNumber > maxInstanceNumber)
                    {
                        maxInstanceNumber = currInstanceNumber;
                    }
                }

                if (ActiveInstance == null)
                {
                    FTSearch fts = new FTSearch();

                    var conf = GetConfiguration();

                    conf.InstanceNumber = maxInstanceNumber + 1;

                    fts.StartInstance(conf);

                    Instances.Add(fts);

                    ActiveInstance = fts;
                }
            }
        }

        [OperationContract]
        public List<FTSearch.Result> SearchPhrase(string phrase, string templateName, int skip, int take)
        {
            lock (this)
            {
                var result = new List<FTSearch.Result>();

                foreach (var fts in Instances)
                {
                    var sr = fts.SearchPhrase(phrase, templateName, uint.MinValue, uint.MaxValue, Convert.ToUInt32(skip));

                    if (skip > sr.FullCountMatches) //skip all results
                    {
                        skip -= Convert.ToInt32(sr.FullCountMatches);
                    }
                    else
                    {
                        if (skip + take <= sr.FullCountMatches) //all our data in current instance, get portion
                        {
                            result.AddRange(sr.Results.Take(take - result.Count));

                            break;
                        }
                        else //go to next instance
                        {
                            if (result.Count + sr.Results.Count <= take) //take all results from current instance, goto next instance
                            {
                                result.AddRange(sr.Results);

                                skip = 0;
                            }
                            else //take part results from current instance, exit
                            {
                                result.AddRange(sr.Results.Take(take - result.Count));

                                break;
                            }
                        }
                    }
                }

                return result;
            }
        }

        [OperationContract]
        public Info GetInfo()
        {
            lock (this)
            {
                var result = new Info();

                foreach (var fts in Instances)
                {
                    var current = fts.GetInfo();

                    result.CountWordsHDD += current.CountWordsHDD;
                    result.CountWordsRAM += current.CountWordsRAM;
                    result.DocumentNameSize = current.DocumentNameSize;
                    result.HasError = result.HasError || current.HasError;
                    result.LastErrorMessage += current.LastErrorMessage;
                    result.LastNameIDHDD += current.LastNameIDHDD;
                    result.LastNameIDRAM += current.LastNameIDRAM;
                    result.TotalMemory += current.TotalMemory;
                    result.UsedMemory += current.UsedMemory;
                    result.Version = current.Version;
                    result.WordsHeaderBase = current.WordsHeaderBase;
                }

                result.IndexSize = Convert.ToUInt64(GetInstances().Sum(x => x.Item2));
                result.AmountInstances = Convert.ToUInt32(Instances.Count);
                result.TextSize = result.IndexSize * 43; //in average

                return result;
            }
        }

        [OperationContract]
        public bool IndexText(string aliasName, string contentText)
        {
            lock (this)
            {
                return ActiveInstance.IndexContent(aliasName, contentText, FTSearch.ContentType.Text);

                //{
                //    foreach (var file in IndexedFilesInMemory)
                //    {
                //        File.AppendAllText(string.Format(@"{0}\log.txt", Path),
                //                           string.Format(@"[{0}] {1}", DateTime.Now.ToString(), file));
                //    }
                //}
            }
        }

        [OperationContract]
        public void SaveIndex()
        {
            lock (this)
            {
                ActiveInstance.SaveIndex();
            }
        }

        [OperationContract]
        public void MergeIndexes()
        {
            lock (this)
            {
                int skipedBySize = 0;

                while (true)
                {
                    var dirsLenSort = GetInstances();

                    if (dirsLenSort.Length <= skipedBySize + 1)
                    {
                        break;
                    }

                    skipedBySize = 0;

                    for (int i = 0; i < dirsLenSort.Length - 1; i += 2)
                    {
                        if (dirsLenSort[i].Item2 + dirsLenSort[i + 1].Item2 < MaxSizeActiveInstance)
                        {
                            string path1 = dirsLenSort[i].Item1;
                            string path2 = dirsLenSort[i + 1].Item1;

                            string[] parts = path1.Split(new string[] { "Instance" }, StringSplitOptions.None);

                            string instance = parts[parts.Length - 1];

                            UInt32 instanceNumber = uint.Parse(instance);

                            FTSearch fts = new FTSearch();

                            var conf = GetConfiguration();

                            conf.InstanceNumber = instanceNumber;

                            fts.StartInstance(conf);

                            fts.ImportIndex(path2);

                            fts.StopInstance();

                            Directory.Delete(path2, true);
                        }
                        else
                        {
                            skipedBySize += 2;
                        }
                    }
                }
            }
        }

        [OperationContract]
        public void Stop()
        {
            lock (this)
            {
                foreach (var fts in Instances)
                {
                    fts.StopInstance();
                }

                Instances.Clear();
            }
        }

        [OperationContract]
        public void CheckIndexes()
        {
            lock (this)
            {
                var dirsLenSort = GetInstances();

                for (int i = 0; i < dirsLenSort.Length; i++)
                {
                    string[] parts = dirsLenSort[i].Item1.Split(new string[] { "Instance" }, StringSplitOptions.None);

                    string instance = parts[parts.Length - 1];

                    UInt32 instanceNumber = uint.Parse(instance);

                    FTSearch fts = new FTSearch();

                    var conf = GetConfiguration();

                    conf.InstanceNumber = instanceNumber;
                    //conf.OnlyCheckIndex = false;

                    fts.StartInstance(conf, true);

                    fts.StopInstance();
                }
            }
        }

        static void Unpack(string archive, string file, string dest)
        {
            //7z e archive.zip - oc:\soft *.cpp - r
            //extracts all *.cpp files from archive archive.zip to c:\soft folder.

            var proc = new System.Diagnostics.Process();
            proc.StartInfo = new System.Diagnostics.ProcessStartInfo();
            proc.StartInfo.FileName = @"C:\Program Files\7-Zip\7z.exe";
            proc.StartInfo.Arguments = " x \"" + archive + "\" -o\"" + dest + "\" \"" + file + "\" -r";
            proc.StartInfo.UseShellExecute = false;
            proc.StartInfo.CreateNoWindow = true;
            proc.StartInfo.WindowStyle = ProcessWindowStyle.Hidden;

            //proc.StartInfo.Verb = "runas";

            proc.Start();
            proc.WaitForExit();

            //Console.WriteLine(proc.StandardError.ReadToEnd());
            //Console.WriteLine(proc.StandardOutput.ReadToEnd());
        }

        [OperationContract]
        public string LoadContent(string name, string aroundPhrase)
        {
            lock (this)
            {
                name = name.Replace(@"C:\FTS\Logs\Unpacked\", string.Empty);

                var parts = name.Split('\\');

                var archiveName = parts[0];

                var archive = Path.Combine(@"\\srv3\fs\Production-Logs", archiveName) + ".7z";

                var file = name.Replace(archiveName + "\\", "");

                var dest = Path.Combine(@"C:\FTS\Logs\Cached", archiveName);

                if (!Directory.Exists(dest))
                {
                    Directory.CreateDirectory(dest);
                }

                var fullPath = Path.Combine(dest, file);

                if (!File.Exists(fullPath))
                {
                    Unpack(archive, file, dest);
                }

                var content = new StringBuilder();

                int startLine = -1;
                int endLine = -1;

                const int amountLines = 50;

                //need optimize !
                Queue<string> lines = new Queue<string>();

                using (StreamReader sr = new StreamReader(fullPath))
                {
                    while (!sr.EndOfStream)
                    {
                        string line = sr.ReadLine();

                        //Regex reg1 = new Regex("[^a-zA-Z0-9]+" + aroundPhrase + "[^a-zA-Z0-9]+");
                        //Regex reg2 = new Regex("^" + aroundPhrase + "[^a-zA-Z0-9]+");
                        //Regex reg3 = new Regex("[^a-zA-Z0-9]+" + aroundPhrase + "$");

                        var words = aroundPhrase.Split(new char[] { ' ' }, StringSplitOptions.RemoveEmptyEntries);

                        if (words.Any(m => Regex.Match(line, "\\b" + m + "\\b", RegexOptions.IgnoreCase).Success))
                        {
                            if (startLine == -1)
                            {
                                if (lines.Count > amountLines)
                                {
                                    startLine = lines.Count - amountLines;
                                }
                                else
                                {
                                    startLine = 0;
                                }
                            }

                            endLine = lines.Count + amountLines;
                        }

                        //make break
                        if (startLine != -1 && lines.Count > endLine)
                        {
                            content.AppendLine("[BREAK]");
                            content.AppendLine(lines.Aggregate((x, y) => x + "\n" + y));

                            if (content.Length > 1024 * 1024)
                            {
                                content.AppendLine("[TooManyMatches]");

                                return content.ToString();
                            }

                            startLine = -1;
                            endLine = -1;

                            //leave 50 rows
                            for (int i = 0; i < lines.Count - amountLines; i++)
                            {
                                lines.Dequeue();
                            }
                        }

                        if (startLine == -1 &&
                            endLine == -1 &&
                            lines.Count > amountLines)
                        {
                            lines.Dequeue();
                        }

                        lines.Enqueue(line);
                    }
                }

                //end of file
                if (startLine != -1)
                {
                    content.AppendLine("[BREAK]");

                    if (lines.Count > 0)
                    {
                        content.AppendLine(lines.Aggregate((x, y) => x + "\r\n" + y));
                    }
                }

                //File.Delete(fullPath);

                return content.ToString();
            }
        }

        public static void StartWebservice(string url)
        {
            Type serviceType = typeof(FTService);
            Uri serviceUri = new Uri(url);

            _host = new ServiceHost(serviceType, serviceUri);

            var basicHttpBinding = new BasicHttpBinding();
            basicHttpBinding.MaxReceivedMessageSize = int.MaxValue;
            basicHttpBinding.MaxBufferSize = int.MaxValue;


            _host.AddServiceEndpoint(serviceType, basicHttpBinding, serviceUri);
            _host.Open();
        }

        public static void StopWebService()
        {
            _host.Abort();
        }

        public void ClearInstance()
        {
            ActiveInstance.ClearInstance();
        }

        #endregion
    }
}
