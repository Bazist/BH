using FTSearchWCF.Stemmers;
using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.IO;
using System.Linq;
using System.Security.Principal;
using System.ServiceModel;
using System.ServiceModel.Description;
using System.Text;
using System.Text.RegularExpressions;
using System.Threading.Tasks;

namespace BH.WCF
{
    

    [ServiceContract]
    [ServiceBehavior(InstanceContextMode = InstanceContextMode.Single)]
    public class FTService
    {
        #region Constructors
        public FTService()
        {

        }
        #endregion

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

        public class DocumentName
        {
            public DocumentName(string name, string version, string robotName)
            {
                Name = name;
                Version = version;
                RobotName = robotName;
            }

            public string Name { get; }
            public string Version { get; }
            public string RobotName { get; }

            public override string ToString()
            {
                return $"{Name};{Version};{RobotName}";
            }

            public static DocumentName Parse(string str)
            {
                var parts = str.Split(';');

                return new DocumentName(parts[0], parts[1], parts[2]);
            }
        }

        #endregion

        #region Members

        private static List<FTSearch> Instances = new List<FTSearch>();

        private long MaxSizeActiveInstance = 10L * 1024 * 1024 * 1024;

        private static FTSearch ActiveInstance = null;

        private static FTSearch.ConfigurationDLL _configuration;

        private static ServiceHost _host;

        private static object _lockObj = new object();

        public const string DEFAULT_FTS_PATH = @"c:\FTS";

        private static Action<Exception> _errorHandler = null;

        private static Func<string, string, string, string> _documentContentResolver = null;

        #endregion

        #region Constructors

        static FTService()
        {
            Instances = new List<FTSearch>();

            _configuration = GetDefaultConfiguration();
        }

        #endregion

        #region Methods

        [OperationContract]
        public bool IsStarted()
        {
            return TryCatch(() =>
            {
                return Instances.Count > 0;
            });
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
            return TryCatch(() =>
            {
                return _configuration;
            });
        }

        [OperationContract]
        public static FTSearch.ConfigurationDLL GetDefaultConfiguration()
        {
            return TryCatch(() =>
            {
                FTSearch.ConfigurationDLL conf = new FTSearch.ConfigurationDLL();

                byte[] indexPath = new byte[1024];
                Array.Copy(Encoding.ASCII.GetBytes(DEFAULT_FTS_PATH), indexPath, DEFAULT_FTS_PATH.Length);

                conf.IndexPath = indexPath;

                conf.MemoryMode = (uint)FTSearch.MemoryMode.InMemory;
                conf.AutoStemmingOn = 8;
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
            });
        }

        [OperationContract]
        public void SetConfiguration(FTSearch.ConfigurationDLL configuration)
        {
            TryCatch(() =>
            {
                _configuration = configuration;
            });
        }

        [OperationContract]
        public void Start(int instanceNumber = 0)
        {
            TryCatch(() =>
            {
                if (IsStarted())
                    throw new Exception("Service already started.");

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
            });
        }

        [OperationContract]
        public FTSearch.SearchResult SearchPhrase(string phrase,
                                                  string templateName,
                                                  int skip,
                                                  int take)
        {
            return TryCatch(() =>
             {
                 if (!IsStarted())
                     throw new Exception("Service is not started.");

                 return GetPortion(ref skip,
                                   take,
                                   fts => fts.SearchPhrase(phrase,
                                                           templateName,
                                                           0,
                                                           int.MaxValue,
                                                           skip));
             });
        }
        
        public IDictionary<string, string> ReadDocumentVersions(IEnumerable<string> fileNames)
        {
            return TryCatch(() =>
            {
                var dicDocumentNames = fileNames.ToDictionary(x => x,
                                                              x => string.Empty);

                if (dicDocumentNames.Count == 0)
                    return dicDocumentNames;

                var foundDocumentNames = 0;

                foreach (var instance in Instances.Reverse<FTSearch>())
                {
                    var info = instance.GetInfo();

                    for (uint docId = info.LastNameIDRAM - 1; docId >= 1; docId--)
                    {
                        string[] nameAndVersion = instance.GetDocumentNameByID(docId).Split(';');

                        string name = nameAndVersion[0];
                        string version = nameAndVersion[1];

                        if (dicDocumentNames.ContainsKey(name))
                        {
                            dicDocumentNames[name] = version;

                            foundDocumentNames++;

                            if (foundDocumentNames >= dicDocumentNames.Count)
                            {
                                return dicDocumentNames;
                            }
                        }
                    }
                }

                return dicDocumentNames;
            });
        }

