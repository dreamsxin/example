

1. Java 中 `hashcode` 相同，`equal` 一定相同

不一定，因为hashcode，equal方法可以重写

2. java自帶的註解有哪些？java的註解是如何工作的？

- `@Override`			表示當前的方法定義將覆蓋超類中的方法。
- `@Deprecated`			使用了註解為它的元素編譯器將發出警告，因為註解@Deprecated是不贊成使用的代碼，被棄用的代碼。
- `@SuppressWarnings`	關閉不當編譯器警告信息。

註解處理就是透過反射機制獲取被檢查方法上的註解信息，然後根據註解元素的值進行特定的處理。

另外附錄元註解的解析：

- @Target

表示該註解可以用於什麼地方。
可能的ElementType參數有：CONSTRUCTOR 構造器的聲明，FIELD 域聲明（包括enum實例），LOCAL_VARIABLE 局部變數聲明，METHOD 方法聲明，PACKAGE 包聲明，PARAMETER 參數聲明，TYPE 類、介面或enum聲明

- @Retention

表示需要在什麼級別保存該註解信息。
可選的RetentionPolicy參數包括：SOURCE 註解將被編譯器丟棄，CLASS 註解在class文件中可用，但會被VM丟棄， RUNTIME VM將在運行期間保留註解，因此可以透過反射機制讀取註解的信息。

- @Document

將註解包含在Javadoc中

- @Inherited

允許子類繼承父類中的註解

3. ArrayList和LinkList，為什麼ArrayList長於索引元素，而LinkList長於修改元素？

ArrayList 底層是數組結構,默認是空集。在添加和刪除時候，ArrayList會自動擴容，擴容演算法如下，也就是取舊長度*3/2和最小擴容長度的最大值。可見需要擴容的用了Arrays.copyof方法，比較耗資源和時間。所以建議如果明確知道長度的話，建議在初始化時候定義size。

LinkedList 核心結構，雙向鏈表結構，記錄最開始檢索的first和last元素，方便從頭或從尾檢索。

transient Node<E> first;

4. 單例模式有多少種寫法？

餓漢模式、懶漢模式、線程安全懶漢模式、雙重鎖懶漢模式、靜態內部類實現

單例模式的特點：

- 單例類只能有一個實例。
- 單例類必須自己自己創建自己的唯一實例。
- 單例類必須給所有其他對象提供這一實例。

* 餓漢模式

餓漢模式在載入類的時候就初始化了，優點是不用考慮多線程問題，缺點是沒使用也進行了初始化操作。

* 懶漢模式

懶漢模式和餓漢模式相反，需要考慮多線程問題。

* 線程安全懶漢模式

給獲取單例的方法加上synchronize就可以實現線程安全。

* 雙重鎖懶漢模式

不過在多併發很大時候，線程安全懶漢模式存在效率問題，所以才有了雙重鎖懶漢模式。

* 靜態內部類實現

靜態內部類，是jvm級別提供的類初始化，推薦使用這種實現。

* 枚舉實現單例

同樣枚舉類型也可以實現單例，不過確實大家使用的都不多。

5. 線程和進程的區別？啟動線程是start方法還是run方法？什麼是線程同步？如何實現線程同步？

進程和線程的主要差別在於它們是不同的作業系統資源管理方式。進程有獨立的地址空間，一個進程崩潰後，在保護模式下不會對其它進程產生影響，而線程只是一個進程中的不同執行路徑。

線程有自己的堆棧和局部變數，但線程之間沒有單獨的地址空間，一個線程死掉就等於整個進程死掉，所以多進程的程式要比多線程的程式健壯，但在進程切換時，耗費資源較大，效率要差一些。

啟動一個是用start方法，至於線程什麼時候執行是有jvm決定。執行方法才是run方法。

線程同步是指多線程運行情況下，運行結果和單線程運行是一樣，那運行的程式可以理解成線程安全。

線程安全可以給代碼加上修飾符synchronized關鍵字，或者用顯示的Lock來代碼只能被一個線程獨占。

6. HashTable於HashMap區別

HashTable繼承Dictionary，HashMap繼承AbstractMap。
HashTable初始化是11，而HashMap初始化長度是16。
HashTable外部方法都是synchronized修飾，可以保證線程安全，而HashMap則不是線程安全。
HashTable不能key為null的鍵值對，而HashMap可以。下麵是HashTble的put源碼。

7. Volatile能實現所修飾的欄位線程安全嗎？

非线程安全，对于volatile类型的变量，系统每次用到他的时候都是直接从对应的内存当中提取，而不会利用cache当中的原有数值，以适应它的未知何时会发生的变化，系统对这种变量的处理不会做优化——显然也是因为它的数值随时都可能变化的情况。

8. transient修飾作用

當對象實現序列化也就是實現 Serliazable, transient修飾的成員可以避免序列化。

9. String 类

String是final類，也就是不可繼承的。實現介面Serializable，可序列化。CharSequence定義了String常用的介面，所以String是面向介面編程的一個例子。
底層是由char的數組實現，定義成final，也就是只能賦值一次，這就是為什麼String定義的值是不可變。另外還有一個hash值，定義了String的hashCode。以下是計算hashCode的源碼，hashCode的計算公式是：s[0]*31^(n-1) + s[1]*31^(n-2) + ... + s[n-1]
