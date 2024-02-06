func BestLang(lang string) string {
	firstlang := lang
	tags, _, _ := language.ParseAcceptLanguage(lang)
	core.Log.Debug("BestLang", zap.Any("tags", tags))
	langs := []string{}
	for _, tag := range tags {
		langs = append(langs, tag.String())
	}
	if len(tags) <= 0 {
		langs = append(langs, firstlang)
	}
	d := time.Duration(0)
	if lastUpdate.IsZero() {
		d = time.Since(lastUpdate)
	}

	if len(langList) <= 0 || d.Minutes() >= 5 {

		value, err, _ := utils.GetSingleFlight().Do("bestlang:"+lang, func() (v interface{}, e error) {

			items := make([]*models.Langs, 20)

			//
			return items, err
		})
		if err != nil {
			return ""
		}
		langList = value.([]*models.Langs)
	}
	if len(langList) <= 0 {
		return ""
	}
	if len(langList) == 1 {
		return langList[0].Lang
	}
	tempList := make([]*models.Langs, len(langList))
	copy(tempList, langList)
	for _, userlang := range langs {

		//取相似度最高的
		for _, lang := range tempList {
			similarity := strutil.Similarity(lang.Lang, userlang, metrics.NewHamming())
			if lang.Similarity < similarity {
				lang.Similarity = similarity
			}
			core.Log.Sugar().Debugf("计算语言相似度 Similarity %v Lang %v userlang %v", lang.Similarity, lang.Lang, userlang)
		}
		sort.Slice(tempList, func(i, j int) bool {
			return tempList[i].Similarity >= tempList[j].Similarity
		})
		if tempList[0].Similarity > 0.5 {
			return tempList[0].Lang
		}
	}
	sort.Slice(tempList, func(i, j int) bool {
		return tempList[i].Similarity >= tempList[j].Similarity
	})
	return tempList[0].Lang
}
