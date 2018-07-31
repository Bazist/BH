using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using BH.BaseRobot;
using System.IO;

namespace BH.ExampleRobot
{
    public class ExampleRobot : BaseRobot.BaseRobot
    {
        private const string DocumentsPath = @"C:\FTS\TestDocs";

        protected override IEnumerable<Document> GetDocuments(Folder folder)
        {
            if (Directory.Exists(DocumentsPath))
            {
                return Directory.GetFiles(DocumentsPath, "*.txt")
                            .Select(x => new Document(folder, x, File.ReadAllText(x)));
            }
            else
            {
                throw new DirectoryNotFoundException(DocumentsPath);
            }
        }
    }
}
