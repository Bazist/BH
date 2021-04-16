using BH.FTServer;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading.Tasks;

namespace BH.REST.Models
{
    public struct Info
    {
        public uint Version { get; set; }

        public uint AmountInstances { get; set; }

        public ulong IndexSize { get; set; }

        public ulong TextSize { get; set; }

        public uint LastNameIDRAM { get; set; }

        public uint LastNameIDHDD { get; set; }

        public ulong CountWordsRAM { get; set; }

        public ulong CountWordsHDD { get; set; }

        public ulong UsedMemory { get; set; }

        public ulong TotalMemory { get; set; }

        public uint WordsHeaderBase { get; set; }

        public uint DocumentNameSize { get; set; }

        public string LastErrorMessage { get; set; }

        public bool HasError { get; set; }

        public uint RelevantLevel { get; set; }

        public static Info From(FTService.Info info)
        {
            return new Info
            {
                Version = info.Version,
                AmountInstances = info.AmountInstances,
                IndexSize = info.IndexSize,
                TextSize = info.TextSize,
                LastNameIDRAM = info.LastNameIDRAM,
                LastNameIDHDD = info.LastNameIDHDD,
                CountWordsRAM = info.CountWordsRAM,
                CountWordsHDD = info.CountWordsHDD,
                UsedMemory = info.UsedMemory,
                TotalMemory = info.TotalMemory,
                WordsHeaderBase = info.WordsHeaderBase,
                DocumentNameSize = info.DocumentNameSize,
                LastErrorMessage = info.LastErrorMessage,
                HasError = info.HasError,
                RelevantLevel = info.RelevantLevel
            };
        }
    }
}
