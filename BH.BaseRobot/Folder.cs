using System.Collections.Generic;
using System.Linq;

namespace BH.BaseRobot
{
    public class Folder
    {
        public Folder(string name,
                      Folder parent = null)
        {
            Name = name;
            Parent = parent;
        }

        public string Name { get; }

        public Folder Parent { get; }

        public static Folder DefaultRoot => new Folder("[Default Root]");

        public string FullPath
        {
            get
            {
                return string.Join("\\", Ancestors.Reverse()
                                                  .Select(x => x.Name)
                                                  .Union(new[] { Name }));
            }
        }

        public IEnumerable<Folder> Ancestors
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
