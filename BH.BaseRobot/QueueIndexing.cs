using System.Collections.Concurrent;
using System.Collections.Generic;

namespace BH.BaseRobot
{
    public class QueueIndexing
    {
        public QueueIndexing()
        {
            _folders = new ConcurrentQueue<Folder>();
            _documents = new ConcurrentQueue<Document>();
            _documentsWithContent = new ConcurrentQueue<Document>();
        }

        private ConcurrentQueue<Folder> _folders;
        private ConcurrentQueue<Document> _documents;
        private ConcurrentQueue<Document> _documentsWithContent;

        public void EnqueFolder(Folder directory)
        {
            _folders.Enqueue(directory);
        }

        public void EnqueFolders(IEnumerable<Folder> folders)
        {
            foreach (var folder in folders)
                EnqueFolder(folder);
        }

        public Folder DequeFolder()
        {
            Folder folder;

            if (_folders.TryDequeue(out folder))
                return folder;
            else
                return null;
        }

        public void EnqueDocument(Document document)
        {
            _documents.Enqueue(document);
        }

        public void EnqueDocuments(IEnumerable<Document> documents)
        {
            foreach (var document in documents)
                EnqueDocument(document);
        }

        public Document DequeDocument()
        {
            Document document;

            if (_documents.TryDequeue(out document))
                return document;
            else
                return null;
        }

        public void EnqueDocumentWithContent(Document document)
        {
            _documentsWithContent.Enqueue(document);
        }

        public Document DequeDocumentWithContent()
        {
            Document document;

            if (_documentsWithContent.TryDequeue(out document))
                return document;
            else
                return null;
        }
    }
}
