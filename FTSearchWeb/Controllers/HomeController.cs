using FTSearchWeb.Model;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text.RegularExpressions;
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

            if (!fts.IsStarted())
            {
                fts.Start(0);
            }

            return View(fts.GetInfo());
        }

        public ActionResult Contact()
        {
            ViewBag.Message = "Your contact page.";

            return View();
        }

        public ActionResult Search(SearchResult result)
        {
            if (ModelState.IsValid)
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

                if (string.IsNullOrEmpty(result.Phrase))
                {
                    result.Phrase = Request.Params["t"];
                }

                if (string.IsNullOrEmpty(result.TemplateName))
                {
                    result.TemplateName = Request.Params["tn"];
                }

                BH.FTServiceClient fts = new BH.FTServiceClient();

                if (!fts.IsStarted())
                {
                    fts.Start(0);
                }

                var f = Request.Params["f"];

                if (string.IsNullOrEmpty(f)) //search phrase
                {
                    BH.FTSearchResult[] res = new BH.FTSearchResult[0];

                    if (!string.IsNullOrEmpty(result.Phrase))
                    {
                        res = fts.SearchPhrase(result.Phrase, (result.TemplateName ?? string.Empty).Trim(), (int.Parse(cp) - 1) * SearchResult.PAGE_SIZE, SearchResult.PAGE_SIZE);
                    }

                    return View("Index", new SearchResult
                    {
                        Phrase = result.Phrase,
                        Results = res,
                        StartPage = int.Parse(sp),
                        CurrentPage = int.Parse(cp)
                    });
                }
                else //open file
                {
                    ViewBag.Title = f;
                    ViewBag.FileName = f;

                    var content = fts.LoadContent(f, result.Phrase);

                    content = content.Replace("[BREAK]",
                                              "<br/><br/>================= BREAK =====================<br/>");

                    content = content.Replace("[TooManyMatches]",
                                              "<br/><br/>================= FILE CANNOT BE LOAD FULL IN WEB ===================== <br/>");

                    content = content.Replace("<", "&lt;").Replace(">", "&gt;");

                    content = content.Replace(" ", "&nbsp;");

                    content = content.Replace("\n", "<br/>");

                    var words = result.Phrase.Split(new char[] { ' ' }, StringSplitOptions.RemoveEmptyEntries);

                    foreach (var word in words)
                    {
                        content = Regex.Replace(content, word, "<span style='background-color:yellow'>" + word + "</span>", RegexOptions.IgnoreCase);
                    }

                    ViewBag.Content = content;

                    return View("File");
                }
            }
            else
            {
                return View("Index", new SearchResult { StartPage = 1, CurrentPage = 1 });
            }
        }
    }
}