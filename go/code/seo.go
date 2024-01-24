package job

import (
	"fmt"
	"net/http"
	"net/url"
	"strings"

	"github.com/PuerkitoBio/goquery"
)

func StartSeo() {
	resp, err := http.Get("http://baidu.com")
	if err != nil {
		fmt.Println("获取页面失败：", err)
		return
	}
	defer resp.Body.Close()

	doc, err := goquery.NewDocumentFromReader(resp.Body)
	if err != nil {
		fmt.Println("解析页面失败：", err)
		return
	}
	fmt.Println("解析页面：", doc)
	urls := []string{}
	doc.Find("a").Each(func(i int, s *goquery.Selection) {
		href, exists := s.Attr("href")
		if exists {
			if !strings.HasPrefix(href, "http") {
				dsturl, err := url.JoinPath("http://baidu.com", href)
				fmt.Println("StartSeo", dsturl)
				if err == nil {
					urls = append(urls, dsturl)
				}
			}
		}
	})
	fmt.Println("StartSeo", urls)

	PushUrls(urls)
}

func Push(url string) {
	go func() {
		PushUrls([]string{url})
	}()
}

// 百度链接推送
func PushUrls(urls []string) {
	log.Println("seo.push", urls)
	if len(urls) == 0 {
		return
	}
	if len(config.Ext.SEO.Site) == 0 || len(config.Ext.SEO.Token) == 0 {
		return
	}

	api := "http://data.zz.baidu.com/urls?site=" + config.Ext.SEO.Site + "&token=" + config.Ext.SEO.Token
	body := strings.Join(urls, "\n")
	if response, err := resty.New().R().SetBody(body).Post(api); err != nil {
		log.Println("seo.push", err)
	} else {
		log.Println("seo.push", response.Body())
	}
}
