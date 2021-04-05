using System.Collections.Generic;
using BH.FTServer;
using Microsoft.AspNetCore.Mvc;
using Microsoft.Extensions.Logging;

namespace BH.REST.Controllers
{
    [ApiController]
    [Route("[controller]")]
    public class FTServerController : ControllerBase
    {
        private readonly ILogger<FTServerController> _logger;

        public FTServerController(ILogger<FTServerController> logger)
        {
            _logger = logger;
            _ftService = new FTService(null, null);
        }

        #region Members

        private static FTService _ftService;

        #endregion

        #region Methods

        [HttpGet("IsStarted")]
        public bool IsStarted() => _ftService.IsStarted();

        [HttpGet("GetConfiguration")]
        public FTSearch.ConfigurationDLL GetConfiguration() => _ftService.GetConfiguration();

        [HttpGet("GetDefaultConfiguration")]
        public FTSearch.ConfigurationDLL GetDefaultConfiguration() => _ftService.GetDefaultConfiguration();

        [HttpPost("SetConfiguration")]
        public void SetConfiguration(FTSearch.ConfigurationDLL configuration) => _ftService.SetConfiguration(configuration);

        [HttpPost("Start")]
        public void Start(int instanceNumber = 0) => _ftService.Start(instanceNumber);

        [HttpGet("SearchPhrase")]
        public FTSearch.SearchResult SearchPhrase(string phrase,
                                                  string templateName,
                                                  int skip,
                                                  int take) => _ftService.SearchPhrase(phrase, templateName, skip, take);

        [HttpGet("SearchQuery")]
        public FTSearch.SearchResult SearchQuery(List<FTSearch.Selector> selectors,
                                                 int minPage,
                                                 int maxPage,
                                                 int skip,
                                                 bool agregateBySubject) => _ftService.SearchQuery(selectors, minPage, maxPage, skip, agregateBySubject);
        [HttpGet("SearchPhraseRel")]
        public FTSearch.SearchResult SearchPhraseRel(string phrase,
                                                     int minPage,
                                                     int maxPage,
                                                     int skip,
                                                     int take) => _ftService.SearchPhraseRel(phrase, minPage, maxPage, skip, take);

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
        public BH.FTServer.FTService.Info GetInfo() => _ftService.GetInfo();

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
