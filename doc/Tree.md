这是对B+树程序的结构的介绍。  
环境：ubuntu 14.04  
  
1,B+树文件:  
name.db1:包含每个节点。  
name.db2:包含每个数据项。  
  
2,B+树介绍:  
每个索引不同。  
两个key之间指针是[,)这样的。  
  
3,文件结构:  
db1文件:  
开始为32Byte的文件头：  

node_num[4Byte]:节点个数。  
node_size[4Byte]:节点大小(默认4KB)。  
key_num[4Byte]:一个节点中key的最大个数(有x个key则有x+1个指针，默认510(node_size为4KB))。  
key_size[4Byte]:每个key的大小(Byte，默认4)。  
point_size[4Byte]:每个指针的大小(Byte，默认4)。  
data_start[4Byte]:本文件中数据开始存储的位置(Byte，默认32)。  
root_pos[4Byte]:指向根节点的位置。  
empty_pos[4Byte]:指向文件中第一个空区域(-1表示无空区域，需要append)。  
(指针为第几个存储片，由节点大小*个数+data_start算出偏移量)。  
  
然后是数据存储的开始：  
每个节点固定大小，结构为：  
[num][leaf][father][key][key]...[key][point][point]...[point]  
其中num个已经占用的key，num+1个已用point。  
leaf表示是否是叶子节点。  
father指向节点的父亲，指向自己是根节点。  
num leaf father point均为4Byte。  
key一共由key_num个，poing一共key_num+1个。  
  
db2文件：  
开始为32Byte的文件头：  
  
node_num[4Byte]:节点个数。  
node_size[4Byte]:节点大小。  
data_size[4Byte]:一个数据的大小，小于等于node_size，为了对齐。  
data_start[4Byte]:数据开始位置。  
empty_pos[4Byte]:指向文件中第一个空区域。  
/[12Byte/]:保留。  
  
4,insert：
先find叶子节点，然后递归向上插入。  
插入前叶子节点是 1|2|3|4|5  
插入后是 1|2|N#3|4|5  
#对于非叶节点插入后是 1|2|3#N|4|5  
