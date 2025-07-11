## 剪贴板

```html
<button id="readClipboard">readClipboard</button>
<img id="showimg"/>
<script>
document.getElementById('readClipboard').addEventListener('click', async () => {
    try {
        const clipboardItems = await navigator.clipboard.read();
        for (let i = 0; i < clipboardItems.length; i++) {
            const item = clipboardItems[i];
			console.log("item", item.types);
            if (item.types.includes('image/png')) {
                const blob = await item.getType('image/png');
				console.log("blob", blob);
                const url = URL.createObjectURL(blob);
				document.getElementById('showimg').src = url;
                console.log(url); // 这里可以得到图片的URL
            }
        }
    } catch (err) {
        console.error('Failed to read clipboard contents: ', err);
    }
});
async function copyImageToClipboard(imageUrl) {
    try {
        // 创建一个img元素并设置src为图片URL，这样浏览器会开始加载图片
        const img = new Image();
        img.src = imageUrl;
        await new Promise((resolve, reject) => {
            img.onload = resolve;
            img.onerror = reject;
        });
 
        // 创建一个canvas元素并绘制图片
        const canvas = document.createElement('canvas');
        const ctx = canvas.getContext('2d');
        canvas.width = img.width;
        canvas.height = img.height;
        ctx.drawImage(img, 0, 0);
 
        // 将canvas内容转换为Data URL
        const dataUrl = canvas.toDataURL('image/png');
        await navigator.clipboard.write([new ClipboardItem({ "image/png": new Blob([dataUrl], { type: 'image/png' }) })]);
        console.log('Image copied to clipboard');
    } catch (err) {
        console.error('Failed to copy image to clipboard: ', err);
    }
}
</script>
```
