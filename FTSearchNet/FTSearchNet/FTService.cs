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
    public class FTService
    {
        public List<FTSearch> Instances = new List<FTSearch>();
        public long MaxSizeActiveInstance = 10L * 1024 * 1024 * 1024;

        public FTSearch ActiveInstance = null;

        public string Path { get; set; }

        public bool InMemoryMode { get; set; }
        public uint AutoStemmingOn { get; set; }
        public uint MinLenWord { get; set; }
        public uint MaxLenWord { get; set; }
        public uint DocumentNameSize { get; set; }
        public uint CountWordInPhrase { get; set; }
        public bool IsUseNumberAlphabet { get; set; }
        public bool IsStarted
        {
            get
            {
                return Instances.Count > 0;
            }
        }

        public FTService(string path)
        {
            Path = path;

            Instances = new List<FTSearch>();

            InMemoryMode = false;
            AutoStemmingOn = 12;
            MinLenWord = 3;
            MaxLenWord = 64;
            DocumentNameSize = FTSearch.DOC_NAME_LENGTH;
            CountWordInPhrase = 1;
            IsUseNumberAlphabet = true;
        }

        private FTSearch.ConfigurationDLL Configuration { get; set; }

        private Tuple<string, long>[] GetInstances(int instanceNumber = 0)
        {
            string template = "Instance" + (instanceNumber > 0 ? instanceNumber.ToString() : string.Empty);

            var dirs = Directory.GetDirectories(Path).Where(x => x.Contains(template)).ToArray();

            var dirsLen = dirs.Select(x => new Tuple<string, long>(x, Directory.GetFiles(x).Sum(y => new FileInfo(y).Length)));

            var dirsLenSort = dirsLen.OrderBy(x => x.Item2).ToArray();

            return dirsLenSort.ToArray();
        }

        [OperationContract]
        public void Start(int instanceNumber = 0)
        {
            if (IsStarted)
                Stop();

            var dirsLenSort = GetInstances(instanceNumber);

            uint maxInstanceNumber = 0;

            for (int i = 0; i < dirsLenSort.Length; i++)
            {
                string[] parts = dirsLenSort[i].Item1.Split(new string[] { "Instance" }, StringSplitOptions.None);

                string instance = parts[parts.Length - 1];

                UInt32 currInstanceNumber = uint.Parse(instance);

                FTSearch fts = new FTSearch(currInstanceNumber);

                fts.StartInstance(Path, InMemoryMode, AutoStemmingOn, MinLenWord, MaxLenWord, DocumentNameSize, CountWordInPhrase, IsUseNumberAlphabet, false, false);

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
                FTSearch fts = new FTSearch(maxInstanceNumber + 1);

                fts.StartInstance(Path, InMemoryMode, AutoStemmingOn, MinLenWord, MaxLenWord, DocumentNameSize, CountWordInPhrase, IsUseNumberAlphabet, false, false);

                Instances.Add(fts);

                ActiveInstance = fts;
            }
        }

        [OperationContract]
        public List<FTSearch.Result> SearchPhrase(string phrase)
        {
            var result = new List<FTSearch.Result>();

            foreach (var fts in Instances)
            {
                result.AddRange(fts.SearchPhrase(phrase, uint.MinValue, uint.MaxValue));
            }

            return result;
        }

        [OperationContract]
        public FTSearch.FTSInstanceInfo GetInfo()
        {
            var result = new FTSearch.FTSInstanceInfo();

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

                        FTSearch fts = new FTSearch(instanceNumber);

                        fts.StartInstance(Path, InMemoryMode, AutoStemmingOn, MinLenWord, MaxLenWord, DocumentNameSize, CountWordInPhrase, IsUseNumberAlphabet, false, false);

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

                FTSearch fts = new FTSearch(instanceNumber);

                fts.StartInstance(Path, InMemoryMode, AutoStemmingOn, MinLenWord, MaxLenWord, DocumentNameSize, CountWordInPhrase, IsUseNumberAlphabet, true, false);

                fts.StopInstance();
            }
        }

        private static ServiceHost _host;
        public static FTSearch _ftSearch;

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
    }
}
