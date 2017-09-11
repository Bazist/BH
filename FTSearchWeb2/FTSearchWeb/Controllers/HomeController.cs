using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading.Tasks;
using Microsoft.AspNetCore.Mvc;
using FTSearchWeb.Model;

namespace FTSearchWeb.Controllers
{
    public class HomeController : Controller
    {
        public IActionResult Index()
        {
            return View();
        }

        public IActionResult About()
        {
            //ViewData["Message"] = "Your application description page.";

            return View();
        }

        public IActionResult Search(string phrase)
        {
            BH.FTServiceClient fts = new BH.FTServiceClient();
            var res = fts.SearchPhraseAsync(phrase);
            res.Wait();
            
            return View("Index", new SearchResult { Phrase = phrase, Results = res.Result });
        }

        //public IActionResult Error()
        //{
        //    return View();
        //}
    }
}
