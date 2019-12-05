using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Runtime.InteropServices;
using System.IO;
using System.Net;
using System.ServiceModel;

namespace BH.WCF
{
    public class FTSearch
    {
        public FTSearch()
        {
        }

        private System.UInt32 InstanceNumber { get; set; }

        #region Enums

        public enum ContentType
        {
            HTML = 1,
            Text = 2
        };

        public enum MemoryMode : UInt32
        {
            InMemory = 1,
            HDD = 2,
            HDDBuffered = 3
        }

        #endregion

        #region Structs

        public struct FTSInstanceInfo
        {
            public uint Version;

            public uint LastNameIDRAM;
            public uint LastNameIDHDD;

            public System.UInt64 CountWordsRAM;
            public System.UInt64 CountWordsHDD;

            public System.UInt64 UsedMemory;
            public System.UInt64 TotalMemory;

            public uint WordsHeaderBase;

            //public uint DocumentNameSize;

            public string LastErrorMessage;
            public bool HasError;

            public uint ControlValue;

            public uint RelevantLevel;

            public uint CountWordInPhrase;

            public uint ReservedValue1;
            public uint ReservedValue2;
        }

        public struct ResultPosition
        {
            public int StartPosition;
            public int Length;
        }

        public struct Result
        {
            public string Name;
            public ResultPosition[] Positions;
        }

        public class Selector
        {
            public string Name;

            public string FilePath;

            public string MinBound;
            public string MaxBound;

            public byte RangeType;     //0 - no range, 1 - range of numbers
            public byte SelectorType;       //1 - filter, 2 - columns, 3 - sorting, 4 - filter + columns + sorting
            public byte AgregationType;   //1 - Amount, 2 - Min value, 3 - Max value
            public byte AgregationSortType; //1 - Amount, 2 - Min value, 3 - Max value, 4 - Doc number
            public bool IsSortAsc;          //0 - desc, 1 - asc
            public byte ConditionType;     //1 - Equals to amount words in dictionary,

            public System.UInt32 Operand2;

            public SelectorDLL ConvertToSelectorDLL()
            {
                SelectorDLL selectorDLL = new SelectorDLL();

                byte[] text;

                if (this.Name != null)
                {
                    text = FTSearch.Encoding.GetBytes(this.Name);
                    Array.Copy(text, selectorDLL.Name, text.Length);
                }

                if (this.FilePath != null)
                {
                    text = FTSearch.Encoding.GetBytes(this.FilePath);
                    Array.Copy(text, selectorDLL.FilePath, text.Length);
                }

                if (this.MinBound != null)
                {
                    text = FTSearch.Encoding.GetBytes(this.MinBound);
                    Array.Copy(text, selectorDLL.MinBound, text.Length);
                }

                if (this.MaxBound != null)
                {
                    text = FTSearch.Encoding.GetBytes(this.MaxBound);
                    Array.Copy(text, selectorDLL.MaxBound, text.Length);
                }

                selectorDLL.RangeType = this.RangeType;
                selectorDLL.SelectorType = this.SelectorType;
                selectorDLL.AgregationType = this.AgregationType;
                selectorDLL.AgregationSortType = this.AgregationSortType;
                selectorDLL.IsSortAsc = this.IsSortAsc ? (byte)1 : (byte)0;
                selectorDLL.ConditionType = this.ConditionType;
                selectorDLL.Operand2 = this.Operand2;

                return selectorDLL;
            }
        }

        #endregion

        #region DLL Structs



        [StructLayout(LayoutKind.Sequential, CharSet = CharSet.Ansi)]
        unsafe public class ConfigurationDLL
        {
            public void SetIndexPath(string indexPath)
            {
                byte[] arrPath = new byte[1024];
                Array.Copy(Encoding.GetBytes(indexPath), arrPath, indexPath.Length);

                IndexPath = arrPath;
            }

            [MarshalAs(UnmanagedType.ByValArray, SizeConst = 1024)]
            public byte[] IndexPath;

            public System.UInt32 InstanceNumber;
            public System.UInt32 MinLenWord;
            public System.UInt32 MaxLenWord;
            public System.UInt32 AutoStemmingOn;

            public System.UInt32 WordsHeaderBase;

            //public System.UInt32 DocumentNameSize;
            
            public System.UInt32 LimitTopResults;
            public System.UInt64 LimitUsedMemory;

            //Level 0 - All words should be in document. A distance between words doesn't matter.
            //Level 1 - A document split on two parts. Words should be in one of part.
            //Level 2 - A document split on four parts. Words should be in one of part.
            //Level 3 - A document split on eight parts. Words should be in one of part.
            //etc...

            public System.UInt32 RelevantLevel;
            public System.UInt32 CountWordInPhrase;

            [MarshalAs(UnmanagedType.I1)]
            public System.Boolean IsCustomPath;

            public System.UInt32 MemoryMode;

            [MarshalAs(UnmanagedType.I1)]
            public System.Boolean IsUseUkranianAlphabet;

