using System.Collections.Generic;
using System.Linq;

namespace BH.BaseRobot
{
    public class Directory
    {
        public Directory(string name,
                         Directory parent = null)
        {
            Name = name;
            Parent = parent;
        }

        public string Name { get; }

        public Directory Parent { get; }

        public static Directory DefaultRoot => new Directory("[Default Root]");

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
