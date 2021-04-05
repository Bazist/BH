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

using System;
using System.Collections.Generic;
using System.Text;

namespace BH.FTServer.Stemmer
{
    internal class Among
    {
        public readonly int s_size; /* search string */
        public readonly char[] s; /* search string */
        public readonly int substring_i; /* index to longest matching substring */
        public readonly int result; /* result of the lookup */
        public delegate bool boolDel();
        public readonly boolDel method; /* method to use if substring matches */

        public Among(string s, int substring_i, int result, boolDel linkMethod)
        {
            this.s_size = s.Length;
            this.s = s.ToCharArray();
            this.substring_i = substring_i;
            this.result = result;
            this.method = linkMethod;
        }
    }
}