            [MarshalAs(UnmanagedType.I1)]
            public System.Boolean IsUseRussianAlphabet;

            [MarshalAs(UnmanagedType.I1)]
            public System.Boolean IsUseEnglishAlphabet;

            [MarshalAs(UnmanagedType.I1)]
            public System.Boolean IsUseNumberAlphabet;

            [MarshalAs(UnmanagedType.I1)]
            public System.Boolean IsCreateNewInstanceOnUpdate;

            [MarshalAs(UnmanagedType.I1)]
            public System.Boolean AutoSaveIndex;
        }

        [StructLayout(LayoutKind.Sequential, CharSet = CharSet.Ansi)]
        public struct FTSInstanceInfoDLL
        {
            public System.UInt32 Version;

            public System.UInt32 LastNameIDRAM;
            public System.UInt32 LastNameIDHDD;

            public System.UInt64 CountWordsRAM;
            public System.UInt64 CountWordsHDD;

            public System.UInt64 UsedMemory;
            public System.UInt64 TotalMemory;

            public System.UInt32 WordsHeaderBase;

            //public System.UInt32 DocumentNameSize;

            public IntPtr LastErrorMessage;

            [MarshalAs(UnmanagedType.I1)]
            public System.Boolean HasError;

            public System.UInt32 ControlValue;

            public System.UInt32 RelevantLevel;

            public System.UInt32 CountWordInPhrase;

            public System.UInt32 ReservedValue1;
            public System.UInt32 ReservedValue2;
        }

        [StructLayoutAttribute(LayoutKind.Sequential, CharSet = CharSet.Ansi)]
        public struct RelevantResultNameDLL
        {
            [MarshalAsAttribute(UnmanagedType.ByValTStr, SizeConst = MAX_DOC_NAME_LENGTH)]
            public string Name;
        }

        [StructLayout(LayoutKind.Sequential, CharSet = CharSet.Ansi)]
        public struct RelevantResultDLL
        {
            [MarshalAs(UnmanagedType.LPStr)]
            public string MatchWords;

            [MarshalAs(UnmanagedType.ByValArray, SizeConst = 25)]
            public RelevantResultNameDLL[] Matches;

            public System.UInt32 CountMatches;
            public System.UInt32 FullCountMatches;
        }

        public struct MatchPositionsDLL
        {
            public System.IntPtr Positions;
            public System.Byte CountPositions;
        };

        //[Serializable]
        [StructLayout(LayoutKind.Sequential, CharSet = CharSet.Ansi)]
        public class SelectorDLL
        {
            IntPtr pBaseClass;

            [MarshalAs(UnmanagedType.ByValArray, SizeConst = 128)]
            public byte[] Name = new byte[128];

            [MarshalAs(UnmanagedType.ByValArray, SizeConst = MAX_DOC_NAME_LENGTH)]
            public byte[] FilePath = new byte[MAX_DOC_NAME_LENGTH];

            [MarshalAs(UnmanagedType.ByValArray, SizeConst = 128)]
            public byte[] MinBound = new byte[128];

            [MarshalAs(UnmanagedType.ByValArray, SizeConst = 128)]
            public byte[] MaxBound = new byte[128];

            IntPtr pDictionary;

            public System.UInt32 AutoStemmingOn;
            public System.Byte RangeType;      //0 - no range, 1 - range of numbers
            public System.Byte SelectorType;   //1 - filter, 2 - columns, 3 - sorting, 4 - filter + columns + sorting
            public System.Byte AgregationType; //1 - Amount, 2 - Min value, 3 - Max value
            public System.Byte AgregationSortType; //1 - Amount, 2 - Min value, 3 - Max value, 4 - Doc Number
            public System.Byte IsSortAsc; //0 - asc, 1 - desc
            public System.Byte ConditionType;  //1 - Equals to amount words in dictionary,
            //2 - Less than Operand 2
            //3 - Equals than Operand 2
            //4 - More than Operand 2
            //5 - Less or equals than Operand 2
            //6 - More or equals than Operand 2
            public System.UInt32 Operand2;

            [MarshalAs(UnmanagedType.ByValArray, SizeConst = 128)]
            public byte[] tempWord = new byte[128];
        }

        [StructLayout(LayoutKind.Sequential, CharSet = CharSet.Ansi)]
        public class QueryRowDLL
        {
            public System.UInt32 DocNumber;

            [MarshalAs(UnmanagedType.ByValArray, SizeConst = MAX_DOC_NAME_LENGTH)]
            public byte[] DocName = new byte[MAX_DOC_NAME_LENGTH];

            [MarshalAs(UnmanagedType.ByValArray, SizeConst = 128)]
            public byte[] Text = new byte[128];

            public System.UInt32 CountFilterSelectors;
            public System.UInt32 CountColumnSelectors;
            public System.UInt32 CountSortSelectors;

            public System.UInt32 Weight;

            public System.Byte IsWeightCalcByGroupedSubjects;

