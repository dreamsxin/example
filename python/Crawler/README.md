# Crawler 爬虫

```shell
sudo apt-get install python-requests
sudo apt-get install python-lxml python-html5lib
sudo apt-get install python-beautifulsoup
```

# requests

```python
url = 'https://api.github.com/some/endpoint'
headers = {'user-agent': 'my-app/0.0.1'}

r = requests.get(url, headers=headers)

payload = {'key1': 'value1', 'key2': ['value2', 'value3']}

r = requests.get('http://httpbin.org/get', params=payload)
print(r.url)
print(r.encoding)

r = requests.post('http://httpbin.org/post', data = {'key':'value'})
r = requests.put('http://httpbin.org/put', data = {'key':'value'})
r = requests.delete('http://httpbin.org/delete')
r = requests.head('http://httpbin.org/get')
r = requests.options('http://httpbin.org/get')

rs = requests.session()

f = open('image.png', 'w')
shutil.copyfileobj(r.raw,f)
f.close()
```

示例
```python
res = requests.get('http://www.myleftstudio.com')
print res.status_code
if res.status_code != requests.codes.ok:
	raise SystemExit
print res.headers['content-type']
print res.text
```

# BeautifulSoup

```python
print(soup.prettify())
# <html>
#  <head>
#   <title>
#    The Dormouse's story
#   </title>
#  </head>
#  <body>
#   <p class="title">
#    <b>
#     The Dormouse's story
#    </b>
#   </p>
#   <p class="story">
#    Once upon a time there were three little sisters; and their names were
#    <a class="sister" href="http://example.com/elsie" id="link1">
#     Elsie
#    </a>
#    ,
#    <a class="sister" href="http://example.com/lacie" id="link2">
#     Lacie
#    </a>
#    and
#    <a class="sister" href="http://example.com/tillie" id="link2">
#     Tillie
#    </a>
#    ; and they lived at the bottom of a well.
#   </p>
#   <p class="story">
#    ...
#   </p>
#  </body>
# </html>

soup.title
# <title>The Dormouse's story</title>

soup.title.name
# u'title'

soup.title.string
# u'The Dormouse's story'

soup.title.parent.name
# u'head'

soup.p
# <p class="title"><b>The Dormouse's story</b></p>

soup.p['class']
# u'title'

soup.a
# <a class="sister" href="http://example.com/elsie" id="link1">Elsie</a>

soup.find_all('a')
# [<a class="sister" href="http://example.com/elsie" id="link1">Elsie</a>,
#  <a class="sister" href="http://example.com/lacie" id="link2">Lacie</a>,
#  <a class="sister" href="http://example.com/tillie" id="link3">Tillie</a>]

soup.find(id="link3")
# <a class="sister" href="http://example.com/tillie" id="link3">Tillie</a>

for link in soup.find_all('a'):
    print(link.get('href'))
    # http://example.com/elsie
    # http://example.com/lacie
    # http://example.com/tillie

print(soup.get_text())

print print soap.text # no tag
print print soap.contents
print soap.select('footer')[0]
print soap.select('#phalcon7')[0]
```

##  selenium

```python
from selenium import webdriver
from selenium.common.exceptions import NoSuchElementException
from selenium.webdriver.common.keys import Keys
import time

browser = webdriver.Firefox() # Get local session of firefox
browser.get("http://www.yahoo.com") # Load page
assert "Yahoo!" in browser.title
elem = browser.find_element_by_name("p") # Find the query box
elem.send_keys("seleniumhq" + Keys.RETURN)
time.sleep(0.2) # Let the page load, will be added to the API

try:
    browser.find_element_by_xpath("//a[contains(@href,'http://seleniumhq.org')]")
	browser.find_element_by_css_selector('.button.c_button.s_button').click()
	browser.find_element_by_link_text('Next').click()
	print browser.page_source
except NoSuchElementException:
    assert 0, "can't find seleniumhq"
browser.close()
```

## cv2

```python
import matplotlib.pyplot as plt
import cv2

image = cv2.imread('Captcha.jpg')
plt.imshow(image)

# 橡皮差
kernel = np.ones((4,4),np.uint8)
erosion = cv2.erode(image,kernel,iterations=1)
# 显示边界
blurred = cv2.GaussianBlur(erosion, (5,5), 0)
edged = cv2.Canny(blurred, 30, 150)
#膨胀，连接断开的线
dilation = cv2.dilate(edged,kernel,iterations=1)


image, contours, hierarchy = cv2.findContours(dilation.copy(),cv2.RETR_TREE,cv2.CHAIN_APPROX_SIMPLE)
cnts = sorted([c, cv2.boundingRect(c)[0] for c in countours, key = lambda x: x[1])

ary = []
for (c, _) in cnts:
	(x, y, w, h) = cv2.boundingRect(c)
	if w>15 and h>15:
		ary.append((x,y,w,h))

fig = plt.figure
for id, (x,y,w,h) in enumerate(arg):
	roi = dilation[y:y+h, x:x+w]
	thresh = roi.copy()
	a = fig.add_subplot(1,len(ary),id+1)
	plt.imshow(thresh)
```

## pytesser

```python
from pytesser import *

image = Image.open('0.png')
print image_to_string(image)
```
