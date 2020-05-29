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
    public class Storage
    {
        private FTService _ftService;

        public Storage(FTService ftService)
        {
            _ftService = ftService;
        }

        public FTService.Info GetInfo()
        {
            return _ftService.GetInfo();
        }

        public IDictionary<string, string> ReadDocumentVersions(IEnumerable<Document> documents)
        {
            return _ftService.ReadDocumentVersions(documents.Select(x => x.Name).ToList());
        }

        public void IndexDocument(Document file, string robotName)
        {
            if (_ftService.IndexText(file.Name,
                                     file.Version,
                                     file.Content,
                                     robotName))
                SaveIndex();
        }

        public bool IsdocumentExists(string name)
        {
            return false;
        }

        public void SaveIndex()
        {
            _ftService.SaveIndex();
        }
    }
}
