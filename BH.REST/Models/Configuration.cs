using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading.Tasks;
using static BH.FTServer.FTSearch;

namespace BH.REST.Models
{
    public class Configuration
    {
        public string IndexPath { get; set; }

        public uint InstanceNumber { get; set; }
        
        public uint MinLenWord { get; set; }
        
        public uint MaxLenWord { get; set; }
        
        public uint AutoStemmingOn { get; set; }

        public uint WordsHeaderBase { get; set; }

        public uint DocumentNameSize { get; set; }

        public uint LimitTopResults { get; set; }

        public ulong LimitUsedMemory { get; set; }

        public uint RelevantLevel { get; set; }

        public uint CountWordInPhrase { get; set; }

        public bool IsCustomPath { get; set; }

        public uint MemoryMode { get; set; }

        public bool IsUseUkranianAlphabet { get; set; }

        public bool IsUseRussianAlphabet { get; set; }

        public bool IsUseEnglishAlphabet { get; set; }

        public bool IsUseNumberAlphabet { get; set; }

        public bool IsCreateNewInstanceOnUpdate { get; set; }

        public bool AutoSaveIndex { get; set; }

        public static Configuration From(ConfigurationDLL config)
        {
            return new Configuration
            {
                IndexPath = Encoding.GetString(config.IndexPath),
                InstanceNumber = config.InstanceNumber,
                MinLenWord = config.MinLenWord,
                MaxLenWord = config.MaxLenWord,
                AutoStemmingOn = config.AutoStemmingOn,
                WordsHeaderBase = config.WordsHeaderBase,
                DocumentNameSize = config.DocumentNameSize,
                LimitTopResults = config.LimitTopResults,
                LimitUsedMemory = config.LimitUsedMemory,
                RelevantLevel = config.RelevantLevel,
                CountWordInPhrase = config.CountWordInPhrase,
                IsCustomPath = config.IsCustomPath,
                MemoryMode = config.MemoryMode,
                IsUseUkranianAlphabet = config.IsUseUkranianAlphabet,
                IsUseRussianAlphabet = config.IsUseRussianAlphabet,
                IsUseEnglishAlphabet = config.IsUseEnglishAlphabet,
                IsUseNumberAlphabet = config.IsUseNumberAlphabet,
                IsCreateNewInstanceOnUpdate = config.IsCreateNewInstanceOnUpdate,
                AutoSaveIndex = config.AutoSaveIndex
            };
        }

        public ConfigurationDLL ToConfigurationDLL()
        {
            return new ConfigurationDLL
            {
                IndexPath = Encoding.GetBytes(this.IndexPath),
                InstanceNumber = this.InstanceNumber,
                MinLenWord = this.MinLenWord,
                MaxLenWord = this.MaxLenWord,
                AutoStemmingOn = this.AutoStemmingOn,
                WordsHeaderBase = this.WordsHeaderBase,
                DocumentNameSize = this.DocumentNameSize,
                LimitTopResults = this.LimitTopResults,
                LimitUsedMemory = this.LimitUsedMemory,
                RelevantLevel = this.RelevantLevel,
                CountWordInPhrase = this.CountWordInPhrase,
                IsCustomPath = this.IsCustomPath,
                MemoryMode = this.MemoryMode,
                IsUseUkranianAlphabet = this.IsUseUkranianAlphabet,
                IsUseRussianAlphabet = this.IsUseRussianAlphabet,
                IsUseEnglishAlphabet = this.IsUseEnglishAlphabet,
                IsUseNumberAlphabet = this.IsUseNumberAlphabet,
                IsCreateNewInstanceOnUpdate = this.IsCreateNewInstanceOnUpdate,
                AutoSaveIndex = this.AutoSaveIndex
            };
        }
    }
}