            public System.Byte IsExcluded;
        }

        [StructLayout(LayoutKind.Sequential, CharSet = CharSet.Ansi)]
        public struct QueryResultDLL
        {
            public IntPtr QueryRows;

            public System.UInt32 minWeight;
            public System.UInt32 minWeightIndex;

            public System.UInt32 CountRows;
            public System.UInt32 Size;
        }

        #endregion

        #region DLL Import

#if DEBUG
        const string DLL_PATH = @"FTSearch.dll";
#else
        const string DLL_PATH = @"FTSearch.dll";
#endif

        //[DllImport(DLL_PATH, EntryPoint = "startDefaultInstance", CallingConvention = CallingConvention.StdCall, CharSet = CharSet.Ansi)]
        //private static unsafe extern void startDefaultInstanceDLL(byte* path,
        //                                                          System.UInt32 instance,
        //                                                          MemoryMode memoryMode,
        //                                                          System.UInt32 autoStemmingOn,
        //                                                          System.UInt32 minLenWord,
        //                                                          System.UInt32 maxLenWord,
        //                                                          System.UInt32 documentNameSize,
        //                                                          System.UInt32 countWordInPhrase,
        //                                                          bool isUseNumberAlphabet);

        [DllImport(DLL_PATH, EntryPoint = "startInstance", CallingConvention = CallingConvention.StdCall, CharSet = CharSet.Ansi)]
        private static extern void startInstanceDLL(ConfigurationDLL configuration, bool onlyCheckIndex);

        [DllImport(DLL_PATH, EntryPoint = "stopInstance", CallingConvention = CallingConvention.StdCall, CharSet = CharSet.Ansi)]
        private static extern void stopInstanceDLL(System.UInt32 instanceNumber);

        [DllImport(DLL_PATH, EntryPoint = "getInfo", CallingConvention = CallingConvention.StdCall, CharSet = CharSet.Ansi)]
        private static extern IntPtr getInfoDLL(System.UInt32 instanceNumber);

        [DllImport(DLL_PATH, EntryPoint = "searchPhrase", CallingConvention = CallingConvention.StdCall, CharSet = CharSet.Ansi)]
        private static unsafe extern IntPtr searchPhraseDLL(System.UInt32 instanceNumber,
                                                            byte* phrase,
                                                            System.UInt32 phraseLen,
                                                            byte* templateName,
                                                            System.UInt32 templateNameLen,
                                                            System.UInt32 minPage,
                                                            System.UInt32 maxPage,
                                                            uint skip);

        [DllImport(DLL_PATH, EntryPoint = "searchPhraseRel", CallingConvention = CallingConvention.StdCall, CharSet = CharSet.Ansi)]
        private static unsafe extern IntPtr searchPhraseRelDLL(System.UInt32 instanceNumber, byte* phrase, System.UInt32 phraseLen, System.UInt32 minPage, System.UInt32 maxPage);

        [DllImport(DLL_PATH, EntryPoint = "initSearchRel", CallingConvention = CallingConvention.StdCall, CharSet = CharSet.Ansi)]
        private static unsafe extern void initSearchRelDLL(System.UInt32 instanceNumber);

        [DllImport(DLL_PATH, EntryPoint = "searchQuery", CallingConvention = CallingConvention.StdCall, CharSet = CharSet.Ansi)]
        private static unsafe extern IntPtr searchQueryDLL(System.UInt32 instanceNumber, IntPtr[] selectors, System.UInt32 countSelectors, System.UInt32 minPage, System.UInt32 maxPage, System.UInt32 skip, System.Boolean agregateBySubject);

        [DllImport(DLL_PATH, EntryPoint = "searchNewMems", CallingConvention = CallingConvention.StdCall, CharSet = CharSet.Ansi)]
        private unsafe static extern IntPtr searchNewMemsDLL(System.UInt32 instanceNumber,
                                                           System.UInt32 startDate1Year,
                                                           System.UInt32 startDate1Month,
                                                           System.UInt32 startDate2Year,
                                                           System.UInt32 startDate2Month,
                                                           System.UInt32 startDate2Day,
                                                           System.UInt32 endDate2Year,
                                                           System.UInt32 endDate2Month,
                                                           System.UInt32 endDate2Day,
                                                           System.UInt32 periodInDays);

        [DllImport(DLL_PATH, EntryPoint = "calculateTrend", CallingConvention = CallingConvention.StdCall, CharSet = CharSet.Ansi)]
        private static unsafe extern IntPtr calculateTrendDLL(System.UInt32 instanceNumber, byte* word, System.UInt32 phraseLen, System.UInt32* points, System.UInt32 count, System.UInt32 minPage, System.UInt32 maxPage);

        [DllImport(DLL_PATH, EntryPoint = "getDocumentNameByID", CallingConvention = CallingConvention.StdCall, CharSet = CharSet.Ansi)]
        private static unsafe extern void getDocumentNameByIDDLL(System.UInt32 instanceNumber, System.UInt32 id, byte* name, System.UInt32 sizeName);

