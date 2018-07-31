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
The largest known database contains more than **14 TB documents** and flow about **~120 gb new documents a day** 
and hosted on one virtual machine.
- **Horizontal scalable** of instances, but in most cases is enough only machine. 
Largest database is hosted only on one virtual machine.
- Flexible **query language** provides search by phrase/word/dictionary with stemming, detect trends. 
Note: Search is designed without calculate distances or order between words.
- For ranking of big amount of documents used special **Machine Learning algorithms** based on associative sequences
- Used and tested every day. for example it's engine for: http:\\booben.com
------------------

## Why we love Trie ? Because it has much more functionality and stability than Hashtables and much more faster than Binary Trees. Let's compare properties:

![alt tag](https://raw.githubusercontent.com/Bazist/HArray/master/Images/functionality.png)

------------------

## Trie ? I heard about Trees and Hastables but don't know anything about Trie
# [Explain me as for Beginners](https://github.com/Bazist/HArray/blob/master/Trie_for_beginners.md)


### Examples

Initialize container

```c++
#include "HArray.h"

HArray ha;
ha.init(); //ha.init(24) set your custom capacity for big datasets
```
Insert a key

```c++
uint32 key[] = { 10, 20, 30, 40 };
uint32 keyLen = sizeof(key);
uint32 value = 1;

ha.insert(key, keyLen, value);
```

Get value by key. Will return 0 if key is not found

```c++
uint32* pValue = ha.getValueByKey(key, keyLen);
```

Get all keys by range from min key to max key. 

```c++
HArrayPair pairs[5];
uint32 pairsSize = 5;

uint32 minKey[] = { 10, 10 };
uint32 minKeyLen = sizeof(minKey);

uint32 maxKey[] = { 20, 20 };
uint32 maxKeyLen = sizeof(maxKey);

uint32 count = ha.getKeysAndValuesByRange(pairs, pairsSize, minKey, minKeyLen, maxKey, maxKeyLen);
for (uint32 i = 0; i < count; i++)
{
   uint32* key = pairs[i].Key;
   uint32 keyLen = pairs[i].KeyLen;

   uint32 value = pairs[i].Value;
   
   //here your code
}
```

Scan all keys through visitor

```c++
bool visitor(uint32* key, uint32 keyLen, uint32 value, uchar8 valueType, void* pData)
{
   //handle founded key here
   // ...

   //return true to continue scaning or false otherwise
   return true;
}

//Start scanning

void* pData = 0; //useful data in context

ha.scanKeysAndValues(&visitor, pData);
```

Serialize/Deserialize containter from/to file

```c++
ha.saveToFile("c:\\dump");

ha.loadFromFile("c:\\dump");
```

Check if container has part of key

```c++
uint32 key[] = { 10, 20, 30 };
uint32 keyLen = sizeof(key);

if(ha.hasPartKey(key, keyLen))
{
   //code here
}
```

Set specific comparator to redefine order of keys in the container.

```c++
float key[] = { 10.0, 20.0, 30.0 };
uint32 keyLen = sizeof(key);

uint32 value = 1;

//Set float comparator for right sorting
//Another options: setStrComparator, setInt32Comparator, setUInt32Comparator 
//or define your custom comparator through setCustomComparator
ha.setFloatComparator();

ha.insert((uint32*)key, keyLen, value);

```

Delete Key and Value from container

```c++
ha.delValueByKey(key, keyLen);
```

### License

The code is licensed under the GNU General Public License v3.0, see the [LICENSE file](LICENSE) for details.

