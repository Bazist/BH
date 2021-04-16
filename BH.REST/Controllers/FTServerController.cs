using System.Collections.Generic;
using BH.REST.Models;
using Microsoft.AspNetCore.Mvc;
using Microsoft.Extensions.Logging;
using BH.FTServer;

namespace BH.REST.Controllers
{
    [ApiController]
    [Route("[controller]")]
    public class FTServerController : ControllerBase
    {
        private readonly ILogger<FTServerController> _logger;

        public FTServerController(FTService ftService, ILogger<FTServerController> logger)
        {
            _logger = logger;
            _ftService = ftService;
        }

        #region Members

        private static FTService _ftService;

        #endregion

        #region Methods

        [HttpGet("IsStarted")]
        public bool IsStarted() => _ftService.IsStarted();

        [HttpGet("GetConfiguration")]
        public Configuration GetConfiguration() => Configuration.From(_ftService.GetConfiguration());

        [HttpGet("GetDefaultConfiguration")]
        public Configuration GetDefaultConfiguration() => Configuration.From(_ftService.GetDefaultConfiguration());

        [HttpPost("SetConfiguration")]
        public void SetConfiguration(Configuration configuration) => _ftService.SetConfiguration(configuration.ToConfigurationDLL());

        [HttpPost("Start")]
        public void Start(int instanceNumber = 0) => _ftService.Start(instanceNumber);

        [HttpGet("SearchPhrase")]
        public SearchResult SearchPhrase(string phrase,
                                         string templateName,
                                         int skip,
                                         int take) => SearchResult.From(_ftService.SearchPhrase(phrase, templateName, skip, take));

        [HttpGet("SearchQuery")]
        public SearchResult SearchQuery(List<FTSearch.Selector> selectors,
                                                 int minPage,
                                                 int maxPage,
                                                 int skip,
                                                 bool agregateBySubject) => SearchResult.From(_ftService.SearchQuery(selectors, minPage, maxPage, skip, agregateBySubject));
        [HttpGet("SearchPhraseRel")]
        public SearchResult SearchPhraseRel(string phrase,
                                            int minPage,
                                            int maxPage,
                                            int skip,
                                            int take) => SearchResult.From(_ftService.SearchPhraseRel(phrase, minPage, maxPage, skip, take));

        [HttpPost("InitSearchRel")]
        public void InitSearchRel() => _ftService.InitSearchRel();

        [HttpGet("StemContent")]
        public string StemContent(string contentText) => _ftService.StemContent(contentText);

        [HttpGet("LoadDocumentContent")]
        public string LoadDocumentContent(string documentName,
                                          string documentVersion,
                                          string robotName) => _ftService.LoadDocumentContent(documentName, documentVersion, robotName);

        [HttpGet("CalculateTrend")]
        public string CalculateTrend(string phrase,
                                     int count,
                                     int minPage,
                                     int maxPage) => _ftService.CalculateTrend(phrase, count, minPage, maxPage);

        [HttpGet("GetInfo")]
        public Info GetInfo() => Info.From(_ftService.GetInfo());

        [HttpPost("IndexText")]
        public bool IndexText(string documentName,
                              string documentVersion,
                              string contentText,
                              string robotName) => _ftService.IndexText(documentName, documentVersion, contentText, robotName);

        [HttpPost("IndexFile")]
        public bool IndexFile(string documentName,
                              string documentVersion,
                              string filePath,
                              string robotName) => _ftService.IndexFile(documentName, documentVersion, filePath, robotName);


        [HttpPost("SaveIndex")]
        public void SaveIndex() => _ftService.SaveIndex();

        [HttpPost("MergeIndexes")]
        public void MergeIndexes() => _ftService.MergeIndexes();

        [HttpPost("Stop")]
        public void Stop() => _ftService.Stop();

        [HttpGet("CheckIndexes")]
        public void CheckIndexes() => _ftService.CheckIndexes();

        [HttpPost("ClearInstance")]
        public void ClearInstance() => _ftService.ClearInstance();

        #endregion
    }
}