        [DllImport(DLL_PATH, EntryPoint = "releaseRelevantResult", CallingConvention = CallingConvention.StdCall, CharSet = CharSet.Ansi)]
        private static extern void releaseRelevantResultDLL(System.UInt32 instanceNumber, System.IntPtr pRelevantResult);

        [DllImport(DLL_PATH, EntryPoint = "releaseQueryResult", CallingConvention = CallingConvention.StdCall, CharSet = CharSet.Ansi)]
        private static extern void releaseQueryResultDLL(System.UInt32 instanceNumber, System.IntPtr pQueryResult);

        [DllImport(DLL_PATH, EntryPoint = "clearInstance", CallingConvention = CallingConvention.StdCall, CharSet = CharSet.Ansi)]
        private static extern void clearInstanceDLL(System.UInt32 instanceNumber);

        [DllImport(DLL_PATH, EntryPoint = "indexText", CallingConvention = CallingConvention.StdCall, CharSet = CharSet.Ansi)]
        [return: MarshalAs(UnmanagedType.I1)]
        private unsafe static extern bool indexTextDLL(System.UInt32 instanceNumber, byte* name, System.UInt32 nameLen, byte* text, System.UInt32 textLen);

        [DllImport(DLL_PATH, EntryPoint = "indexHtml", CallingConvention = CallingConvention.StdCall, CharSet = CharSet.Ansi)]
        [return: MarshalAs(UnmanagedType.I1)]
        private unsafe static extern bool indexHtmlDLL(System.UInt32 instanceNumber, byte* name, System.UInt32 nameLen, byte* text, System.UInt32 textLen);

        [DllImport(DLL_PATH, EntryPoint = "saveIndex", CallingConvention = CallingConvention.StdCall, CharSet = CharSet.Ansi)]
        private static extern void saveIndexDLL(System.UInt32 instanceNumber);

        [DllImport(DLL_PATH, EntryPoint = "importIndex", CallingConvention = CallingConvention.StdCall, CharSet = CharSet.Ansi)]
        private unsafe static extern void importIndexDLL(System.UInt32 instanceNumber, byte* importPath, bool isDeleteImportedIndex);

        [DllImport(DLL_PATH, EntryPoint = "saveIndex", CallingConvention = CallingConvention.StdCall, CharSet = CharSet.Ansi)]
        [return: MarshalAs(UnmanagedType.I1)]
        private unsafe static extern bool isNameExistsDLL(System.UInt32 instanceNumber, byte* name, System.UInt32 nameLen);


        #endregion

        #region Members

        public const int MAX_DOC_NAME_LENGTH = 256;
        public static Encoding Encoding = Encoding.GetEncoding("windows-1251");

        #endregion

        #region Methods

        public void StartInstance(ConfigurationDLL configuration, bool onlyCheckIndex = false)
        {
            /*
            ConfigurationDLL configuration = new ConfigurationDLL();

            byte[] indexPath = new byte[1024];
            Array.Copy(Encoding.GetBytes(path), indexPath, path.Length);

            configuration.IndexPath = indexPath;

            configuration.InstanceNumber = InstanceNumber;
            configuration.MemoryMode = (System.UInt32)memoryMode;
            configuration.AutoStemmingOn = autoStemmingOn;
            configuration.MinLenWord = minLenWord;
            configuration.MaxLenWord = maxLenWord;
            configuration.DocumentNameSize = documentNameSize;
            configuration.CountWordInPhrase = countWordInPhrase;
            configuration.IsUseNumberAlphabet = isUseNumberAlphabet;
            configuration.IsUseRussianAlphabet = true;
            configuration.IsUseUkranianAlphabet = true;
            configuration.IsUseEnglishAlphabet = true;
            configuration.WordsHeaderBase = 24;
            configuration.LimitTopResults = 100;
            configuration.LimitUsedMemory = 8000000000; //8000 má by default
            configuration.RelevantLevel = 0;
            configuration.IsCreateNewInstanceOnUpdate = false;
            configuration.IsCustomPath = false;
            configuration.AutoSaveIndex = autoSaveIndex;
            */

            InstanceNumber = configuration.InstanceNumber;

            startInstanceDLL(configuration, onlyCheckIndex);
        }

        public void StopInstance()
        {
            stopInstanceDLL(InstanceNumber);
        }

        public void SaveIndex()
        {
            saveIndexDLL(InstanceNumber);
        }

        public bool IndexFile(string aliasName, string fullName, ContentType type)
        {
            return IndexContent(aliasName, ReadFileBytes(fullName), type);
        }

        public unsafe bool IndexContent(string aliasName, string contentText, ContentType type)
        {
            fixed (Byte* pName = Encoding.GetBytes(aliasName), pContent = Encoding.GetBytes(contentText))
            {
                if (type == ContentType.Text)
                {
                    return indexTextDLL(InstanceNumber, pName, (System.UInt32)aliasName.Length, pContent, (System.UInt32)contentText.Length);
                }
                else if (type == ContentType.HTML)
                {
                    return indexHtmlDLL(InstanceNumber, pName, (System.UInt32)aliasName.Length, pContent, (System.UInt32)contentText.Length);
                }
                else
                {
                    throw new Exception("Type not found");
                }
            }
        }

