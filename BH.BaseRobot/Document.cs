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

        public bool HasVersion => !string.IsNullOrEmpty(Version);

        public string FullPath => $"{Folder.FullPath}\\{Name}";

        public string Version { get; set; }

        public override string ToString()
        {
            return FullPath;
        }
    }
}
