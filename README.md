## Black Hole - Full Text Search Engine - with huge compression for Big Data

**BH\FTService** - The server which can be installed as Windows Service

**BH\WebSite** - Web site for search documents in the storage 

**BH\Robots** - Make your custom Spider / Crawler / Job to fill storage by scheduler

------------------
### Overview

- **High index compression** In many cases you can expect that your index will take not more **1-3% of text** size or even less.
- **High speed of indexing** up to **60 mb/sec** of text or about 100 files/sec (30-50 kb each) on regular machine
- **High speed of searching** takes **few milliseconds** for a typical simple query on the web site.
- The database is designed for **large datasets** on regular machines. 
The largest known database contains more than **14 TB documents** and flow about **~120 gb new documents per day**. For all this is enough one virtual machine.
- **Horizontal scalable** of instances, but in most cases is enough only machine. 
Largest database is hosted only on one virtual machine.
- Flexible **query language** provides search by phrase/word/dictionary with stemming, detect trends. 
Note: Search is designed without calculate distances or order between words.
- Ability to use ranking algorithm with **Machine Learning** based on associative sequences
- Used and tested every day. for example it's engine for: **[http:\\booben.com](http:\\booben.com)**
------------------

## Three simple steps to install all infrastructure

1. Install your BH\FTService as Windows Service
https://github.com/Bazist/BH/tree/master/Install/Server

2. Install your Web Search Site on IIS
https://github.com/Bazist/BH/tree/master/Install/WebSite

3. Make your custom Robot to fill database.
Example here:
https://github.com/Bazist/BH/tree/master/BH.ExampleRobot

------------------
### Example of simplest Robot

```c#
public class ExampleRobot : BaseRobot.BaseRobot
{
    private const string DocumentsPath = @"C:\FTS\TestDocs";

    protected override IEnumerable<Document> GetDocuments(Folder folder)
    {
        if (Directory.Exists(DocumentsPath))
        {
            return Directory.GetFiles(DocumentsPath, "*.txt")
                        .Select(x => new Document(folder, x, File.ReadAllText(x)));
        }
        else
        {
            throw new DirectoryNotFoundException(DocumentsPath);
        }
    }
}
```

### License

The code is licensed under the GNU General Public License v3.0, see the [LICENSE file](LICENSE) for details.

