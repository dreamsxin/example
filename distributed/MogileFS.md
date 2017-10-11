# MogileFS

MogileFS是一套開源的分布式存儲組件，由LiveJournal旗下的Danga Interactive研發，主要應用於存儲海量的小文件。

MogileFS有3個組件：

- Tracker Nodes
是文件系統集群的核心節點，借助數據庫節點保存元數據，主要功能包括監控後端Storage節點，及指示Storage節點完成數據的添加、刪除、查詢，及節點之間數據的復制等，該節點上運行的進程為mogilefsd。往往Tracker節點有多個，以實現負載均衡和高可用；

- Storage Nodes
實際存儲數據的位置，基於WebDAV模式工作，能完成文件創建、刪除、重命名等操作，運行的進程為mogstored；

- database Nodes
幫助Tracker節點存放元數據及全局信息，包括host，domain，class等。數據庫建議使用高可用集群，防止單點故障。

## 實現機制

在該文件系統集群中，每一個數據塊都會有多個副本，存放於多個Storage Node上，不同的文件系統副本的數量各不相同（一般建議為3份），在Tracker Node上會分別單獨記錄每一個數據塊的存儲位置（位於哪個Storage及路徑）。若某個Storage節點不在線了，客戶端向Tracker節點請求獲取數據時，Tracker節點會返回用於冗余的副本的存儲位置給客戶端。這樣就保證了整個文件系統中數據的高可用性。當有數據存儲至某一storage節點後，各storage節點會根據Tracker的指示通過節點間通信將副本發送至其他節點上。若某一storage下線了，該節點上的副本會由其他節點重新提供，保證副本的個數不少於指定的值。

