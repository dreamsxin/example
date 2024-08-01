## 插件安装监听
```js
chrome.runtime.onInstalled.addListener(() => {
    // Check if userId already exists
    chrome.storage.local.get('userId', (data) => {
        if (!data.userId) {
            // Generate a random user ID with "user-" prefix
            const userId = 'user-' + Math.random().toString(36).substr(2, 9);

            // Store the user ID to local storage
            chrome.storage.local.set({ 'userId': userId }, () => {
                // Call the API to store the user ID
				console.log("2");
                storeUserIdToAPI(userId);
            });
        }
    });
});

function storeUserIdToAPI(userId) {

}
```

## 发送消息
```js
chrome.runtime.sendMessage({ 
    emails: emails, 
    phoneNumbers: phoneNumbers,
    url: window.location.href, 
    title: title 
});
```
## 消息监听
```js
chrome.runtime.onMessage.addListener((message, sender, sendResponse) => {
    if ((message.emails && message.emails.length > 0) || (message.phoneNumbers && message.phoneNumbers.length > 0)) {
        chrome.storage.local.get('userId', (data) => {
            saveData(message.emails, message.phoneNumbers, message.url, message.title, data.userId);
        });
    }
});
```
function saveData(newEmails, newPhoneNumbers, sourceUrl, sourceTitle, userId) {
	console.log("Received message:", newEmails, newPhoneNumbers, sourceUrl, sourceTitle);
    chrome.storage.local.get(['storedData'], (result) => {
        let data = result.storedData || [];
        
        if(newEmails && newEmails.length > 0) {
            newEmails.forEach(email => {
                data.push({ userId, type: 'email', value: email, url: sourceUrl, title: sourceTitle });
            });
        }
        
        if(newPhoneNumbers && newPhoneNumbers.length > 0) {
            newPhoneNumbers.forEach(phone => {
                data.push({ userId, type: 'phone', value: phone, url: sourceUrl, title: sourceTitle });
            });
        }

        // 去除重复项
        const uniqueData = Array.from(new Set(data.map(a => a.userId + '|' + a.type + '|' + a.value)))
            .map(id => {
                const parts = id.split('|');
                return data.find(a => a.userId === parts[0] && a.type === parts[1] && a.value === parts[2]);
            });
        
        chrome.storage.local.set({ 'storedData': uniqueData }, () => {
            console.log("Data saved:", uniqueData);
        });
    });
}
```

## chrome.webNavigation API

提供了一组用于观察和处理浏览器导航事件的方法和事件。这个 API 允许你在页面加载的不同阶段执行代码，例如在页面开始加载、完成加载或出现错误时。

- onBeforeNavigate
在导航到指定的页面之前触发。
- onCompleted
在页面导航完成时触发。
- onErrorOccurred
在导航过程中出现错误时触发。

获取标签页详情
某些事件（如 onCompleted）在其 details 对象中提供了标签页的 tabId，你可以使用这个 tabId 来获取更多关于该标签页的信息。

首先，你需要在你的 manifest.json 文件中添加 webNavigation 和 host permissions 权限：
```json
{
  "permissions": [
    "webNavigation",
    "<all_urls>"
  ]
}
```

然后，背景脚本（background.js 或 background.ts）中，你可以使用 chrome.webNavigation API 的各种事件。
```js
chrome.webNavigation.onCompleted.addListener(function(details) {
  chrome.tabs.get(details.tabId, function(tab) {
    console.log("Current tab:", tab);
  });
});

chrome.webNavigation.onCompleted.addListener((details) => {
    chrome.storage.local.get('active', (result) => {
        if (result.active !== false) {  // default to active if not set
            chrome.scripting.executeScript({
                target: {tabId: details.tabId},
                files: ['contentScript.js']
            });
        }
    });
});
```