        public unsafe bool IndexContent(string aliasName, byte[] contentBytes, ContentType type)
        {
            fixed (Byte* pName = Encoding.ASCII.GetBytes(aliasName), pContent = contentBytes)
            {
                if (type == ContentType.Text)
                {
                    return indexTextDLL(InstanceNumber, pName, (System.UInt32)aliasName.Length, pContent, (System.UInt32)contentBytes.Length);
                }
                else if (type == ContentType.HTML)
                {
                    return indexHtmlDLL(InstanceNumber, pName, (System.UInt32)aliasName.Length, pContent, (System.UInt32)contentBytes.Length);
                }
                else
                {
                    throw new Exception("Type not found");
                }
            }
        }

        public unsafe bool IsNameExists(string name)
        {
            fixed (Byte* pName = Encoding.ASCII.GetBytes(name))
            {
                return isNameExistsDLL(InstanceNumber, pName, (System.UInt32)name.Length);
            }
        }

        public unsafe bool IndexUrl(string aliasName, string url, ContentType type)
        {
            string uri;

            StringBuilder name = new StringBuilder(aliasName);
            byte[] contentBytes = Encoding.GetBytes(this.DownloadWebPage(url, out uri));

            if (type == ContentType.HTML)
            {
                fixed (Byte* pName = Encoding.GetBytes(aliasName), pContent = contentBytes)
                {
                    return indexHtmlDLL(InstanceNumber, pName, (System.UInt32)aliasName.Length, pContent, (System.UInt32)contentBytes.Length);
                }
            }
            else
            {
                throw new Exception("Type not found");
            }
        }

        public void ClearInstance()
        {
            clearInstanceDLL(InstanceNumber);
        }

        public static Encoding GetFileEncoding(string srcFile)
        {
            Encoding enc = Encoding.UTF8;

            //StreamReader sr = new StreamReader(srcFile);
            //string text = sr.ReadLine();
            //sr.Close();

            //text = text.Substring(text.IndexOf("encoding", StringComparison.OrdinalIgnoreCase));
            //text = text.Substring(text.IndexOf("\"", StringComparison.OrdinalIgnoreCase) + 1);
            //text = text.Substring(0, text.IndexOf("\"", StringComparison.OrdinalIgnoreCase));

            ////if (text.IndexOf("utf", StringComparison.OrdinalIgnoreCase) >= 0)
            ////    enc = Encoding.UTF8;
            ////else if (text.IndexOf("1251", StringComparison.OrdinalIgnoreCase) >= 0)
            ////    enc = Encoding.GetEncoding("windows-1251");
            ////else if (text.IndexOf("1252", StringComparison.OrdinalIgnoreCase) >= 0)
            ////    enc = Encoding.GetEncoding("windows-1252");
            ////else
            ////    enc = Encoding.UTF8;

            //try
            //{
            //    enc = Encoding.GetEncoding(text);
            //}
            //catch (Exception ex)
            //{
            //    enc = Encoding.UTF8;
            //}

            //// *** Use Default of Encoding.Default (Ansi CodePage)


            // *** Detect byte order mark if any - otherwise assume default
            //FileStream file = null;

            //byte[] buffer = new byte[5];
            //try
            //{
            //    file = new FileStream(srcFile, FileMode.Open);
            //    file.Read(buffer, 0, 5);

            //    if (buffer[0] == 0xef && buffer[1] == 0xbb && buffer[2] == 0xbf)
            //        enc = Encoding.UTF8;
            //    else if (buffer[0] == 0xfe && buffer[1] == 0xff)
            //        enc = Encoding.Unicode;
            //    else if (buffer[0] == 0 && buffer[1] == 0 && buffer[2] == 0xfe && buffer[3] == 0xff)
            //        enc = Encoding.UTF32;
            //    else if (buffer[0] == 0x2b && buffer[1] == 0x2f && buffer[2] == 0x76)
            //        enc = Encoding.UTF7;
            //}
            //finally
            //{
            //    if (file != null)
            //    {
            //        file.Close();
            //    }
            //}

            return enc;
        }

        public static byte[] ReadFileBytes(string fullName)
        {
            string text = File.ReadAllText(fullName, GetFileEncoding(fullName));
            return GetContentBytes(text);
        }

        public static byte[] GetContentBytes(string text)
        {
            byte[] bytes = Encoding.GetBytes(text);
            return bytes;
        }

        public static string ReadFileText(string fullName)
        {
            return File.ReadAllText(fullName, GetFileEncoding(fullName));
        }

