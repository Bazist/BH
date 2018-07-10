using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace BH.BaseRobot
{
    public class File
    {
        public File(Directory directory,
                    string name,
                    string content = null)
        {
            Directory = directory;
            Name = name;
            Content = content;
        }

        public Directory Directory { get; private set; }

        public string Name { get; }

        public string Content { get; }

        public string FullPath => $"{Directory.FullPath}\\{Name}";

        public string Version { get; set; }

        public override string ToString()
        {
            return FullPath;
        }
    }
}
