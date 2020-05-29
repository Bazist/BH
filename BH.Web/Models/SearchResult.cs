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
using System.ComponentModel.DataAnnotations;
using System.Globalization;
using System.Linq;
using System.Text.RegularExpressions;
using System.Threading.Tasks;

namespace BH.Web.Model
{
    public class PhraseAttribute : ValidationAttribute
    {
        public PhraseAttribute()
        {
        }

        protected override ValidationResult IsValid(object value, ValidationContext validationContext)
        {
            if(value == null)
                return ValidationResult.Success;

            var parts = value.ToString().Split(new char[] { ' ' }, StringSplitOptions.RemoveEmptyEntries);

            if (parts.Length > 3)
                return new ValidationResult("Max words in phrase: 3");

            if (parts.Any(x => x.Length < 3))
                return new ValidationResult("Min phrase len: 3");

            if (parts.Any(x => !Regex.IsMatch(x, "^[a-zA-Z\\-0-9]+$")))
                return new ValidationResult("Word in phrase has wrong symbols");


            return ValidationResult.Success;
        }
    }

    public class TemplateNameAttribute : ValidationAttribute
    {
        public TemplateNameAttribute()
        {
        }

        protected override ValidationResult IsValid(object value, ValidationContext validationContext)
        {
            if (value == null)
                return ValidationResult.Success;

            var parts = value.ToString().Split(new char[] { ' ' }, StringSplitOptions.RemoveEmptyEntries);

            if (parts.Length > 1)
                return new ValidationResult("Max words in phrase: 1");

            if (parts.Any(x => !Regex.IsMatch(x, "^[a-zA-Z\\-0-9]+$")))
                return new ValidationResult("Word in phrase has wrong symbols");


            return ValidationResult.Success;
        }
    }

    public class SearchResult
    {
        public const int PAGE_SIZE = 25;

        [Required]
        [Range(0, 1000)]
        public int StartPage { get; set; }

        [Required]
        [Range(0, 1000)]
        public int CurrentPage { get; set; }

        [PhraseAttribute]
        public string Phrase { get; set; }

        [TemplateNameAttribute]
        public string TemplateName { get; set; }

        public WebService.FTSearchResult[] Results { get; set; }

        public static string FormatSize(ulong size)
        {
            return size.ToString("N1", CultureInfo.InvariantCulture).Replace(".0", string.Empty);
        }
    }
}