        private FTSearch.SearchResult GetPortion(ref int skip,
                                                 int take,
                                                 Func<FTSearch, FTSearch.SearchResult> searchFunc)
        {
            FTSearch.SearchResult result = new FTSearch.SearchResult();

            foreach (var fts in Instances)
            {
                //fts.SearchQuery()
                var sr = searchFunc(fts); 

                if (skip > sr.FullCountMatches) //skip all results
                {
                    skip -= Convert.ToInt32(sr.FullCountMatches);
                }
                else
                {
                    if (skip + take <= sr.FullCountMatches) //all our data in current instance, get portion
                    {
                        result.MatchWords += sr.MatchWords;
                        result.Results.AddRange(sr.Results.Take(take - result.Results.Count));

                        break;
                    }
                    else //go to next instance
                    {
                        if (result.Results.Count + sr.Results.Count <= take) //take all results from current instance, goto next instance
                        {
                            result.MatchWords += sr.MatchWords;
                            result.Results.AddRange(sr.Results);

                            skip = 0;
                        }
                        else //take part results from current instance, exit
                        {
                            result.MatchWords += sr.MatchWords;
                            result.Results.AddRange(sr.Results.Take(take - result.Results.Count));

                            break;
                        }
                    }
                }
            }

            return result;
        }

        [OperationContract]
        public FTSearch.SearchResult SearchQuery(List<FTSearch.Selector> selectors,
                                                 int minPage,
                                                 int maxPage,
                                                 int skip,
                                                 bool agregateBySubject)
        {
            return TryCatch(() =>
            {
                if (!IsStarted())
                    throw new Exception("Service is not started.");

                //TODO merge logic on instance with logic on shards
                int shardSkip = 0;
                int shardTake = int.MaxValue;

                return GetPortion(ref shardSkip,
                                  shardTake,
                                  fts => fts.SearchQuery(selectors,
                                                         minPage,
                                                         maxPage,
                                                         skip,
                                                         agregateBySubject));
            });
        }

        [OperationContract]
        public FTSearch.SearchResult SearchPhraseRel(string phrase, int minPage, int maxPage, int skip, int take)
        {
            return TryCatch(() =>
            {
                return GetPortion(ref skip,
                                      take,
                                      fts => fts.SearchPhraseRel(phrase,
                                                                 minPage,
                                                                 maxPage));
            });
        }

        [OperationContract]
        public void InitSearchRel()
        {
            TryCatch(() =>
            {
                foreach (var fts in Instances)
                {
                    fts.InitSearchRel();
                }
            });
        }

        private static RussianStemmer _rusStemmer = new RussianStemmer();
        private static EnglishStemmer _engStemmer = new EnglishStemmer();

        [OperationContract]
        public string StemContent(string contentText)
        {
            contentText = _rusStemmer.StemContent(contentText);
            contentText = _engStemmer.StemContent(contentText);
            
            return contentText.ToLower();
        }

        [OperationContract]
        public string LoadDocumentContent(string documentName,
                                          string documentVersion,
                                          string robotName)
        {
            return TryCatch(() =>
            {
                if (_documentContentResolver != null)
                {
                    return _documentContentResolver(documentName, documentVersion, robotName);
                }
                else
                {
                    return null;
                }
            });
        }

        [OperationContract]
        public string CalculateTrend(string phrase, int count, int minPage, int maxPage)
        {
            return TryCatch(() =>
            {
                return ActiveInstance.CalculateTrend(phrase,
                                                     count,
                                                     minPage,
                                                     maxPage);
            });
        }

        [OperationContract]
        public Info GetInfo()
        {
            return TryCatch<Info>(() =>
            {
                if (!IsStarted())
                    throw new Exception("Service is not started.");

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
                result.TextSize = result.IndexSize * 123; //in average

                return result;
            });
        }

