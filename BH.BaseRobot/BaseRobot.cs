﻿using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace BH.BaseRobot
{
    public class Configuration
    {
        public static Configuration Default => new Configuration();
    }

    public class Directory
    {
        public static Directory DefaultRoot => new Directory();
    }

    public class File
    {
        public string Version => null;
    }

    public class Statistic
    {

    }

    public class Error
    {

    }

    public class Scheduler
    {
        public Scheduler()
        {

        }

        protected void Start()
        {

        }

        protected void Stop()
        {

        }

        protected void Delay(TimeSpan time)
        {

        }
    }

    public class Storage
    {
        public bool IsFileExists(string name)
        {
            return false;
        }
    }

    public class QueueIndexing
    {
        protected void AddDirectory(Directory dir)
        {

        }

        protected void RemoveDirectory(Directory dir)
        {

        }

        protected void AddDirectories(IEnumerable<Directory> dirs)
        {

        }

        protected void AddFile(File file)
        {

        }

        protected void RemoveFile(File file)
        {

        }

        protected void AddFiles(IEnumerable<File> files)
        {

        }
    }

    //1. Booben Com
    //2. Index Files
    //3. Index DataTable

    //1. Hierarchy scan
    //2. Do not scan one file twice (memory)
    //3. Append new directories
    //4. Html/Text

    public class BaseRobot
    {
        #region Event Methods

        protected virtual bool OnBeforeIndexing() => true;
        protected virtual bool OnAfterIndexing() => true;

        protected virtual bool OnBeforeLoadFileContent(File file) => true;
        protected virtual bool OnAfterLoadFileContent(File file) => true;

        protected virtual bool OnBeforeFileIndexing(File file) => true;
        protected virtual bool OnAfterFileIndexing(File file) => true;

        protected virtual bool OnStatUpdated(Statistic stat) => true;

        protected virtual bool OnError(Error error) => true;

        #endregion

        #region Virtual Methods

        protected virtual Configuration GetConfiguration(Configuration config)
        {
            return config;
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

        public Configuration Configuration { get; } = Configuration.Default;

        public Scheduler Scheduler { get; } = new Scheduler();

        public Storage Storage { get; } = new Storage();

        public QueueIndexing QueueIndexing { get; } = new QueueIndexing();

        #endregion
        
        #region Manage Methods

        #endregion
    }
}
