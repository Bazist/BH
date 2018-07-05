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

        public BH.FTSearchResult[] Results { get; set; }

        public static string FormatSize(ulong size)
        {
            return size.ToString("N1", CultureInfo.InvariantCulture).Replace(".0", string.Empty);
        }
    }
}
