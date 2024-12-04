## 序列化和反序列化Map对象

对于Map、Set等ES6中引入的复杂数据结构，JSON是无法直接序列化和反序列化的。

```js
/**
 * 序列化Map对象
 * @param {Map} map 要序列化的Map对象
 * @returns {Object} 序列化后的对象
 */
function serializeMap(map) {
  return Array.from(map.entries());
}

/**
 * 反序列化Map对象
 * @param {Array} entries 序列化后的对象
 * @returns {Map} 反序列化后的Map对象
 */
function deserializeMap(entries) {
  return new Map(entries);
}

/**
 * 保存mindDataObj格式的数据
 * @param {string} key 存储对象的键
 * @param {Object} initMindDataObj 要保存的对象
 * @returns {Promise<void>} 返回一个 Promise，表示操作完成
 */
export async function saveMindData(key, mindDataObj) {
  const serializedData = {
    keywordMap: serializeMap(mindDataObj.keywordMap),
    mindDataArr: mindDataObj.mindDataArr
  };
  await saveObject(key, serializedData);
}

/**
 * 获取mindDataObj格式的数据
 * @param {string} key 存储对象的键
 * @returns {Promise<Object>} 返回一个包含mindDataObj格式数据的 Promise
 */
export async function getMindData(key) {
  const serializedData = await getObject(key);
  const mindDataObj = {
    keywordMap: deserializeMap(serializedData.keywordMap || []),
    mindDataArr: serializedData.mindDataArr || []
  };
  return mindDataObj;
}
```
