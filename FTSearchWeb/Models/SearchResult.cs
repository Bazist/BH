﻿using System;
using System.Collections.Generic;
using System.Globalization;
using System.Linq;
using System.Threading.Tasks;

namespace FTSearchWeb.Model
{
    public class SearchResult
    {
        public const int PAGE_SIZE = 25;

        public int StartPage { get; set; }

        public int CurrentPage { get; set; }

        public string Phrase { get; set; }
        public BH.FTSearchResult[] Results { get; set; }

        public static string FormatSize(ulong size)
        {
            return size.ToString("N1", CultureInfo.InvariantCulture).Replace(".0", string.Empty);
        }
    }
}
