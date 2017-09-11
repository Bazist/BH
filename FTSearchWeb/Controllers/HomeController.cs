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
            
            return View("Index", new SearchResult { StartPage = 1, CurrentPage = 1 });
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
            string sp = Request.Params["sp"];

            if (string.IsNullOrEmpty(sp))
            {
                sp = "1";
            }

            string cp = Request.Params["cp"];

            if (string.IsNullOrEmpty(cp))
            {
                cp = "1";
            }

            if(string.IsNullOrEmpty(phrase))
            {
                phrase = Request.Params["t"];
            }
            
            BH.FTServiceClient fts = new BH.FTServiceClient();

            if(!fts.IsStarted())
            {
                fts.Start(0);
            }
            
            var res = fts.SearchPhrase(phrase, (int.Parse(cp) - 1) * SearchResult.PAGE_SIZE, SearchResult.PAGE_SIZE);
            
            return View("Index", new SearchResult { Phrase = phrase,
                                                    Results = res,
                                                    StartPage = int.Parse(sp),
                                                    CurrentPage = int.Parse(cp) });
        }
    }
}