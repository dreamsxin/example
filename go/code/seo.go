package job

import (
	""
	"seo"
	"models"
	"service"
	"fmt"
	"log"
	"net/http"
	"net/url"
	"os"
	"strconv"
	"strings"
	"sync"
	"time"

	"github.com/PuerkitoBio/goquery"
	"github.com/baowk/dilu-core/core"
	"github.com/snabb/sitemap"
	"go.uber.org/zap"
	"gorm.io/gorm"
	"gorm.io/gorm/clause"
)

const sitemapfilepath = "sitemap.xml"

var sm *sitemap.Sitemap
var dsturls map[string]*sitemap.URL
var dsturlsLock sync.Mutex

func init() {
	// 初始化
	sm = sitemap.New()
	dsturls = make(map[string]*sitemap.URL, 100)
	if fileExists(sitemapfilepath) {
		//生成 sitemap.xml
		file, err := os.OpenFile(sitemapfilepath, os.O_CREATE|os.O_RDONLY, os.ModePerm)
		if err != nil {
			core.Log.Error("StartSeo.OpenFile", zap.Error(err))
			return
		}
		defer file.Close()

		sm.ReadFrom(file)

		for _, v := range sm.URLs {
			dsturls[v.Loc] = v
		}
	}
}

func StartSeo() {
	if config.Ext.SEO.Site == "" {
		return
	}
	resp, err := http.Get(config.Ext.SEO.Site)
	if err != nil {
		fmt.Println("获取页面失败：", err)
		core.Log.Error("StartSeo", zap.Error(err))
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
			core.Log.Debug("StartSeo", zap.Any("find", href))
			if !strings.HasPrefix(href, "http") {
				dsturl, err := url.JoinPath(config.Ext.SEO.Site, href)
				core.Log.Debug("StartSeo", zap.Any("find", dsturl))
				// dsturl, err := url.Parse(href)
				if err == nil {
					urls = append(urls, dsturl)
				}
			}
		}
	})

	//更新新闻
	seoPush := models.SeoPush{}
	err = service.SerSeoPush.DB().Model(&models.SeoPush{}).Order("id DESC").First(&seoPush).Error
	if err != nil || seoPush.Id <= 0 {
		core.Log.Error("StartSeo.First", zap.Error(err))
		log.Println("------------------------------------", time.Now().AddDate(-1, 0, 0))
		SeoUpdated(time.Now().AddDate(-1, 0, 0))
		log.Println("------------------------------------", time.Now().AddDate(-1, 0, 0))
	} else {
		SeoUpdated(seoPush.CreatedAt)
	}
	SeoAdd(urls)

	SeoSave()
}

func SeoDifference(a, b []string) (diff []string) {
	m := make(map[string]bool)

	for _, item := range b {
		m[item] = true
	}

	for _, item := range a {
		if _, ok := m[item]; !ok {
			diff = append(diff, item)
		}
	}
	return
}

func SeoUniqueSliceElements[T comparable](inputSlice []T) []T {
	uniqueSlice := make([]T, 0, len(inputSlice))
	seen := make(map[T]bool, len(inputSlice))
	for _, element := range inputSlice {
		if !seen[element] {
			uniqueSlice = append(uniqueSlice, element)
			seen[element] = true
		}
	}
	return uniqueSlice
}

func fileExists(filename string) bool {
	_, err := os.Stat(filename)
	if err != nil {
		if os.IsNotExist(err) {
			return false
		}
		return false
	}
	return true
}

func SeoSave() {
	file, err := os.OpenFile(sitemapfilepath, os.O_CREATE|os.O_TRUNC|os.O_WRONLY, os.ModePerm)
	if err != nil {
		core.Log.Error("StartSeo.SeoSave", zap.Error(err))
		return
	}
	defer file.Close()
	sm.WriteTo(file)
}

func SeoAdd(urls []string) {
	dsturlsLock.Lock()
	defer dsturlsLock.Unlock()

	urls = SeoUniqueSliceElements(urls)
	core.Log.Debug("SeoAdd", zap.Any("urls", urls))

	conflict := clause.OnConflict{
		Columns:   []clause.Column{{Name: "url"}},
		DoNothing: true,
	}

	t := time.Now()
	for _, v := range urls {
		if _, ok := dsturls[v]; !ok {
			seoPush := &models.SeoPush{}
			seoPush.Url = v
			err := service.SerSeoPush.DB().Clauses(conflict).Create(seoPush).Error
			if err != nil {
				core.Log.Error("StartSeo.Create", zap.Error(err))
			}
			smurl := &sitemap.URL{
				Loc:        v,
				LastMod:    &t,
				ChangeFreq: sitemap.Daily,
				Priority:   0.6,
			}
			sm.Add(smurl)
			dsturls[v] = smurl

			core.Log.Debug("SeoAdd:" + v)
		}
	}
}

func SeoRemoveIndex(s []*sitemap.URL, index int) []*sitemap.URL {
	ret := make([]*sitemap.URL, 0)
	ret = append(ret, s[:index]...)
	return append(ret, s[index+1:]...)
}

func SeoDel(urls []string) {
	dsturlsLock.Lock()
	defer dsturlsLock.Unlock()

	urls = SeoUniqueSliceElements(urls)
	core.Log.Debug("SeoDel", zap.Any("urls", urls))

	oldurls := sm.URLs
	delnum := 0
	for i, v := range oldurls {
		found := false
		for _, vv := range urls {
			if vv == v.Loc {
				found = true
				break
			}
		}
		if found {
			sm.URLs = SeoRemoveIndex(sm.URLs, i-delnum)
			core.Log.Debug("SeoDel:" + v.Loc)
			delnum++
		}
	}
	for _, v := range urls {
		delete(dsturls, v)
	}
}