        [OperationContract]
        public bool IndexText(string documentName,
                              string documentVersion,
                              string contentText,
                              string robotName)
        {
            return TryCatch<bool>(() =>
            {
                if (!IsStarted())
                    throw new Exception("Service is not started.");

                var docName = new DocumentName(documentName, documentVersion, robotName);

                return ActiveInstance.IndexContent(docName.ToString(), contentText, FTSearch.ContentType.Text);

                //{
                //    foreach (var file in IndexedFilesInMemory)
                //    {
                //        File.AppendAllText(string.Format(@"{0}\log.txt", Path),
                //                           string.Format(@"[{0}] {1}", DateTime.Now.ToString(), file));
                //    }
                //}
            });
        }

        [OperationContract]
        public bool IndexFile(string documentName,
                              string documentVersion,
                              string filePath,
                              string robotName)
        {
            return TryCatch<bool>(() =>
            {
                if (!IsStarted())
                    throw new Exception("Service is not started.");

                var docName = new DocumentName(documentName, documentVersion, robotName);

                var contentText = File.ReadAllText(filePath, Encoding.GetEncoding("windows-1251"));

                if (contentText.Length > 65000)
                {
                    contentText = contentText.Substring(0, 65000);
                }

                return ActiveInstance.IndexContent(docName.ToString(), contentText, FTSearch.ContentType.Text);

                //{
                //    foreach (var file in IndexedFilesInMemory)
                //    {
                //        File.AppendAllText(string.Format(@"{0}\log.txt", Path),
                //                           string.Format(@"[{0}] {1}", DateTime.Now.ToString(), file));
                //    }
                //}
            });
        }

        [OperationContract]
        public void SaveIndex()
        {
            TryCatch(() =>
            {
                if (!IsStarted())
                    throw new Exception("Service is not started.");

                ActiveInstance.SaveIndex();
            });
        }

        [OperationContract]
        public void MergeIndexes()
        {
            TryCatch(() =>
            {
                if (!IsStarted())
                    throw new Exception("Service is not started.");

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
            });
        }

        [OperationContract]
        public void Stop()
        {
            TryCatch(() =>
            {
                if (!IsStarted())
                    throw new Exception("Service already stopped.");

                if (Instances.Count > 0)
                {
                    foreach (var fts in Instances)
                    {
                        fts.StopInstance();
                    }

                    Instances.Clear();
                }
            });
        }

        [OperationContract]
        public void CheckIndexes()
        {
            TryCatch(() =>
            {
                if (!IsStarted())
                    throw new Exception("Service is not started.");

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
            });
        }

        //static void Unpack(string archive, string file, string dest)
        //{
        //    //7z e archive.zip - oc:\soft *.cpp - r
        //    //extracts all *.cpp files from archive archive.zip to c:\soft folder.

        //    var proc = new System.Diagnostics.Process();
        //    proc.StartInfo = new System.Diagnostics.ProcessStartInfo();
        //    proc.StartInfo.FileName = @"C:\Program Files\7-Zip\7z.exe";
        //    proc.StartInfo.Arguments = " x \"" + archive + "\" -o\"" + dest + "\" \"" + file + "\" -r";
        //    proc.StartInfo.UseShellExecute = false;
        //    proc.StartInfo.CreateNoWindow = true;
        //    proc.StartInfo.WindowStyle = ProcessWindowStyle.Hidden;

        //    //proc.StartInfo.Verb = "runas";

        //    proc.Start();
        //    proc.WaitForExit();

        //    //Console.WriteLine(proc.StandardError.ReadToEnd());
        //    //Console.WriteLine(proc.StandardOutput.ReadToEnd());
        //}

        //[OperationContract]
        //public string LoadContent(string name, string aroundPhrase)
        //{
        //    return TryCatch<string>(() =>
        //    {
        //        name = name.Replace(@"C:\FTS\Logs\Unpacked\", string.Empty);

        //        var parts = name.Split('\\');

        //        var archiveName = parts[0];

        //        var archive = Path.Combine(@"\\Production-Logs", archiveName) + ".7z";

        //        var file = name.Replace(archiveName + "\\", "");

        //        var dest = Path.Combine(@"C:\FTS\Logs\Cached", archiveName);

        //        if (!Directory.Exists(dest))
        //        {
        //            Directory.CreateDirectory(dest);
        //        }

        //        var fullPath = Path.Combine(dest, file);

        //        if (!File.Exists(fullPath))
        //        {
        //            Unpack(archive, file, dest);
        //        }

        //        var content = new StringBuilder();

        //        int startLine = -1;
        //        int endLine = -1;

        //        const int amountLines = 50;

