using System.Collections.Concurrent;
using System.Collections.Generic;

namespace BH.BaseRobot
{
    public class QueueIndexing
    {
        public QueueIndexing()
        {
            _directories = new ConcurrentQueue<Directory>();
            _files = new ConcurrentQueue<File>();
            _filesWithContent = new ConcurrentQueue<File>();
        }

        private ConcurrentQueue<Directory> _directories;
        private ConcurrentQueue<File> _files;
        private ConcurrentQueue<File> _filesWithContent;

        public void EnqueDirectory(Directory directory)
        {
            _directories.Enqueue(directory);
        }

        public void EnqueDirectories(IEnumerable<Directory> directories)
        {
            foreach (var directory in directories)
                EnqueDirectory(directory);
        }

        public Directory DequeDirectory()
        {
            Directory directory;

            if (_directories.TryDequeue(out directory))
                return directory;
            else
                return null;
        }

        public void EnqueFile(File file)
        {
            _files.Enqueue(file);
        }

        public void EnqueFiles(IEnumerable<File> files)
        {
            foreach (var file in files)
                EnqueFile(file);
        }

        public File DequeFile()
        {
            File file;

            if (_files.TryDequeue(out file))
                return file;
            else
                return null;
        }

        public void EnqueFileWithContent(File file)
        {
            _filesWithContent.Enqueue(file);
        }

        public File DequeFileWithContent()
        {
            File file;

            if (_filesWithContent.TryDequeue(out file))
                return file;
            else
                return null;
        }
    }
}