        //TODO make another method to load portion of names in one call
        public unsafe string GetDocumentNameByID(uint id)
        {
            byte[] nameBytes = new byte[MAX_DOC_NAME_LENGTH];

            fixed (Byte* pName = nameBytes)
            {
                getDocumentNameByIDDLL(InstanceNumber, id, pName, MAX_DOC_NAME_LENGTH);

                return Encoding.GetString(nameBytes).Replace("\0", "");
            }
        }

        public unsafe List<Result> SearchPhraseWithOr(string phrase, uint minPage, uint maxPage)
        {
            string[] phrases = phrase.Split(new string[] { "|" }, StringSplitOptions.RemoveEmptyEntries);

            List<uint> ids = new List<uint>();

            foreach (string currPhrase in phrases)
            {
                ids.AddRange(SearchIDByPhrase(currPhrase, string.Empty, minPage, maxPage));
            }

            ids.Sort();

            List<Result> results = new List<Result>();

            byte[] nameBytes = new byte[MAX_DOC_NAME_LENGTH];

            fixed (Byte* pName = nameBytes)
            {
                foreach (uint id in ids.Distinct())
                {
                    //get name of document
                    getDocumentNameByIDDLL(InstanceNumber, id, pName, MAX_DOC_NAME_LENGTH);

                    Result result = new Result();

                    result.Name = Encoding.GetString(nameBytes).Replace("\0", "");

                    results.Add(result);
                }
            }

            return results;
        }

        public unsafe List<uint> SearchIDByPhrase(string phrase, string templateName, uint minPage, uint maxPage)
        {
            List<uint> results = new List<uint>();
            byte[] nameBytes = new byte[MAX_DOC_NAME_LENGTH];

            fixed (Byte* pPhrase = Encoding.GetBytes(phrase),
                         pTemplateName = Encoding.GetBytes(templateName),
                         pName = nameBytes)
            {
                IntPtr pRelevantResult = searchPhraseDLL(InstanceNumber,
                                                         pPhrase,
                                                         (System.UInt32)phrase.Length,
                                                         pTemplateName,
                                                         (System.UInt32)templateName.Length,
                                                         minPage,
                                                         maxPage,
                                                         0);

                RelevantResultDLL relevantResultDLL = (RelevantResultDLL)Marshal.PtrToStructure(pRelevantResult, typeof(RelevantResultDLL));

                for (uint i = 0; i < relevantResultDLL.CountMatches; i++)
                {
                    //NOT IMPLEMENTED
                    //results.Add(relevantResultDLL.Matches[i]);
                }

                releaseRelevantResultDLL(InstanceNumber, pRelevantResult);

                //Marshal.Release(pRelevantResult);
            }

            return results;
        }

        public class SearchResult
        {
            public List<Result> Results = new List<Result>();
            public string MatchWords;
            public uint FullCountMatches = 0;
        }

        public unsafe SearchResult SearchPhrase(string phrase,
                                                string templateName,
                                                int minPage,
                                                int maxPage,
                                                int skip)
        {
            if (templateName == null)
                templateName = string.Empty;

            SearchResult sr = new SearchResult();
            
            fixed (Byte* pPhrase = Encoding.GetBytes(phrase),
                         pTemplateName = Encoding.GetBytes(templateName))
            {
                IntPtr pRelevantResult = searchPhraseDLL(InstanceNumber,
                                                         pPhrase,
                                                         Convert.ToUInt32(phrase.Length),
                                                         pTemplateName,
                                                         Convert.ToUInt32(templateName.Length),
                                                         Convert.ToUInt32(minPage),
                                                         Convert.ToUInt32(maxPage),
                                                         Convert.ToUInt32(skip));

                RelevantResultDLL relevantResultDLL = (RelevantResultDLL)Marshal.PtrToStructure(pRelevantResult, typeof(RelevantResultDLL));

                sr.FullCountMatches = relevantResultDLL.FullCountMatches;

                for (uint i = 0; i < relevantResultDLL.CountMatches; i++)
                {
                    //get name of document
                    Result result = new Result();
                    //if (true)
                    //{
                    result.Name = relevantResultDLL.Matches[i].Name;
                    //*result.Positions = new ResultPosition[0]; //empty
                    //}
                    //else
                    //{
                    //    string[] nameAndPart = tempName.ToString().Split(new string[] { "=>" }, StringSplitOptions.None);

                    //    result.Name = nameAndPart[0];

                    //    //get positions
                    //    MatchPositionsDLL matchPositionsDLL = (MatchPositionsDLL)Marshal.PtrToStructure(relevantResultDLL.BestMatchPositions[i], typeof(MatchPositionsDLL));

                    //    byte[] matchPositions = new byte[matchPositionsDLL.CountPositions];

                    //    Marshal.Copy(matchPositionsDLL.Positions, matchPositions, 0, matchPositionsDLL.CountPositions);
                    //    Marshal.Release(matchPositionsDLL.Positions);
                    //    Marshal.Release(relevantResultDLL.BestMatchPositions[i]);

                    //    result.Positions = new ResultPosition[matchPositions.Length];
                    //    for (int j = 0; j < result.Positions.Length; j++)
                    //    {
                    //        result.Positions[j].StartPosition = int.Parse(nameAndPart[1]) * 65536 + matchPositions[j] * DOC_NAME_LENGTH;
                    //        result.Positions[j].Length = 512;
                    //    }
                    //}

                    sr.Results.Add(result);
                }

                //if (false)
                //{
                //    for (uint i = 0; i < relevantResultDLL.CountOtherMatches; i++)
                //    {
                //        getDocumentNameByIDDLL(relevantResultDLL.OtherMatches[i], tempName, 1024);
                //        relevantResult.OtherMatches.Add(tempName.ToString());
                //    }
                //}

                releaseRelevantResultDLL(InstanceNumber, pRelevantResult);

                //Marshal.Release(pRelevantResult);
            }

            return sr;
        }

