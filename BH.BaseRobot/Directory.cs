using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace BH.BaseRobot
{
    public class Directory
    {
        private Directory()
        {
        }

        public static Directory CreateDirectory(string name,
                                                Directory parent = null)
        {
            return new Directory
            {
                Name = name,
                Parent = parent
            };
        }

        public string Name { get; private set; }

        public Directory Parent { get; private set; }

        public static Directory DefaultRoot => new Directory { Name = "[Default Root]" };

        public string FullPath
        {
            get
            {
                return string.Join("\\", Ancestors.Reverse()
                                                  .Select(x => x.Name)
                                                  .Union(new[] { Name }));
            }
        }

        public IEnumerable<Directory> Ancestors
        {
            get
            {
                var parent = Parent;
                while (parent != null)
                {
                    yield return parent;
                    parent = parent.Parent;
                }
            }
        }

        public override string ToString()
        {
            return FullPath;
        }
    }
}
