using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using System.IO;

namespace FTSearchTest
{
    public partial class SearchForm : Form
    {
        //public SearchForm(FTSearch fts)
        //{
        //    InitializeComponent();

        //    this.fts = fts;
        //}

        //FTSearch fts;

        //private void search()
        //{
        //    List<FTSearch.Result> results = fts.SearchPhrase(textBox1.Text, 0, 1000000);

        //    pnlResult.Controls.Clear();

        //    startAddDocuments();

        //    if (results.Count > 0)
        //    {
        //        addCaption("BEST MATCHES");

        //        for (int i = 0; i < results.Count && i < 100; i++)
        //        {
        //            FTSearch.Result result = results[i];

        //            addDocumentName(result.Name);

        //            if (result.Positions != null && result.Positions.Length > 0)
        //            {
        //                string text = FTSearch.ReadFileText(result.Name);

        //                foreach (FTSearch.ResultPosition rp in result.Positions)
        //                {
        //                    string subText = text.Substring(rp.StartPosition, rp.Length);

        //                    addDocumentText(subText, textBox1.Text);
        //                }
        //            }
        //        }
        //    }
        //    else
        //    {
        //        addCaption("NO RESULTS");
        //    }

        //    //if (rr.OtherMatches.Count > 0)
        //    //{
        //    //    addCaption("OTHER MATCHES");

        //    //    for (int i = 0; i < rr.OtherMatches.Count; i++)
        //    //    {
        //    //        string path = rr.OtherMatches[i];
        //    //        addDocumentName(path);
        //    //    }
        //    //}
        //}

        private int number;
        private int top;

        private void startAddDocuments()
        {
            number = 1;
            top = 0;
            pnlResult.Controls.Clear();
        }

        private void addCaption(string caption)
        {
            Label lbl = new Label();
            lbl.Name = "Label" + number.ToString();
            lbl.Text = caption;
            lbl.Width = pnlResult.Width - 30;
            lbl.Top = top;

            top += lbl.Height;

            pnlResult.Controls.Add(lbl);
        }

        private void addDocumentName(string name)
        {
            top += 15;

            LinkLabel lbl = new LinkLabel();
            lbl.Name = "LinkLabel" + number.ToString();
            lbl.Text = name;
            lbl.Width = pnlResult.Width - 30;
            lbl.Top = top;

            top += lbl.Height;

            pnlResult.Controls.Add(lbl);
        }

        private void addDocumentText(string text, string phrase)
        {
            RichTextBox rtb = new RichTextBox();
            rtb.Name = "RichTextBox" + number.ToString();
            rtb.Text = text;
            rtb.Width = pnlResult.Width - 30;
            rtb.Top = top;
            rtb.ReadOnly = true;

            top += rtb.Height;

            //selection
            string[] words = phrase.Split(' ');
            foreach (string word in words)
            {
                if (word.Length >= 4)
                {
                    string findWord;

                    if (word.Length > 8)
                    {
                        findWord = word.Substring(0, 8);
                    }
                    else
                    {
                        findWord = word;
                    }

                    int idx = 0;

                    while (idx != -1)
                    {
                        idx = rtb.Text.Replace("\r", "").IndexOf(findWord, idx + 1, StringComparison.OrdinalIgnoreCase);
                        if (idx >= 0)
                        {
                            rtb.SelectionStart = idx;
                            rtb.SelectionLength = word.Length;
                            rtb.SelectionColor = Color.FromArgb(0, 0, 255);
                            rtb.SelectionFont = new Font("Verdana", 8, FontStyle.Bold);
                        }
                    }
                }
            }

            pnlResult.Controls.Add(rtb);
        }


        private void button1_Click(object sender, EventArgs e)
        {
            //search();
        }
    }
}