        public unsafe void ImportIndex(string importPath, bool isDeleteImportedIndex = true)
        {
            fixed (Byte* pImportPath = Encoding.GetBytes(importPath))
            {
                importIndexDLL(InstanceNumber, pImportPath, isDeleteImportedIndex);
            }
        }

        public unsafe SearchResult SearchPhraseRel(string phrase,
                                                   int minPage,
                                                   int maxPage)
        {
            SearchResult result = new SearchResult();

            //byte[] nameBytes = new byte[DOC_NAME_LENGTH];

            fixed (Byte* pPhrase = Encoding.GetBytes(phrase))
            {
                IntPtr pRelevantResult = searchPhraseRelDLL(InstanceNumber,
                                                            pPhrase,
                                                            Convert.ToUInt32(phrase.Length),
                                                            Convert.ToUInt32(minPage),
                                                            Convert.ToUInt32(maxPage));

                RelevantResultDLL relevantResultDLL = (RelevantResultDLL)Marshal.PtrToStructure(pRelevantResult, typeof(RelevantResultDLL));

                result.MatchWords = relevantResultDLL.MatchWords;

                for (uint i = 0; i < relevantResultDLL.CountMatches; i++)
                {
                    //get name of document
                    //getDocumentNameByIDDLL(InstanceNumber, relevantResultDLL.Matches[i], pName, DOC_NAME_LENGTH);

                    result.Results.Add(new Result { Name = relevantResultDLL.Matches[i].Name });
                }

                releaseRelevantResultDLL(InstanceNumber, pRelevantResult);

                //Marshal.Release(pRelevantResult);
            }

            return result;
        }

        public unsafe void InitSearchRel()
        {
            initSearchRelDLL(InstanceNumber);
        }

        public unsafe SearchResult SearchNewMems(DateTime startDate1,
                                                 DateTime startDate2,
                                                 DateTime endDate2)
        {
            var result = new SearchResult();

            IntPtr pRelevantResult = searchNewMemsDLL(InstanceNumber,
                                                      (uint)startDate1.Year,
                                                      (uint)startDate1.Month,
                                                      (uint)startDate2.Year,
                                                      (uint)startDate2.Month,
                                                      (uint)startDate2.Day,
                                                      (uint)endDate2.Year,
                                                      (uint)endDate2.Month,
                                                      (uint)endDate2.Day,
                                                      (uint)endDate2.Subtract(startDate2).TotalDays + 1);
            if (pRelevantResult.ToInt32() > 0)
            {
                RelevantResultDLL relevantResultDLL = (RelevantResultDLL)Marshal.PtrToStructure(pRelevantResult, typeof(RelevantResultDLL));

                result.MatchWords = relevantResultDLL.MatchWords;

                releaseRelevantResultDLL(InstanceNumber, pRelevantResult);
            }

            //Marshal.Release(pRelevantResult);


            return result;
        }

        public unsafe SearchResult SearchQuery(List<Selector> selectors,
                                               int minPage,
                                               int maxPage,
                                               int skip,
                                               bool agregateBySubject)
        {
            IntPtr[] ptrs = new IntPtr[selectors.Count];

            //allocate structures
            for (int i = 0; i < selectors.Count; i++)
            {
                var size = Marshal.SizeOf(typeof(SelectorDLL));

                ptrs[i] = Marshal.AllocHGlobal(size);

                Marshal.StructureToPtr(selectors[i].ConvertToSelectorDLL(), ptrs[i], false);
            }

            //query
            IntPtr pQueryResult = searchQueryDLL(InstanceNumber,
                                                 ptrs,
                                                 Convert.ToUInt32(selectors.Count),
                                                 Convert.ToUInt32(minPage),
                                                 Convert.ToUInt32(maxPage),
                                                 Convert.ToUInt32(skip),
                                                 agregateBySubject);

            //Console.WriteLine(pQueryResult);

            //get results
            SearchResult sr = new SearchResult();

            QueryResultDLL queryResultDLL = (QueryResultDLL)Marshal.PtrToStructure(pQueryResult, typeof(QueryResultDLL));
            int rowSize = Marshal.SizeOf(typeof(QueryRowDLL));

            byte[] nameBytes = new byte[MAX_DOC_NAME_LENGTH];

            fixed (Byte* pName = nameBytes)
            {
                for (int i = 0, j = 0; i < queryResultDLL.CountRows; i++, j += rowSize)
                {
                    QueryRowDLL queryRowDLL = (QueryRowDLL)Marshal.PtrToStructure(queryResultDLL.QueryRows + j, typeof(QueryRowDLL));

                    //getDocumentNameByIDDLL(queryRowDLL.DocNumber, pName, DOC_NAME_LENGTH);
                    string docName = Encoding.GetString(queryRowDLL.DocName).Replace("\0", "");

                    string text = docName + ";" + Encoding.GetString(queryRowDLL.Text).Replace("\0", "");

                    sr.Results.Add(new Result { Name = text });
                }
            }

            //release pointers
            for (int i = 0; i < selectors.Count; i++)
            {
                Marshal.FreeHGlobal(ptrs[i]);
            }

            //releaseQueryResult
            releaseQueryResultDLL(InstanceNumber, pQueryResult);

            return sr;
        }