// 从数据库读取新闻
func SeoUpdated(last time.Time) {
	core.Log.Debug("SeoUpdated:"+config.Ext.SEO.Newsurl, zap.Any("lastTime", last.Format("2006-01-02 15:04:05")))
	if config.Ext.SEO.Newsurl != "" {
		//查询指定时间之后添加的数据

		addlist := make([]models.HelpCenters, 10)
		db := service.SerHelpCenters.DB().Where("category_id in (?)", gorm.Expr("SELECT id FROM help_center_categories WHERE type= 1")).Where("created_at >= ?", last.Format("2006-01-02 15:04:05"))

		err := db.Limit(1000).Offset(0).Find(&addlist).Error
		if err != nil {
			core.Log.Error("SeoUpdated", zap.Error(err))
		}
		urls := []string{}
		for _, data := range addlist {
			url := strings.Replace(config.Ext.SEO.Newsurl, "{0}", strconv.Itoa(data.Id), 1)
			urls = append(urls, url)
		}
		SeoAdd(urls)

		//查询指定时间之后删除的数据
		dellist := make([]models.HelpCenters, 10)
		db = service.SerHelpCenters.DB().Unscoped().Where("category_id in (?)", gorm.Expr("SELECT id FROM help_center_categories WHERE type= 1")).Where("deleted_at >= ?", last.Format("2006-01-02 15:04:05"))

		err = db.Limit(1000).Offset(0).Find(&dellist).Error
		if err != nil {
			core.Log.Error("SeoUpdated", zap.Error(err))
		}

		urls = []string{}
		for _, data := range dellist {
			url := strings.Replace(config.Ext.SEO.Newsurl, "{0}", strconv.Itoa(data.Id), 1)
			urls = append(urls, url)
		}
		SeoDel(urls)

		SeoSave()
	}
}

func Push(dsturls []string) {
	size := 10

	var j int
	for i := 0; i < len(dsturls); i += size {
		j += size
		if j > len(dsturls) {
			j = len(dsturls)
		}
		// do what do you want to with the sub-slice, here just printing the sub-slices
		sub := dsturls[i:j]
		res := PushUrls(sub)
		core.Log.Info("StartSeo.PushUrls", zap.Any("res", res))
		if i > 0 {
			return
		}
		// 设置状态
		if res.Success > 0 {
			diff := sub
			if len(res.NotSameSite) > 0 {
				diff = SeoDifference(diff, res.NotSameSite)
			}
			if len(res.NotSameSite) > 0 {
				diff = SeoDifference(diff, res.NotValid)
			}
			//设置状态
			err := service.SerSeoPush.DB().Model(&models.SeoPush{}).Where("status != 2").Where("url in (?)", diff).Update("status", 2).Error
			if err != nil {
				core.Log.Error("StartSeo.Create", zap.Error(err))
			}
			//设置状态
			err = service.SerSeoPush.DB().Model(&models.SeoPush{}).Where("status = 1").Where("url in (?)", res.NotSameSite).Update("status", 3).Error
			if err != nil {
				core.Log.Error("StartSeo.Create", zap.Error(err))
			}
			//设置状态
			err = service.SerSeoPush.DB().Model(&models.SeoPush{}).Where("status = 1").Where("url in (?)", res.NotValid).Update("status", 3).Error
			if err != nil {
				core.Log.Error("StartSeo.Create", zap.Error(err))
			}
		}
	}
}


type SeoPushRes struct {
	Error       int      `json:"error"`
	Message     string   `json:"message"`
	Remain      int      `json:"remain"`        //成功推送的url条数
	Success     int      `json:"success"`       //当天剩余的可推送url条数
	NotSameSite []string `json:"not_same_site"` //由于不是本站url而未处理的url列表
	NotValid    []string `json:"not_valid"`     //不合法的url列表
}

// 百度链接推送
func PushUrls(urls []string) *SeoPushRes {
	res := &SeoPushRes{}

	log.Println("seo.push", urls)
	if core.Log != nil {
		core.Log.Info("seo.push", zap.Any("urls", urls))
	} else {
		log.Println("seo.push", urls)
	}
	if len(urls) == 0 {
		core.Log.Info("seo.PushUrls", zap.Error(errors.New("百度链接推送未配置")))
		return res
	}
	if len(config.Ext.SEO.Site) == 0 || len(config.Ext.SEO.Token) == 0 {
		core.Log.Info("seo.PushUrls", zap.Error(errors.New("百度链接推送未配置")))
		return res
	}
	// Create a Resty Client
	client := resty.New()
	// Unique settings at Client level
	//--------------------------------
	// Enable debug mode
	client.SetDebug(true)
	// 百度链接提交
	api := "http://data.zz.baidu.com/urls?site=" + config.Ext.SEO.Site + "&token=" + config.Ext.SEO.Token
	body := strings.Join(urls, "\n")
	response, err := client.R().SetResult(res).SetError(res).SetBody(body).Post(api)
	if err != nil {
		core.Log.Error("seo.push", zap.Error(err))
	} else {
		core.Log.Info("seo.push", zap.Any("resp", string(response.Body())))
	}
	return res
}