        //        //need optimize !
        //        Queue<string> lines = new Queue<string>();

        //        using (StreamReader sr = new StreamReader(fullPath))
        //        {
        //            while (!sr.EndOfStream)
        //            {
        //                string line = sr.ReadLine();

        //                //Regex reg1 = new Regex("[^a-zA-Z0-9]+" + aroundPhrase + "[^a-zA-Z0-9]+");
        //                //Regex reg2 = new Regex("^" + aroundPhrase + "[^a-zA-Z0-9]+");
        //                //Regex reg3 = new Regex("[^a-zA-Z0-9]+" + aroundPhrase + "$");

        //                var words = aroundPhrase.Split(new char[] { ' ' }, StringSplitOptions.RemoveEmptyEntries);

        //                if (words.Any(m => Regex.Match(line, "\\b" + m + "\\b", RegexOptions.IgnoreCase).Success))
        //                {
        //                    if (startLine == -1)
        //                    {
        //                        if (lines.Count > amountLines)
        //                        {
        //                            startLine = lines.Count - amountLines;
        //                        }
        //                        else
        //                        {
        //                            startLine = 0;
        //                        }
        //                    }

        //                    endLine = lines.Count + amountLines;
        //                }

        //                //make break
        //                if (startLine != -1 && lines.Count > endLine)
        //                {
        //                    content.AppendLine("[BREAK]");
        //                    content.AppendLine(lines.Aggregate((x, y) => x + "\n" + y));

        //                    if (content.Length > 1024 * 1024)
        //                    {
        //                        content.AppendLine("[TooManyMatches]");

        //                        return content.ToString();
        //                    }

        //                    startLine = -1;
        //                    endLine = -1;

        //                    //leave 50 rows
        //                    for (int i = 0; i < lines.Count - amountLines; i++)
        //                    {
        //                        lines.Dequeue();
        //                    }
        //                }

        //                if (startLine == -1 &&
        //                    endLine == -1 &&
        //                    lines.Count > amountLines)
        //                {
        //                    lines.Dequeue();
        //                }

        //                lines.Enqueue(line);
        //            }
        //        }

        //        //end of file
        //        if (startLine != -1)
        //        {
        //            content.AppendLine("[BREAK]");

        //            if (lines.Count > 0)
        //            {
        //                content.AppendLine(lines.Aggregate((x, y) => x + "\r\n" + y));
        //            }
        //        }

        //        //File.Delete(fullPath);

        //        return content.ToString();
        //    });
        //}

        private static T TryCatch<T>(Func<T> action)
        {
            try
            {
                lock (_lockObj)
                {
                    return action();
                }
            }
            catch (Exception ex)
            {
                if (_errorHandler != null)
                {
                    _errorHandler(ex);
                }

                throw;
            }
        }

        private static void TryCatch(Action action)
        {
            try
            {
                lock (_lockObj)
                {
                    action();
                }
            }
            catch (Exception ex)
            {
                if (_errorHandler != null)
                {
                    _errorHandler(ex);
                }

                throw;
            }
        }

        public static void StartWebservice(FTService service,
                                           string url,
                                           Func<string, string, string, string> documentContentResolver = null,
                                           Action<Exception> errorHandler = null)
        {
            _documentContentResolver = documentContentResolver;
            _errorHandler = errorHandler;

            TryCatch(() =>
            {
                Uri serviceUri = new Uri(url);

                _host = new ServiceHost(service, serviceUri);

                var basicHttpBinding = new BasicHttpBinding();
                basicHttpBinding.MaxReceivedMessageSize = int.MaxValue;
                basicHttpBinding.MaxBufferSize = int.MaxValue;
                
                var smb = new ServiceMetadataBehavior();
                smb.HttpGetEnabled = true;
                smb.MetadataExporter.PolicyVersion = PolicyVersion.Policy15;

                _host.Description.Behaviors.Add(smb);

                Type serviceType = typeof(FTService);
                _host.AddServiceEndpoint(serviceType, basicHttpBinding, serviceUri);
                _host.Open();
            });
        }

        public static void StopWebService()
        {
            TryCatch(() =>
            {
                if (_host != null)
                {
                    _host.Abort();
                }
            });
        }

        public void ClearInstance()
        {
            TryCatch(() =>
            {
                ActiveInstance.ClearInstance();
            });
        }

        #endregion
    }
}
