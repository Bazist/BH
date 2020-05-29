/*
# Copyright(C) 2010-2020 Viacheslav Makoveichuk (email: slv709@gmail.com, skype: vyacheslavm81)
# This file is part of Black Hole FTS.
#
# Black Hole FTS is free software : you can redistribute it and / or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# Black Hole FTS is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with Foobar.  If not, see <http://www.gnu.org/licenses/>.
*/

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
