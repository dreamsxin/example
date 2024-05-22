#

https://github.com/chroma-core/chroma

```shell
pip install chromadb
```

```py
import chromadb
chroma_client = chromadb.Client()

collection = chroma_client.create_collection(name="my_collection")

collection.add(
    documents=[
        "This is a document about pineapple",
        "This is a document about oranges",
        "This is a document about hawaii system"
    ],
	metadatas=[{"chapter": "1", "verse": "16"}, {"chapter": "2", "verse": "5"}, {"chapter": "3", "verse": "5"}],
    ids=["id1", "id2", "id3"]
)

results = collection.query(
    query_texts=["This is a query document about hawaii"], # Chroma will embed this for you
    n_results=2 # how many results to return
)
print(results)
```
output:
```output
{'ids': [['id3', 'id1']], 'distances': [[0.2729957103729248, 1.0404009819030762]], 'metadatas': [[{'chapter': '3', 'verse': '5'}, {'chapter': '1', 'verse': '16'}]], 'embeddings': None, 'documents': [['This is a document about hawaii system', 'This is a document about pineapple']], 'uris': None, 'data': None}
```
