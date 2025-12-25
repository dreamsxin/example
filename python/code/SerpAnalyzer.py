import requests
import json
import re
from typing import Dict, List, Tuple, Optional
from dataclasses import dataclass
from enum import Enum
from collections import Counter
import matplotlib.pyplot as plt
import numpy as np

class RelevanceLevel(Enum):
    """相关性级别枚举"""
    FULL_MATCH = "完全包含"
    PARTIAL_MATCH = "部分包含"
    REFERENCE = "可供参考"
    IRRELEVANT = "基本无关"

@dataclass
class SearchResult:
    """搜索结果数据类"""
    title: str
    snippet: str
    url: str
    relevance_level: Optional[RelevanceLevel] = None
    relevance_score: float = 0.0
    site_quality_score: float = 0.0
    weighted_score: float = 0.0
    final_rating: str = ""

class SerpAnalyzer:
    """Serp API搜索结果分析器"""
    
    def __init__(self, api_key: str = None, relevance_weight: float = 0.7, 
                 quality_weight: float = 0.3):
        """
        初始化分析器
        
        Args:
            api_key: Serp API密钥
            relevance_weight: 相关性权重(默认0.7)
            quality_weight: 站点质量权重(默认0.3)
        """
        self.api_key = api_key
        self.relevance_weight = relevance_weight
        self.quality_weight = quality_weight
        
        # 高质量域名列表（示例）
        self.high_quality_domains = {
            'wikipedia.org', 'github.com', 'stackoverflow.com',
            'medium.com', 'towardsdatascience.com', 'arxiv.org',
            'ieee.org', 'springer.com', 'nature.com', 'science.org'
        }
        
        # 中等质量域名列表
        self.medium_quality_domains = {
            'blogspot.com', 'wordpress.com', 'youtube.com',
            'linkedin.com', 'twitter.com', 'facebook.com'
        }
    
    def search_with_serpapi(self, query: str, num_results: int = 10, 
                           location: str = "us", lang: str = "en") -> List[SearchResult]:
        """
        使用Serp API进行搜索
        
        Args:
            query: 搜索查询
            num_results: 结果数量
            location: 地理位置
            lang: 语言
            
        Returns:
            SearchResult对象列表
        """
        if not self.api_key:
            # 如果没有API密钥，使用模拟数据
            print("警告：未提供API密钥，使用模拟数据进行演示")
            return self._get_mock_data(query, num_results)
        
        try:
            params = {
                "q": query,
                "api_key": self.api_key,
                "num": num_results,
                "location": location,
                "hl": lang,
                "gl": "us"
            }
            
            response = requests.get("https://serpapi.com/search", params=params)
            response.raise_for_status()
            
            data = response.json()
            results = []
            
            for item in data.get('organic_results', [])[:num_results]:
                result = SearchResult(
                    title=item.get('title', ''),
                    snippet=item.get('snippet', ''),
                    url=item.get('link', '')
                )
                results.append(result)
            
            return results
            
        except Exception as e:
            print(f"Serp API请求失败: {e}")
            return self._get_mock_data(query, num_results)
    
    def _get_mock_data(self, query: str, num_results: int = 10) -> List[SearchResult]:
        """生成模拟数据用于测试"""
        mock_results = []
        
        # 基于查询生成不同的结果
        query_lower = query.lower()
        
        for i in range(num_results):
            if 'python' in query_lower:
                titles = [
                    f"Complete Python Tutorial for Beginners - {query}",
                    f"How to use {query} in Python programming",
                    f"Advanced {query} techniques in Python",
                    f"{query} - Python Documentation",
                    f"Understanding {query} in machine learning"
                ]
                snippets = [
                    f"This tutorial covers everything you need to know about {query} in Python.",
                    f"Learn how to implement {query} efficiently with Python code examples.",
                    f"In this article, we explore {query} and its applications in data science.",
                    f"Official documentation for {query} module in Python standard library.",
                    f"A comprehensive guide to mastering {query} concepts and best practices."
                ]
            else:
                titles = [
                    f"Understanding {query} - A Comprehensive Guide",
                    f"The Importance of {query} in Modern Technology",
                    f"How {query} is Changing the Industry",
                    f"Beginner's Guide to {query}",
                    f"Advanced Topics in {query} Research"
                ]
                snippets = [
                    f"This article provides a deep dive into {query} and its applications.",
                    f"Discover why {query} is becoming increasingly important today.",
                    f"Explore the impact of {query} on various sectors of the economy.",
                    f"Learn the basics of {query} with easy-to-follow examples.",
                    f"Recent research on {query} shows promising results for the future."
                ]
            
            domains = [
                'wikipedia.org', 'github.com', 'medium.com',
                'blog.example.com', 'news.site.org', 'research-paper.edu',
                'tutorials.net', 'docs.python.org', 'stackoverflow.com'
            ]
            
            title = titles[i % len(titles)]
            snippet = snippets[i % len(snippets)]
            domain = domains[i % len(domains)]
            url = f"https://{domain}/article-{i+1}"
            
            result = SearchResult(
                title=title,
                snippet=snippet,
                url=url
            )
            mock_results.append(result)
        
        return mock_results
    
    def analyze_relevance(self, query: str, result: SearchResult) -> RelevanceLevel:
        """
        分析单个结果的相关性
        
        Args:
            query: 原始查询
            result: 搜索结果
            
        Returns:
            相关性级别
        """
        query_lower = query.lower()
        title_lower = result.title.lower()
        snippet_lower = result.snippet.lower()
        
        # 检查完全包含
        query_words = set(re.findall(r'\b\w+\b', query_lower))
        title_words = set(re.findall(r'\b\w+\b', title_lower))
        snippet_words = set(re.findall(r'\b\w+\b', snippet_lower))
        
        # 判断完全包含
        if query_lower in title_lower or query_lower in snippet_lower:
            return RelevanceLevel.FULL_MATCH
        
        # 判断部分包含（超过50%的查询词出现）
        matched_in_title = len(query_words.intersection(title_words)) / len(query_words)
        matched_in_snippet = len(query_words.intersection(snippet_words)) / len(query_words)
        
        if matched_in_title > 0.5 or matched_in_snippet > 0.5:
            return RelevanceLevel.PARTIAL_MATCH
        
        # 判断可供参考（有部分匹配）
        if (matched_in_title > 0.2 or matched_in_snippet > 0.2 or 
            any(word in title_lower or word in snippet_lower for word in query_words)):
            return RelevanceLevel.REFERENCE
        
        # 基本无关
        return RelevanceLevel.IRRELEVANT
    
    def score_site_quality(self, url: str) -> float:
        """
        评估站点质量分数(0-5分)
        
        Args:
            url: 网站URL
            
        Returns:
            质量分数(0-5)
        """
        try:
            from urllib.parse import urlparse
            domain = urlparse(url).netloc.lower()
            
            # 基于域名评估质量
            for high_domain in self.high_quality_domains:
                if high_domain in domain:
                    return 5.0
            
            for medium_domain in self.medium_quality_domains:
                if medium_domain in domain:
                    return 3.5
            
            # 检查域名权威性特征
            if domain.endswith('.edu') or domain.endswith('.gov'):
                return 4.5
            elif domain.endswith('.org'):
                return 3.0
            elif len(domain.split('.')) > 2:  # 子域名较多
                return 2.0
            else:
                return 2.5
                
        except:
            return 2.5  # 默认中等分数
    
    def calculate_weighted_score(self, relevance_level: RelevanceLevel, 
                               site_quality_score: float) -> float:
        """
        计算加权平均分
        
        Args:
            relevance_level: 相关性级别
            site_quality_score: 站点质量分数
            
        Returns:
            加权分数(0-5)
        """
        # 将相关性级别转换为分数
        relevance_scores = {
            RelevanceLevel.FULL_MATCH: 5.0,
            RelevanceLevel.PARTIAL_MATCH: 4.0,
            RelevanceLevel.REFERENCE: 3.0,
            RelevanceLevel.IRRELEVANT: 1.0
        }
        
        relevance_score = relevance_scores[relevance_level]
        
        # 计算加权平均
        weighted_score = (relevance_score * self.relevance_weight + 
                         site_quality_score * self.quality_weight)
        
        return weighted_score
    
    def get_rating(self, weighted_score: float) -> str:
        """
        根据加权分数确定评级
        
        Args:
            weighted_score: 加权分数
            
        Returns:
            'good', 'same', 或 'bad'
        """
        if weighted_score >= 4.0:
            return "good"
        elif weighted_score >= 2.5:
            return "same"
        else:
            return "bad"
    
    def analyze_results(self, query: str, results: List[SearchResult]) -> Dict:
        """
        分析所有搜索结果
        
        Args:
            query: 搜索查询
            results: 搜索结果列表
            
        Returns:
            包含分析结果的字典
        """
        # 初始化计数器
        relevance_counts = Counter()
        rating_counts = Counter()
        
        # 分析每个结果
        for result in results:
            # 分析相关性
            relevance_level = self.analyze_relevance(query, result)
            result.relevance_level = relevance_level
            relevance_counts[relevance_level.value] += 1
            
            # 评估站点质量
            site_score = self.score_site_quality(result.url)
            result.site_quality_score = site_score
            
            # 计算加权分数
            weighted_score = self.calculate_weighted_score(relevance_level, site_score)
            result.weighted_score = weighted_score
            
            # 确定最终评级
            rating = self.get_rating(weighted_score)
            result.final_rating = rating
            rating_counts[rating] += 1
        
        # 计算百分比
        total_results = len(results)
        relevance_percentages = {
            level: (count / total_results * 100) 
            for level, count in relevance_counts.items()
        }
        
        rating_percentages = {
            rating: (count / total_results * 100)
            for rating, count in rating_counts.items()
        }
        
        # 计算平均分数
        avg_relevance_score = np.mean([
            self.calculate_weighted_score(result.relevance_level, result.site_quality_score)
            for result in results
        ])
        
        return {
            'query': query,
            'total_results': total_results,
            'relevance_distribution': dict(relevance_counts),
            'relevance_percentages': relevance_percentages,
            'rating_distribution': dict(rating_counts),
            'rating_percentages': rating_percentages,
            'average_score': avg_relevance_score,
            'detailed_results': results
        }
    
    def visualize_analysis(self, analysis_result: Dict):
        """
        可视化分析结果
        
        Args:
            analysis_result: 分析结果字典
        """
        fig, axes = plt.subplots(2, 2, figsize=(12, 10))
        
        # 1. 相关性分布饼图
        ax1 = axes[0, 0]
        labels = analysis_result['relevance_distribution'].keys()
        sizes = analysis_result['relevance_distribution'].values()
        colors = ['#4CAF50', '#2196F3', '#FFC107', '#F44336']
        ax1.pie(sizes, labels=labels, colors=colors, autopct='%1.1f%%', startangle=90)
        ax1.set_title('相关性分布')
        
        # 2. 评分分布饼图
        ax2 = axes[0, 1]
        labels = ['Good', 'Same', 'Bad']
        sizes = [analysis_result['rating_distribution'].get('good', 0),
                analysis_result['rating_distribution'].get('same', 0),
                analysis_result['rating_distribution'].get('bad', 0)]
        colors = ['#4CAF50', '#FFC107', '#F44336']
        ax2.pie(sizes, labels=labels, colors=colors, autopct='%1.1f%%', startangle=90)
        ax2.set_title('评分分布 (good:same:bad)')
        
        # 3. 相关性分数直方图
        ax3 = axes[1, 0]
        scores = [r.weighted_score for r in analysis_result['detailed_results']]
        ax3.hist(scores, bins=10, color='skyblue', edgecolor='black')
        ax3.axvline(np.mean(scores), color='red', linestyle='dashed', linewidth=2, 
                   label=f'平均分: {np.mean(scores):.2f}')
        ax3.set_xlabel('加权分数')
        ax3.set_ylabel('数量')
        ax3.set_title('分数分布直方图')
        ax3.legend()
        
        # 4. 质量vs相关性散点图
        ax4 = axes[1, 1]
        rel_scores = []
        quality_scores = []
        colors_map = []
        
        for result in analysis_result['detailed_results']:
            rel_scores.append({
                RelevanceLevel.FULL_MATCH: 5,
                RelevanceLevel.PARTIAL_MATCH: 4,
                RelevanceLevel.REFERENCE: 3,
                RelevanceLevel.IRRELEVANT: 1
            }[result.relevance_level])
            quality_scores.append(result.site_quality_score)
            colors_map.append({
                'good': 'green',
                'same': 'orange',
                'bad': 'red'
            }[result.final_rating])
        
        for i, (x, y, c) in enumerate(zip(rel_scores, quality_scores, colors_map)):
            ax4.scatter(x, y, color=c, s=100, alpha=0.6)
            ax4.annotate(f'{i+1}', (x, y), fontsize=8, ha='center', va='center')
        
        ax4.set_xlabel('相关性分数')
        ax4.set_ylabel('站点质量分数')
        ax4.set_title('相关性 vs 站点质量')
        ax4.grid(True, alpha=0.3)
        
        plt.suptitle(f"搜索结果分析: '{analysis_result['query']}'", fontsize=16)
        plt.tight_layout()
        plt.show()
    
    def print_analysis_report(self, analysis_result: Dict):
        """
        打印分析报告
        
        Args:
            analysis_result: 分析结果字典
        """
        print("\n" + "="*60)
        print("搜索结果相关性分析报告")
        print("="*60)
        
        print(f"\n查询: {analysis_result['query']}")
        print(f"分析结果总数: {analysis_result['total_results']}")
        print(f"平均加权分数: {analysis_result['average_score']:.2f}/5.0")
        
        print("\n" + "-"*60)
        print("相关性分布:")
        print("-"*60)
        for level, count in analysis_result['relevance_distribution'].items():
            percentage = analysis_result['relevance_percentages'][level]
            print(f"  {level}: {count} 个 ({percentage:.1f}%)")
        
        print("\n" + "-"*60)
        print("评分分布 (good:same:bad):")
        print("-"*60)
        good = analysis_result['rating_distribution'].get('good', 0)
        same = analysis_result['rating_distribution'].get('same', 0)
        bad = analysis_result['rating_distribution'].get('bad', 0)
        
        good_pct = analysis_result['rating_percentages'].get('good', 0)
        same_pct = analysis_result['rating_percentages'].get('same', 0)
        bad_pct = analysis_result['rating_percentages'].get('bad', 0)
        
        print(f"  Good: {good} 个 ({good_pct:.1f}%)")
        print(f"  Same: {same} 个 ({same_pct:.1f}%)")
        print(f"  Bad:  {bad} 个 ({bad_pct:.1f}%)")
        
        print(f"\n  Good:Same:Bad 比例 = {good}:{same}:{bad}")
        print(f"  或 {good_pct:.1f}% : {same_pct:.1f}% : {bad_pct:.1f}%")
        
        print("\n" + "-"*60)
        print("详细结果:")
        print("-"*60)
        for i, result in enumerate(analysis_result['detailed_results'], 1):
            print(f"\n结果 #{i}:")
            print(f"  标题: {result.title[:50]}...")
            print(f"  相关性: {result.relevance_level.value}")
            print(f"  站点质量: {result.site_quality_score:.1f}/5.0")
            print(f"  加权分数: {result.weighted_score:.2f}/5.0")
            print(f"  最终评级: {result.final_rating.upper()}")
            print(f"  URL: {result.url}")

def main():
    """主函数示例"""
    # 初始化分析器
    analyzer = SerpAnalyzer(
        api_key=None,  # 替换为您的Serp API密钥
        relevance_weight=0.7,
        quality_weight=0.3
    )
    
    # 示例查询
    queries = [
        "Python machine learning tutorial",
        "blockchain technology explained",
        "climate change effects 2024"
    ]
    
    for query in queries:
        print(f"\n{'='*80}")
        print(f"正在分析查询: '{query}'")
        print('='*80)
        
        # 搜索
        results = analyzer.search_with_serpapi(query, num_results=8)
        
        # 分析
        analysis = analyzer.analyze_results(query, results)
        
        # 打印报告
        analyzer.print_analysis_report(analysis)
        
        # 可视化
        analyzer.visualize_analysis(analysis)

if __name__ == "__main__":
    main()
