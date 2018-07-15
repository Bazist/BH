using BH.WCF;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading;

namespace BH.BaseRobot
{
    //1. Booben Com
    //2. Index Files
    //3. Index DataTable

    //1. Hierarchy scan
    //2. Do not scan one file twice (memory)
    //3. Append new directories
    //4. Html/Text

    public interface IBaseRobot
    {
        void Run();
    }

    public class BaseRobot : IBaseRobot
    {
        #region Event Methods

        protected virtual bool OnBeforeScanning() => true;
        protected virtual bool OnAfterScanning() => true;

        protected virtual bool OnBeforeDirectoryScanning(Directory file) => true;
        protected virtual bool OnAfterDirectoryScanning(Directory file) => true;

        protected virtual bool OnBeforeFileScanning(File file) => true;
        protected virtual bool OnAfterFileScanning(File file) => true;

        protected virtual bool OnBeforeLoadFileContent(File file) => true;
        protected virtual bool OnAfterLoadFileContent(File file) => true;

        protected virtual bool OnBeforeFileIndexing(File file) => true;
        protected virtual bool OnAfterFileIndexing(File file) => true;
        
        protected virtual void OnStatUpdated(Statistic stat) { }

        protected virtual bool OnError(Error error) => true;

        #endregion

        #region Virtual Methods

        protected virtual Scheduler GetScheduler()
        {
            return new Scheduler(new RunNowTimeTable(), Run);
        }

        protected virtual IEnumerable<Directory> GetDirectories()
        {
            return new[] { Directory.DefaultRoot };
        }

        protected virtual IEnumerable<File> GetFiles(Directory directory)
        {
            return null;
        }

        protected virtual bool ShouldFileIndexed(File file, string oldVersion, string newVersion) => (newVersion == null ||
                                                                                                      newVersion != oldVersion);
                                   
        protected virtual string LoadFileContent(File file)
        {
            return null;
        }

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

        public void Init(FTService ftService)
        {
            Storage = new Storage(ftService);
            QueueIndexing = new QueueIndexing();
            Scheduler = GetScheduler();
        }

        public void Run()
        {
            ThreadPool.QueueUserWorkItem(x => ScanDirectories());
            ThreadPool.QueueUserWorkItem(x => IndexFiles());
        }

        private void ScanDirectories()
        {
            try
            {
                IsScanRunning = true;

                if (!OnBeforeScanning())
                    return;

                var directories = GetDirectories();
                QueueIndexing.EnqueDirectories(directories);

                while (true)
                {
                    var directory = QueueIndexing.DequeDirectory();

                    if (directory != null)
                    {
                        if (!ScanDirectory(directory))
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

        private bool ScanDirectory(Directory directory)
        {
            try
            {
                if (!OnBeforeDirectoryScanning(directory))
                    return false;

                var files = GetFiles(directory);

                QueueIndexing.EnqueFiles(files);

                ReadFileVersions(files);

                while (true)
                {
                    var file = QueueIndexing.DequeFile();

                    if (file != null)
                    {
                        if (!ScanFile(file))
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
                OnAfterDirectoryScanning(directory);
            }

            return true;
        }

        private bool ScanFile(File file)
        {
            try
            {
                if (!OnBeforeFileScanning(file))
                    return false;

                if (ShouldFileIndexed(file, file.Version, null))
                {
                    try
                    {
                        if (!OnBeforeLoadFileContent(file))
                            return false;

                        string content = LoadFileContent(file);

                        if(!string.IsNullOrEmpty(content))
                        {
                            file.Content = content;

                            QueueIndexing.EnqueFileWithContent(file);
                        }
                    }
                    finally
                    {
                        OnAfterLoadFileContent(file);
                    }
                }
            }
            finally
            {
                OnAfterFileScanning(file);
            }

            return true;
        }

        private void IndexFiles()
        {
            try
            {
                IsIndexRunning = true;

                while (true)
                {
                    var file = QueueIndexing.DequeFileWithContent();

                    if (file != null)
                    {
                        try
                        {
                            if (!OnBeforeFileIndexing(file))
                                return;

                            IndexFile(file);
                        }
                        finally
                        {
                            OnAfterFileIndexing(file);
                        }
                    }
                    else if(IsScanRunning)
                    {
                        Thread.Sleep(TimeSpan.FromSeconds(1)); //wait files from scanning
                    }
                    else
                    {
                        break;
                    }
                }
            }
            finally
            {
                IsIndexRunning = false;
            }

            return;
        }

        private bool IndexFile(File file)
        {
            return true;
        }

        #endregion
    }
}
