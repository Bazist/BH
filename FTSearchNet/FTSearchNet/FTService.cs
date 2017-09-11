using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.ServiceModel;
using System.Text;
using System.Threading.Tasks;

namespace FTSearchNet
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

            public uint CountWordsRAM;
            public uint CountWordsHDD;

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

        private long MaxSizeActiveInstance = 10L * 1024 * 1024 * 1024;

        private FTSearch ActiveInstance = null;

        private FTSearch.ConfigurationDLL _configuration;

        private static ServiceHost _host;
        private static FTSearch _ftSearch;
        
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
            return Instances.Count > 0;
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
            return _configuration;
        }

        [OperationContract]
        public FTSearch.ConfigurationDLL GetDefaultConfiguration()
        {
            FTSearch.ConfigurationDLL conf = new FTSearch.ConfigurationDLL();

            string path = @"C:\FTS";

            byte[] indexPath = new byte[1024];
            Array.Copy(Encoding.ASCII.GetBytes(path), indexPath, path.Length);

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

        [OperationContract]
        public void SetConfiguration(FTSearch.ConfigurationDLL configuration)
        {
            _configuration = configuration;
        }

        [OperationContract]
        public void Start(int instanceNumber = 0)
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

        [OperationContract]
        public List<FTSearch.Result> SearchPhrase(string phrase, int skip, int take)
        {
            var result = new List<FTSearch.Result>();

            foreach (var fts in Instances)
            {
                var sr = fts.SearchPhrase(phrase, uint.MinValue, uint.MaxValue, Convert.ToUInt32(skip));

                if(skip > sr.FullCountMatches) //skip all results
                {
                    skip -= Convert.ToInt32(sr.FullCountMatches);
                }
                else
                {
                    if(skip + take <= sr.FullCountMatches) //all our data in current instance, get portion
                    {
                        result.AddRange(sr.Results.Take(take));

                        break;
                    }
                    else //go to next instance
                    {
                        result.AddRange(sr.Results);

                        skip -= sr.Results.Count;
                    }
                }
            }

            return result;
        }

        [OperationContract]
        public Info GetInfo()
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
            result.TextSize = result.IndexSize * 55; //in average

            return result;
        }

        [OperationContract]
        public bool IndexText(string aliasName, string contentText)
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

        [OperationContract]
        public void SaveIndex()
        {
            ActiveInstance.SaveIndex();
        }

        [OperationContract]
        public void MergeIndexes()
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

                        //Directory.Delete(path2, true);
                    }
                    else
                    {
                        skipedBySize += 2;
                    }
                }
            }
        }

        [OperationContract]
        public void Stop()
        {
            foreach (var fts in Instances)
            {
                fts.StopInstance();
            }

            Instances.Clear();
        }

        [OperationContract]
        public void CheckIndexes()
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

                fts.StartInstance(conf);

                fts.StopInstance();
            }
        }
        
        public static void StartWebservice()
        {
            Type serviceType = typeof(FTService);
            Uri serviceUri = new Uri(string.Format("http://localhost:8080/"));

            _host = new ServiceHost(serviceType, serviceUri);

            var basicHttpBinding = new BasicHttpBinding();
            basicHttpBinding.MaxReceivedMessageSize = int.MaxValue;

            _host.AddServiceEndpoint(serviceType, basicHttpBinding, serviceUri);
            _host.Open();
        }

        #endregion
    }
}
