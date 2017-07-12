# coding:utf-8
import requests
from bs4 import BeautifulSoup
import os

if not os.path.exists('./index.html'):
	res = requests.get('http://www.myleftstudio.com')
	print res.status_code
	if res.status_code != requests.codes.ok:
		raise SystemExit
	print res.headers['content-type']
	html_file = open('./index.html', 'w')
	html_file.write(res.text.encode('utf-8'))
	html_file.close()
	soap = BeautifulSoup(res.text, 'html.parser')
else:
	html_file = open('./index.html', 'rw')
	soap = BeautifulSoup(html_file.read(), 'lxml')
	html_file.close()


e = soap.select('#phalcon7')[0]
print e.select('h1')