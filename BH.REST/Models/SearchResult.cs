using BH.FTServer;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading.Tasks;

namespace BH.REST.Models
{
    public class SearchResult
    {
        public List<Result> Results { get; set; }
        public string MatchWords { get; set; }
        public uint FullCountMatches { get; set; }

        public static SearchResult From(FTSearch.SearchResult searchResult)
        {
            return new SearchResult
            {
                Results = searchResult.Results?.Select(x => Result.From(x)).ToList(),

                MatchWords = searchResult.MatchWords,

                FullCountMatches = searchResult.FullCountMatches
            };
        }
    }
}
