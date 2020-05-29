﻿/*
# Copyright(C) 2010-2020 Viacheslav Makoveichuk (email: slv709@gmail.com, skype: vyacheslavm81)
# This file is part of Black Hole FTS.
#
# Black Hole FTS is free software : you can redistribute it and / or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# Black Hole FTS is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with Foobar.  If not, see <http://www.gnu.org/licenses/>.
*/

using BH.Web.Model;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text.RegularExpressions;
using System.Web;
using System.Web.Mvc;

namespace BH.Web.Controllers
{
    public class HomeController : Controller
    {
        public ActionResult Index()
        {
            WebService.FTServiceClient fts = new WebService.FTServiceClient();
            
            return View("Index", new SearchResult { StartPage = 1, CurrentPage = 1 });
        }

        public ActionResult Info()
        {
            WebService.FTServiceClient fts = new WebService.FTServiceClient();

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

                WebService.FTServiceClient fts = new WebService.FTServiceClient();

                if (!fts.IsStarted())
                {
                    fts.Start(0);
                }
                
                var f = Request.Params["f"];

                if (string.IsNullOrEmpty(f)) //search phrase
                {
                    WebService.FTSearchSearchResult res = new WebService.FTSearchSearchResult();

                    if (!string.IsNullOrEmpty(result.Phrase))
                    {
                        res = fts.SearchPhrase(result.Phrase,
                                              (result.TemplateName ?? string.Empty).Trim(),
                                              (int.Parse(cp) - 1) * SearchResult.PAGE_SIZE,
                                              SearchResult.PAGE_SIZE);
                    }

                    return View("Index", new SearchResult
                    {
                        Phrase = result.Phrase,
                        Results = res.Results,
                        StartPage = int.Parse(sp),
                        CurrentPage = int.Parse(cp)
                    });
                }
                else //open file
                {
                    ViewBag.Title = f;
                    ViewBag.FileName = f;

                    //TODO Implement load content
                    var content = string.Empty; //fts.LoadContent(f, result.Phrase);

                    content = content.Replace("<", "&lt;").Replace(">", "&gt;");

                    content = content.Replace(" ", "&nbsp;");

                    content = content.Replace("\n", "<br/>");

                    content = content.Replace("[BREAK]",
                                              "<br/><br/>================= BREAK =====================<br/>");

                    content = content.Replace("[TooManyMatches]",
                                              "<br/><br/>================= FILE CANNOT BE LOAD FULL IN WEB ===================== <br/>");
                    
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