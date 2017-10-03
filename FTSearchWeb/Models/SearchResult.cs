using System;
using System.Collections.Generic;
using System.ComponentModel.DataAnnotations;
using System.Globalization;
using System.Linq;
using System.Text.RegularExpressions;
using System.Threading.Tasks;

namespace FTSearchWeb.Model
{
    public class PhraseAttribute : ValidationAttribute
    {
        public PhraseAttribute()
        {
        }

        protected override ValidationResult IsValid(object value, ValidationContext validationContext)
        {
            if (value != null)
            {
                var parts = value.ToString().Split(new char[] { ' ' }, StringSplitOptions.RemoveEmptyEntries);

                if (parts.Length > 3)
                    return new ValidationResult("Max words in phrase: 3");

                if (parts.Any(x => x.Length < 3))
                    return new ValidationResult("Min phrase len: 3");

                if (parts.Any(x => !Regex.IsMatch(x, "^[a-zA-Z\\-0-9]+$")))
                    return new ValidationResult("Word in phrase has wrong symbols");
            }

            return ValidationResult.Success;
        }
    }

    public class SearchResult
    {
        public const int PAGE_SIZE = 25;

        [Required]
        [Range(1, 1000)]
        public int StartPage { get; set; }

        [Required]
        [Range(1, 1000)]
        public int CurrentPage { get; set; }

        [Required]
        [StringLength(100)]
        [PhraseAttribute]
        public string Phrase { get; set; }

        [StringLength(100)]
        public string TemplateName { get; set; }

        public BH.FTSearchResult[] Results { get; set; }

        public static string FormatSize(ulong size)
        {
            return size.ToString("N1", CultureInfo.InvariantCulture).Replace(".0", string.Empty);
        }
    }
}
