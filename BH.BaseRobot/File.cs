using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace BH.BaseRobot
{
    public class File
    {
        private File()
        {
        }

        public static File CreateFile(Directory directory,
                                      string name,
                                      string content = null)
        {
            return new File
            {
                Directory = directory,
                Name = name,
                Content = content
            };
        }

        public Directory Directory { get; private set; }

        public string FullPath
        {
            get
            {
                return $"{Directory.FullPath}\\{Name}";
            }
        }

        public string Name { get; private set; }

        public string Content { get; private set; }

        public string Version { get; set; }

        public override string ToString()
        {
            return FullPath;
        }
    }
}
