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
            return new Document[]
            {
                new Document(folder, "GreenDoc", "Green"),
                new Document(folder, "RedDoc", "Red"),
                new Document(folder, "YelloDoc", "Yellow"),
            };
        }
    }
}
