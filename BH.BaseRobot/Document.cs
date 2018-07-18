using BH.WCF;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace BH.BaseRobot
{
    public class Document
    {
        public Document(Folder folder,
                        string name,
                        string content = null)
        {
            Folder = folder;
            Name = name;
            Content = content;
        }

        public Folder Folder { get; private set; }

        public string Name { get; }

        public string Content { get; set; }

        public bool HasContent => !string.IsNullOrEmpty(Content);

        public string FullPath => $"{Folder.FullPath}\\{Name}";

        public string Version { get; set; }

        public override string ToString()
        {
            return FullPath;
        }

        public FTService.DocumentName ToDocumentName()
        {
            return new FTService.DocumentName { Name = Name, Version = Version };
        }
    }
}