        public unsafe string CalculateTrend(string phrase, int count, int minPage, int maxPage)
        {
            StringBuilder sb = new StringBuilder();

            fixed (Byte* pPhrase = Encoding.GetBytes(phrase))
            {
                fixed (uint* points = new uint[count])
                {
                    calculateTrendDLL(InstanceNumber,
                                      pPhrase,
                                      Convert.ToUInt32(phrase.Length),
                                      points,
                                      Convert.ToUInt32(count),
                                      Convert.ToUInt32(minPage),
                                      Convert.ToUInt32(maxPage));

                    for (int i = 0; i < count; i++)
                    {
                        sb.AppendFormat("{0},{1} ", i, points[i].ToString());
                    }
                }
            }

            return sb.ToString();
        }

        public unsafe SearchResult SearchHtmlSeldomWords(string text)
        {
            SearchResult sr = new SearchResult();

            fixed (Byte* pText = Encoding.GetBytes(text))
            {
                //IntPtr pRelevantResult = searchHtmlSeldomWordsDLL(pText, (System.UInt32)text.Length);

                //RelevantResultDLL relevantResultDLL = (RelevantResultDLL)Marshal.PtrToStructure(pRelevantResult, typeof(RelevantResultDLL));

                //relevantResult.MatchWords = relevantResultDLL.MatchWords;

                //releaseResultDLL(pRelevantResult);

                ////Marshal.Release(pRelevantResult);
            }

            return sr;
        }

        public FTSInstanceInfo GetInfo()
        {
            IntPtr pInfo = getInfoDLL(InstanceNumber);

            FTSInstanceInfoDLL dllInfo = (FTSInstanceInfoDLL)Marshal.PtrToStructure(pInfo, typeof(FTSInstanceInfoDLL));

            FTSInstanceInfo info = new FTSInstanceInfo();

            info.Version = dllInfo.Version;

            info.LastNameIDRAM = dllInfo.LastNameIDRAM;
            info.LastNameIDHDD = dllInfo.LastNameIDHDD;

            info.CountWordsRAM = dllInfo.CountWordsRAM;
            info.CountWordsHDD = dllInfo.CountWordsHDD;

            info.UsedMemory = dllInfo.UsedMemory;
            info.TotalMemory = dllInfo.TotalMemory;

            info.WordsHeaderBase = dllInfo.WordsHeaderBase;

            //info.DocumentNameSize = dllInfo.DocumentNameSize;

            info.LastErrorMessage = Marshal.PtrToStringAnsi(dllInfo.LastErrorMessage);
            info.HasError = dllInfo.HasError;

            info.ControlValue = dllInfo.ControlValue;

            //Marshal.Release(dllInfo.LastErrorMessage);

            //Marshal.Release(pInfo);

            return info;
        }

        /// <summary>
        /// Returns the content of a given web adress as string.
        /// </summary>
        /// <param name="Url">URL of the webpage</param>
        /// <returns>Website content</returns>
        public string DownloadWebPage(string Url, out string uri)
        {
            // Open a connection
            HttpWebRequest WebRequestObject = (HttpWebRequest)HttpWebRequest.Create(Url);

            // You can also specify additional header values like 
            // the user agent or the referer:
            WebRequestObject.UserAgent = @"";
            WebRequestObject.Referer = @"";

            // Request response:
            WebResponse Response = WebRequestObject.GetResponse();

            // Open data stream:
            Stream WebStream = Response.GetResponseStream();

            // Create reader object:
            StreamReader Reader = new StreamReader(WebStream, Encoding);

            // Read the entire stream content:
            string PageContent = Reader.ReadToEnd();

            uri = Response.ResponseUri.ToString();

            // Cleanup
            Reader.Close();
            WebStream.Close();
            Response.Close();

            return PageContent;
        }
        
        #endregion
    }
}