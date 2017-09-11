using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading.Tasks;

namespace FTSearchWeb.Model
{
    public class SearchResult
    {
        public string Phrase { get; set; }
        public BH.FTSearchResult[] Results { get; set; }
    }
}
