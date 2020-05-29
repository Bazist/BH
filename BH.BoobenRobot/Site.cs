/*
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

using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Net;
using System.IO;
using System.Threading;
using System.Text.RegularExpressions;
using System.Xml;
using BH.WCF;

namespace BH.BoobenRobot
{
    public abstract class Site
    {
        public Site(FTService service)
        {
            _service = service;

            //by default
            SiteEncoding = Encoding.UTF8;
            PageDelay = TimeSpan.FromSeconds(1);
            SiteDelay = TimeSpan.FromHours(4);
            ErrorDelay = TimeSpan.FromMinutes(15);
        }

        public const string FTRobot_PATH = @"C:\FTRobot\";

        private FTService _service;

        //public Dictionary<string, string> Labels = new Dictionary<string, string>();

        public string Code { get; set; }

        public Encoding SiteEncoding;

        public TimeSpan PageDelay;

        public TimeSpan SiteDelay;

        public TimeSpan ErrorDelay;

        public string BaseUrl;

        private string GetDirectoryPath(string docNumber)
        {
            return FTRobot_PATH + (int.Parse(docNumber) - int.Parse(docNumber) % 1000).ToString() + @"\";
        }

        protected abstract List<string> GetDocNumberByUrl(string url);

        protected abstract string GetUrlByDocNumber(string docNumber, int pageNumber, string dashboardID);

        protected virtual string GetDocCode(string code, string dashboardId, string docNumber, int pageNumber)
        {
            if (string.IsNullOrEmpty(dashboardId))
            {
                return code + "-" + int.Parse(docNumber).ToString() + "-" + pageNumber.ToString();
            }
            else
            {
                return code + "_" + dashboardId + "-" + int.Parse(docNumber).ToString() + "-" + pageNumber.ToString();
            }
        }

        protected string GetFilePath(string code, string dashboardID, string docNumber, int pageNumber)
        {
            return GetDirectoryPath(docNumber) + GetDocCode(code, dashboardID, docNumber, pageNumber) + ".txt";
        }

        public void CheckLabelAndAddPage(List<Page> pages, string url, string label, string dashboardID = null)
        {
            if (!DBHelpers.HasLabel(url, label))
            {
                List<string> docNumber = GetDocNumberByUrl(url);

                if (docNumber != null && docNumber.Count > 0)
                {
                    int pageNumber = 1;

                    while (true)
                    {
                        string filePath = GetFilePath(Code, dashboardID, docNumber[0], pageNumber + 1);

                        if (!File.Exists(filePath))
                        {
                            break;
                        }
                        else
                        {
                            pageNumber++;
                        }
                    }

                    pages.Add(new Page()
                    {
                        DashboardURL = url,
                        DashboardID = dashboardID,
                        URL = GetUrlByDocNumber(docNumber[0], pageNumber, dashboardID),
                        DocNumber = docNumber[0],
                        PageNumber = pageNumber
                    });

                    DBHelpers.SaveLabel(this.Code, url, label);
                }
            }
        }

        protected abstract List<Page> GetDashboards();

        protected List<Page> GetDashboards(string mainDashboardUrl, string baseUrl, string startUrl)
        {
            Page mainPage = new Page();
            mainPage.URL = mainDashboardUrl;

            DownloadWebPage(mainPage);

            List<Page> pages = new List<Page>();

            int idx = 0;

            while (true)
            {
                idx = mainPage.HtmlContent.IndexOf(startUrl, idx);

                if (idx >= 0)
                {
                    string partDoc = mainPage.HtmlContent.Substring(startUrl.Length, 16);

                    List<string> nums = ExtractByRegexp(partDoc, "(?<num>[0-9]+");

                    if (nums.Count > 0)
                    {
                        pages.Add(new Page() { URL = baseUrl + startUrl + nums[0] });
                    }

                    idx += startUrl.Length;
                }
                else
                {
                    break;
                }
            }

            return pages;
        }

        protected abstract List<Page> OnDashboardLoaded(Page page);

        protected abstract void OnPageLoaded(Page page);

        protected List<string> GetParts(string text, string startTag, string endTag)
        {
            List<string> parts = new List<string>();

            int idx1 = -1;
            int idx2 = 0;

            while (true)
            {
                idx1 = text.IndexOf(startTag, idx1 + 1);
                if (idx1 >= 0)
                {
                    idx2 = text.IndexOf(endTag, idx1 + startTag.Length + 1);

                    if (idx2 >= 0)
                    {
                        parts.Add(text.Substring(idx1 + startTag.Length, idx2 - idx1 - startTag.Length).Trim());

                        idx1 = idx2;
                    }
                    else
                    {
                        break;
                    }
                }
                else
                {
                    break;
                }
            }

            return parts;
        }

        protected string GetMessages(string tag1,
                                     string tag2,
                                     string tag,
                                     string text)
        {
            int countMessages;

            return GetMessages(tag1, tag2, tag, text, out countMessages);
        }

        protected string GetMessages(string tag1,
                                     string tag2,
                                     string tag,
                                     string text,
                                     out int countMessages)
        {
            countMessages = 0;

            string startTag = "<" + tag;
            string endTag = "</" + tag + ">";

            StringBuilder sb = new StringBuilder();

            int startIndex = 0;

            while (true)
            {
                int idx1 = text.IndexOf(tag1, startIndex);

                if (idx1 >= 0)
                {
                    idx1 += tag1.Length;

                    //idx1 = text.IndexOf(">", idx1);

                    int level = 0;
                    int idx2 = idx1;

                    for (int i = idx1; i <= text.Length - endTag.Length; i++)
                    {
                        bool bFind = true;
                        for (int j = 0; j < startTag.Length; j++)
                        {
                            if (text[i + j] != startTag[j])
                            {
                                bFind = false;

                                break;
                            }
                        }

                        if (bFind)
                        {
                            level++;
                        }
                        else
                        {
                            //bbb
                            bFind = true;
                            for (int j = 0; j < endTag.Length; j++)
                            {
                                if (text[i + j] != endTag[j])
                                {
                                    bFind = false;

                                    break;
                                }
                            }

                            if (bFind)
                            {
                                level--;
                            }
                        }

                        if (level == -1)
                        {
                            idx2 = i;

                            break;
                        }
                    }

                    sb.Append(" ");
                    sb.Append(text.Substring(idx1, idx2 - idx1));

                    startIndex = idx1;
                }
                else
                {
                    break;
                }

                countMessages++;
            }

            return sb.ToString();
        }

        protected List<string> ExtractByRegexp(string text,
                                               string pattern,
                                               string groupName = "num")
        {
            Regex regex = new Regex(pattern, RegexOptions.IgnoreCase);

            MatchCollection mc = regex.Matches(text);

            List<string> res = new List<string>();

            foreach (Match m in mc)
            {
                if (m.Success && m.Groups.Count > 0)
                {
                    res.Add(m.Groups[groupName].Value);
                }
            }

            return res;
        }

        protected List<string> GetTagsByAttr(string text, string attrText)
        {
            List<string> parts = new List<string>();

            int idx = 0;

            while (true)
            {
                idx = text.IndexOf(attrText, idx + 1);
                if (idx >= 0)
                {
                    int idx1 = idx - 1;

                    while (true)
                    {
                        if (text[idx1] == '<')
                            break;

                        idx1--;
                    }

                    int idx2 = text.IndexOf(">", idx + attrText.Length + 1);

                    parts.Add(text.Substring(idx1, idx2 - idx1 + 1));
                }
                else
                {
                    break;
                }

                idx += attrText.Length + 1;
            }

            return parts;
        }

        public static Encoding ASC2Encoding = Encoding.GetEncoding("windows-1251");

        private void WriteFile(Page page)
        {
            string dirPath = GetDirectoryPath(page.DocNumber);

            if (!Directory.Exists(dirPath))
            {
                Directory.CreateDirectory(dirPath);
            }

            page.FilePath = GetFilePath(Code, page.DashboardID, page.DocNumber, page.PageNumber);

            File.WriteAllText(page.FilePath, page.FileContent, ASC2Encoding);
        }

        private string GetTitleWords(string title)
        {
            MatchCollection mc = Regex.Matches(title, "(?<word>[a-zA-Zа-яА-Я0-9]{3,})");

            string titleWords = string.Empty;

            foreach (Match m in mc)
            {
                if (m.Success)
                {
                    titleWords += "_" + m.Groups["word"].Value.ToLower() + " ";
                }
            }

            return titleWords;
        }

        private void AddTags(Page page)
        {
            //title
            string title = GetMessages("<title>", "</title>", "title", page.HtmlContent);
            page.FileContent += " <title>" + title + "</title>";

            page.FileContent += "<tw>" + GetTitleWords(title) + "</tw>";

            //tags
            page.FileContent += GetTags(page);
        }

        //private static object FileLocker = new object();

        //private void WriteLog(string operation, string text)
        //{
        //    lock (FileLocker)
        //    {
        //        string str = string.Format("[{0}] {1} -> {2}\r\n", DateTime.Now.ToString(), operation, text);

        //        string logPath = @"Site.FTRobot_PATH\Log.txt";

        //        File.AppendAllText(logPath, str);
        //    }
        //}

        private void WriteLog(string filePath)
        {
            //lock (FileLocker)
            //{
            //    string logPath = @"d:\BH.BoobenRobot\Log.txt";

            //    File.AppendAllText(logPath, text + "\r\n");
            //}

            DBHelpers.SavePage(this.Code, filePath.Replace(Site.FTRobot_PATH, string.Empty));
        }

        private void ProcessPage(Page firstPage, Page page)
        {
            //WriteLog("Download page", page.URL);
            int count = 0;

            while (true)
            {
                try
                {
                    LastErrorMessage = null;

                    this.DownloadWebPage(page);

                    break;
                }
                catch (Exception ex)
                {
                    //if (ex.Message.IndexOf("404") >= 0 || ex.Message.IndexOf("403") >= 0)
                    {
                        page.HtmlContent = string.Empty;
                        page.FileContent = string.Empty;
                        page.NeedLoadNextPage = false;

                        return; //skip
                    }
                    
                    //if first page could be loaded, then troubles only with this page. Skip.
                    if (firstPage != page)
                    {
                        try
                        {
                            this.DownloadWebPage(firstPage);

                            page.HtmlContent = string.Empty;
                            page.FileContent = string.Empty;
                            page.NeedLoadNextPage = false;

                            return;
                        }
                        catch
                        {
                            //wait until site will be available
                        }
                    }

                    this.NextUpdate = DateTime.Now.Add(this.ErrorDelay);

                    this.LastErrorMessage = ex.Message + " Attempt: " + (count + 1).ToString();

                    Thread.Sleep(this.ErrorDelay);

                    if (count < 3)
                    {
                        count++;
                    }
                    else
                    {
                        throw;
                    }
                }
            }

            this.OnPageLoaded(page);

            if (page.FileContent.Length > 64000) //64K max
            {
                page.FileContent = page.FileContent.Substring(0, 64000);
            }

            if (page.FileContent.Trim().Length > 3) //do not index empty pages
            {
                this.AddTags(page);

                this.WriteFile(page);

                this.WriteLog(page.FilePath);

                //index
                //((BasicHttpBinding)client.Endpoint.Binding).MaxReceivedMessageSize = int.MaxValue;

                //System.ServiceModel.Channels.Binding binding = client.Endpoint.Binding;

                //XmlDictionaryReaderQuotas myReaderQuotas = new XmlDictionaryReaderQuotas();
                //myReaderQuotas.MaxStringContentLength = int.MaxValue;
                //myReaderQuotas.MaxArrayLength = int.MaxValue;
                //myReaderQuotas.MaxBytesPerRead = int.MaxValue;
                //myReaderQuotas.MaxDepth = int.MaxValue;
                //myReaderQuotas.MaxNameTableCharCount = int.MaxValue;

                //client.Endpoint.Binding.GetType().GetProperty("ReaderQuotas").SetValue(binding, myReaderQuotas, null);

                string docCode = GetDocCode(Code, page.DashboardID, page.DocNumber, page.PageNumber);

                try
                {
                    if (_service != null)
                    {
                        _service.IndexText(docCode, null, page.FileContent, null);
                    }
                }
                catch
                {
                    //nothing;
                }
            }
        }

        private static object _saveIndexLocker = new object();
        private static int _amountUpdated = 0;
        public static List<Site> Sites;

        private void WaitNextUpdate(bool isError)
        {
            if (!isError)
            {
                this.NextUpdate = DateTime.Now.Add(this.SiteDelay);
                this.CurrentPage = null;

                //lock (_saveIndexLocker)
                //{
                //    _amountUpdated++;

                //    if (_amountUpdated >= Sites.Count)
                //    {
                //        //FTSearchService7.FTSearchServiceClient client = new FTSearchService7.FTSearchServiceClient();
                //        //client.SaveIndex();

                //        _amountUpdated = 0;
                //    }
                //}

                while(DateTime.Now < this.NextUpdate)
                {
                    Thread.Sleep(TimeSpan.FromSeconds(5));
                }
            }
            else
            {
                this.NextUpdate = DateTime.Now.Add(this.ErrorDelay);
                this.CurrentPage = null;

                Thread.Sleep(this.ErrorDelay);
            }
        }

        public Page CurrentPage
        {
            get;
            set;
        }

        public int AmountPages;

        public long TotalSize;

        public string LastErrorMessage;

        public string Progress;

        public DateTime NextUpdate = DateTime.Now;

        public void Run()
        {
            while (true)
            {
                //this.LastErrorMessage = null;

                List<Page> pages = new List<Page>();

                try
                {
                    //dashboards
                    List<Page> dashboards = GetDashboards();

                    //WriteLog("Download dashboards", dashboards.Count.ToString());

                    foreach (Page page in dashboards)
                    {
                        this.CurrentPage = page;
                        this.Progress = "Dashboard: " + (dashboards.IndexOf(page) + 1).ToString() + " of " + dashboards.Count;

                        try
                        {
                            this.DownloadWebPage(page);
                        }
                        catch (Exception ex)
                        {
                            this.LastErrorMessage = ex.Message;

                            Thread.Sleep(this.PageDelay);

                            continue;
                        }

                        this.TotalSize += page.HtmlContent.Length;
                        this.AmountPages++;

                        pages.AddRange(this.OnDashboardLoaded(page));

                        Thread.Sleep(this.PageDelay);
                    }

                    //WriteLog("Process pages", pages.Count.ToString());
                }
                catch (Exception ex)
                {
                    if (this.CurrentPage != null)
                    {
                        //clear labels
                        for (int i = 0; i < pages.Count; i++)
                        {
                            //lock (Labels)
                            //{
                            //    Labels[pages[i].DashboardURL] = string.Empty;
                            //}

                            DBHelpers.SaveLabel(this.Code, pages[i].DashboardURL, string.Empty);
                        }

                        this.LastErrorMessage = string.Format("Failed page: {0};{1}",
                                                              this.GetUrlByDocNumber(this.CurrentPage.DocNumber,
                                                                                     this.CurrentPage.PageNumber,
                                                                                     this.CurrentPage.DashboardID),
                                                              ex.Message);

                        //MessageBox.Show(ex.Message + this.CurrentPage.ToString() + ex.StackTrace);
                    }
                    else
                    {
                        this.LastErrorMessage = ex.Message;
                        //MessageBox.Show(ex.Message + ex.StackTrace);
                    }

                    this.WaitNextUpdate(true);

                    continue;
                }

                try
                {
                    //pages
                    for (int i = 0; i < pages.Count; i++)
                    {
                        Page page = pages[i];

                        this.CurrentPage = page;
                        this.Progress = "Article: " + (i + 1).ToString() + " of " + pages.Count;

                        this.ProcessPage(pages[0], page);

                        this.TotalSize += page.HtmlContent.Length;
                        this.AmountPages++;

                        while (page.NeedLoadNextPage)
                        {
                            page.PageNumber++;

                            if (page.PageNumber > 5000)
                            {
                                break;
                            }

                            page.URL = GetUrlByDocNumber(page.DocNumber, page.PageNumber, page.DashboardID);

                            this.CurrentPage = page;

                            this.ProcessPage(pages[0], page);

                            if (page.URL != page.RedirectURL) //avoid cycles
                            {
                                break;
                            }

                            this.TotalSize += page.HtmlContent.Length;
                            this.AmountPages++;

                            Thread.Sleep(this.PageDelay);
                        }

                        Thread.Sleep(this.PageDelay);
                    }

                    //WriteLog("Site " + Code + " sleep to ", DateTime.Now.Add(this.SiteDelay).ToString());

                    //this.SaveLabels();

                    this.WaitNextUpdate(false);
                }
                catch (Exception ex)
                {
                    //WriteLog("Exception in site " + Code, ex.Message + ex.StackTrace);
                    //WriteLog("Site " + Code + " sleep to ", DateTime.Now.Add(this.SiteDelay).ToString());

                    if (this.CurrentPage != null)
                    {
                        //clear labels
                        for (int i = pages.IndexOf(this.CurrentPage); i < pages.Count; i++)
                        {
                            //lock (Labels)
                            //{
                            //    Labels[pages[i].DashboardURL] = string.Empty;
                            //}

                            DBHelpers.SaveLabel(this.Code, pages[i].DashboardURL, string.Empty);
                        }

                        this.LastErrorMessage = string.Format("Failed page: {0};{1}",
                                                              this.GetUrlByDocNumber(this.CurrentPage.DocNumber,
                                                                                     this.CurrentPage.PageNumber,
                                                                                     this.CurrentPage.DashboardID),
                                                              ex.Message);

                        //MessageBox.Show(ex.Message + this.CurrentPage.ToString() + ex.StackTrace);
                    }
                    else
                    {
                        this.LastErrorMessage = ex.Message;

                        //MessageBox.Show(ex.Message + ex.StackTrace);
                    }

                    this.WaitNextUpdate(true);
                }
            }
        }

        private void DownloadWebPage(Page page)
        {
            // Open a connection
            HttpWebRequest WebRequestObject = (HttpWebRequest)HttpWebRequest.Create(page.URL);

            // You can also specify additional header values like 
            // the user agent or the referer:
            WebRequestObject.UserAgent = @"Booben.Com";
            WebRequestObject.Referer = @"";

            // Request response:
            WebResponse Response = WebRequestObject.GetResponse();

            // Open data stream:
            Stream WebStream = Response.GetResponseStream();

            // Create reader object:
            StreamReader Reader = new StreamReader(WebStream, this.SiteEncoding);

            // Read the entire stream content:
            string PageContent = Reader.ReadToEnd();

            page.RedirectURL = Response.ResponseUri.ToString();

            // Cleanup
            Reader.Close();
            WebStream.Close();
            Response.Close();

            page.HtmlContent = PageContent;
        }

        protected string GetTags(Page page)
        {
            StringBuilder sb = new StringBuilder();

            sb.Append(" ");

            //year
            sb.Append("y");
            sb.Append(DateTime.Now.Year.ToString("0000"));
            sb.Append(" ");

            //month
            sb.Append("m");
            sb.Append(DateTime.Now.Month.ToString("00"));
            sb.Append(" ");

            //week
            sb.Append("w");
            sb.Append((DateTime.Now.DayOfYear / 7 + 1).ToString("00"));
            sb.Append(" ");

            //day
            sb.Append("d");
            sb.Append(DateTime.Now.Day.ToString("00"));
            sb.Append(" ");

            //hour
            sb.Append("h");
            sb.Append(DateTime.Now.Hour.ToString("00"));
            sb.Append(" ");

            //minute
            sb.Append("mi");
            sb.Append(DateTime.Now.Minute.ToString("00"));
            sb.Append(" ");

            //length
            sb.Append("l");
            sb.Append(page.FileContent.Length.ToString("0000000"));
            sb.Append(" ");

            //docno
            sb.Append("n");
            sb.Append(int.Parse(page.DocNumber).ToString("0000000"));
            sb.Append(" ");

            //page
            sb.Append("p");
            sb.Append(page.PageNumber.ToString("0000000"));
            sb.Append(" ");

            //site
            sb.Append("s");
            sb.Append(Code);
            sb.Append(" ");

            if (page.CountMessages > 0)
            {
                //count messages
                sb.Append("c");
                sb.Append(page.CountMessages.ToString("0000000"));
                sb.Append(" ");
            }

            //dashboard id
            int dashId;
            if (int.TryParse(page.DashboardID, out dashId))
            {
                sb.Append("i");
                sb.Append(dashId.ToString("0000000"));
                sb.Append(" ");
            }

            //has big gif
            if (HasBigGif(page))
            {
                sb.Append("bgif");
                sb.Append(" ");
            }

            return sb.ToString();
        }

        public bool HasBigGif(Page page)
        {
            Regex reg = new Regex("<img[^!]+src=[\"'](?<href>http[s]?:[^\"']+\\.gif)[\"']");

            MatchCollection mc = reg.Matches(page.FileContent);

            foreach (Match match in mc)
            {
                string href = match.Groups["href"].Value;

                if (!href.Contains(BaseUrl) &&
                    !href.Contains("forumimg.net") &&
                    !href.Contains("smil")) //avoid smiles
                {

                    if (GetSize(href) > 1024 * 1024) //only big gifs
                    {
                        return true;
                    }
                }
            }

            return false;
        }

        public long GetSize(string url)
        {
            try
            {
                // Open a connection
                HttpWebRequest WebRequestObject = (HttpWebRequest)HttpWebRequest.Create(url);

                // You can also specify additional header values like 
                // the user agent or the referer:
                WebRequestObject.UserAgent = @"Booben.Com";
                WebRequestObject.Referer = @"";

                // Request response:
                WebResponse Response = WebRequestObject.GetResponse();

                long length = Response.ContentLength;

                Response.Close();

                return length;
            }
            catch
            {
                return 0;
            }
        }

        //public void LoadLabels()
        //{
        //    //read labels
        //    string path = BH.BoobenRobot.Site.BH.BoobenRobot_PATH + "labels_" + this.Code + ".txt";

        //    if (File.Exists(path))
        //    {
        //        this.Labels.Clear();

        //        StreamReader sr = new StreamReader(path);

        //        while (!sr.EndOfStream)
        //        {
        //            string[] lbl = sr.ReadLine().Split(';');

        //            this.Labels.Add(lbl[0], lbl[1]);
        //        }

        //        sr.Close();
        //    }
        //}

        //public void SaveLabels()
        //{
        //    //save labels
        //    lock (this.Labels)
        //    {
        //        if (this.Labels.Count > 0)
        //        {
        //            var lbl = this.Labels.GetEnumerator();

        //            StreamWriter sw = new StreamWriter(BH.BoobenRobot.Site.BH.BoobenRobot_PATH + "labels_" + this.Code + ".txt");

        //            while (lbl.MoveNext())
        //            {
        //                sw.WriteLine(lbl.Current.Key + ";" + lbl.Current.Value);
        //            }

        //            sw.Close();
        //        }
        //    }
        //}
    }
}
