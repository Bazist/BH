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
        protected override IEnumerable<Document> GetDocuments(Folder folder)
        {
            return Directory.GetFiles(@"C:\FTS\TestDocs", "*.txt")
                            .Select(x => new Document(folder, x, File.ReadAllText(x)));
        }
    }
}
