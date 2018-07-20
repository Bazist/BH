using BH.WCF;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading;

namespace BH.BaseRobot
{
    //1. Booben Com
    //2. Index Documents
    //3. Index DataTable

    //1. Hierarchy scan
    //2. Do not scan one document twice (memory)
    //3. Append new folders
    //4. Html/Text

    public interface IBaseRobot
    {
        void Start(FTService ftService);
        void Stop();
    }

    public class BaseRobot : IBaseRobot
    {
        #region Event Methods

        protected virtual bool OnBeforeScanning() => true;
        protected virtual bool OnAfterScanning() => true;

        protected virtual bool OnBeforeFolderScanning(Folder document) => true;
        protected virtual bool OnAfterFolderScanning(Folder document) => true;

        protected virtual bool OnBeforeDocumentScanning(Document document) => true;
        protected virtual bool OnAfterDocumentScanning(Document document) => true;

        protected virtual bool OnBeforeLoadDocumentContent(Document document) => true;
        protected virtual bool OnAfterLoadDocumentContent(Document document) => true;

        protected virtual bool OnBeforeDocumentIndexing(Document document) => true;
        protected virtual bool OnAfterDocumentIndexing(Document document) => true;
        
        protected virtual bool OnError(Error error) => true;

        #endregion

        #region Virtual Methods

        protected virtual TimeTable GetTimeTable()
        {
            return new RunNowTimeTable();
        }

        protected virtual IEnumerable<Folder> GetFolders()
        {
            return new[] { Folder.DefaultRoot };
        }

        protected virtual IEnumerable<Document> GetDocuments(Folder folder)
        {
            return new Document[] { };
        }

        protected virtual bool ShouldDocumentIndexed(Document document, string oldVersion, string newVersion) => (newVersion == null ||
                                                                                                      newVersion != oldVersion);
                                   
        protected virtual string LoadDocumentContent(Document document)
        {
            return null;
        }

        public virtual bool IsEnabled => true;

        #endregion

        #region Members

        public Scheduler Scheduler { get; private set; }

        public Storage Storage { get; private set; }

        public QueueIndexing QueueIndexing { get; private set; }

        public bool IsRunning => IsScanRunning || IsIndexRunning;

        public bool IsScanRunning { get; private set; }

        public bool IsIndexRunning { get; private set; }

        #endregion

        #region Run Methods

        public void Start(FTService ftService)
        {
            Storage = new Storage(ftService);
            QueueIndexing = new QueueIndexing();
            Scheduler = new Scheduler(GetTimeTable(), ScanAndIndex);
            Scheduler.Start();
        }

        public void Stop()
        {
            Scheduler.Abort();
        }

        public void ScanAndIndex()
        {
            ThreadPool.QueueUserWorkItem(x => ScanFolders());
            Thread.Sleep(100); //wait to run scan folders thread
            ThreadPool.QueueUserWorkItem(x => IndexDocuments());
        }

        private void ScanFolders()
        {
            try
            {
                IsScanRunning = true;

                if (!OnBeforeScanning())
                    return;

                var folders = GetFolders();
                QueueIndexing.EnqueFolders(folders);

                while (true)
                {
                    var folder = QueueIndexing.DequeFolder();

                    if (folder != null)
                    {
                        if (!ScanFolder(folder))
                            return;
                    }
                    else
                    {
                        break;
                    }
                }
            }
            finally
            {
                OnAfterScanning();

                IsScanRunning = false;
            }

            return;
        }

        private bool ScanFolder(Folder folder)
        {
            try
            {
                if (!OnBeforeFolderScanning(folder))
                    return false;

                var documents = GetDocuments(folder);

                QueueIndexing.EnqueDocuments(documents);

                IDictionary<string, string> oldVersions = null;

                if (documents.Any(x => x.HasVersion))
                {
                    oldVersions = Storage.ReadDocumentVersions(documents);
                }

                while (true)
                {
                    var document = QueueIndexing.DequeDocument();

                    if (document != null)
                    {
                        if (!ScanDocument(oldVersions, document))
                            return false;
                    }
                    else
                    {
                        break;
                    }
                }

            }
            finally
            {
                OnAfterFolderScanning(folder);
            }

            return true;
        }

        private bool ScanDocument(IDictionary<string, string> oldVersions, Document document)
        {
            try
            {
                if (!OnBeforeDocumentScanning(document))
                    return false;

                if (oldVersions == null ||
                    ShouldDocumentIndexed(document, oldVersions[document.Name], document.Version))
                {
                    try
                    {
                        if (!OnBeforeLoadDocumentContent(document))
                            return false;

                        if (!document.HasContent)
                        {
                            document.Content = LoadDocumentContent(document); ;
                        }

                        if(document.HasContent)
                        {
                            QueueIndexing.EnqueDocumentWithContent(document);
                        }
                    }
                    finally
                    {
                        OnAfterLoadDocumentContent(document);
                    }
                }
            }
            finally
            {
                OnAfterDocumentScanning(document);
            }

            return true;
        }

        private void IndexDocuments()
        {
            try
            {
                IsIndexRunning = true;

                while (true)
                {
                    var document = QueueIndexing.DequeDocumentWithContent();

                    if (document != null)
                    {
                        try
                        {
                            if (!OnBeforeDocumentIndexing(document))
                                return;

                            Storage.IndexDocument(document);
                        }
                        finally
                        {
                            OnAfterDocumentIndexing(document);
                        }
                    }
                    else if(IsScanRunning)
                    {
                        Thread.Sleep(TimeSpan.FromSeconds(1)); //wait documents from scanning
                    }
                    else
                    {
                        break;
                    }
                }
            }
            finally
            {
                Storage.SaveIndex();

                IsIndexRunning = false;
            }

            return;
        }

        #endregion
    }
}
