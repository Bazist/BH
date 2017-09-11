using FTSearchWeb.Model;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Web;
using System.Web.Mvc;

namespace FTSearchWeb.Controllers
{
    public class HomeController : Controller
    {
        public ActionResult Index()
        {
            BH.FTServiceClient fts = new BH.FTServiceClient();
            
            return View("Index", new SearchResult { });
        }

        public ActionResult Info()
        {
            BH.FTServiceClient fts = new BH.FTServiceClient();

            return View(fts.GetInfo());
        }

        public ActionResult Contact()
        {
            ViewBag.Message = "Your contact page.";

            return View();
        }

        public ActionResult Search(string phrase)
        {
            BH.FTServiceClient fts = new BH.FTServiceClient();
            var res = fts.SearchPhrase(phrase);
            
            return View("Index", new SearchResult { Phrase = phrase, Results = res });
        }
    }
}