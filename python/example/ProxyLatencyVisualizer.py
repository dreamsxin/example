import pandas as pd
import matplotlib.pyplot as plt
import matplotlib.dates as mdates
from clickhouse_driver import Client
import numpy as np
from datetime import datetime
import seaborn as sns

class ProxyLatencyVisualizer:
    def __init__(self, host='localhost', port='9000', user='default', password='', database='default'):
        self.client = Client(host=host, port=port, user=user, password=password, database=database)
    
    def fetch_data(self):
        """从ClickHouse获取数据"""
        sql = """
        SELECT
            toStartOfMinute(created_at) AS time_minute,
            extract(url, '@([^:]+)') AS host,
            count() AS count,
            avg(duration_ms) AS avg_duration,
            min(duration_ms) AS min_duration,
            max(duration_ms) AS max_duration,
            quantile(0.5)(duration_ms) AS median_duration,
            quantile(0.95)(duration_ms) AS p95_duration,
            quantile(0.99)(duration_ms) AS p99_duration,
            stddevPop(duration_ms) AS std_duration
        FROM logs
        WHERE 
           timestamp > '2025-11-20 08:45:00' and timestamp < '2025-11-20 10:00:00'
        GROUP BY url, time_minute
        ORDER BY url, time_minute;
        """
        
        result = self.client.execute(sql)
        
        # 转换为DataFrame
        columns = ['time_minute', 'host', 'count', 'avg_duration', 'min_duration', 
                  'max_duration', 'median_duration', 'p95_duration', 'p99_duration', 'std_duration']
        df = pd.DataFrame(result, columns=columns)
        df['time_minute'] = pd.to_datetime(df['time_minute'])
        
        return df
    
    def create_latency_trend_plot(self, df, top_hosts=10):
        """创建延时趋势图"""
        # 选择请求量最多的前N个主机
        host_counts = df.groupby('host')['count'].sum().sort_values(ascending=False)
        top_hosts_list = host_counts.head(top_hosts).index.tolist()
        df_top = df[df['host'].isin(top_hosts_list)]
        
        fig, axes = plt.subplots(2, 2, figsize=(16, 12))
        fig.suptitle('代理服务器延时波动分析', fontsize=16, fontweight='bold')
        
        # 1. 平均延时趋势
        self._plot_latency_trend(axes[0, 0], df_top, 'avg_duration', '平均延时 (ms)', '平均延时趋势')
        
        # 2. 中位数延时趋势
        self._plot_latency_trend(axes[0, 1], df_top, 'median_duration', '中位数延时 (ms)', '中位数延时趋势')
        
        # 3. P95延时趋势
        self._plot_latency_trend(axes[1, 0], df_top, 'p95_duration', 'P95延时 (ms)', 'P95延时趋势')
        
        # 4. 延时波动（标准差）
        self._plot_std_trend(axes[0, 2], df_top, '标准差趋势')
        
        plt.tight_layout()
        return fig
    
    def _plot_latency_trend(self, ax, df, column, ylabel, title):
        """绘制延时趋势子图"""
        for host in df['host'].unique():
            host_data = df[df['host'] == host]
            ax.plot(host_data['time_minute'], host_data[column], 
                   marker='o', markersize=3, linewidth=2, label=host, alpha=0.8)
        
        ax.set_title(title, fontsize=12, fontweight='bold')
        ax.set_ylabel(ylabel, fontsize=10)
        ax.set_xlabel('时间', fontsize=10)
        ax.xaxis.set_major_formatter(mdates.DateFormatter('%H:%M'))
        ax.xaxis.set_major_locator(mdates.MinuteLocator(interval=5))
        ax.legend(bbox_to_anchor=(1.05, 1), loc='upper left', fontsize=8)
        ax.grid(True, alpha=0.3)
        plt.setp(ax.xaxis.get_majorticklabels(), rotation=45)
    
    def _plot_std_trend(self, ax, df, title):
        """绘制标准差趋势子图"""
        for host in df['host'].unique():
            host_data = df[df['host'] == host]
            ax.plot(host_data['time_minute'], host_data['std_duration'], 
                   marker='s', markersize=3, linewidth=2, label=host, alpha=0.8)
        
        ax.set_title(title, fontsize=12, fontweight='bold')
        ax.set_ylabel('延时标准差 (ms)', fontsize=10)
        ax.set_xlabel('时间', fontsize=10)
        ax.xaxis.set_major_formatter(mdates.DateFormatter('%H:%M'))
        ax.xaxis.set_major_locator(mdates.MinuteLocator(interval=5))
        ax.legend(bbox_to_anchor=(1.05, 1), loc='upper left', fontsize=8)
        ax.grid(True, alpha=0.3)
        plt.setp(ax.xaxis.get_majorticklabels(), rotation=45)
    
    def create_host_comparison_plot(self, df, specific_hosts=None):
        """创建指定主机的详细对比图"""
        if specific_hosts:
            df_filtered = df[df['host'].isin(specific_hosts)]
        else:
            # 如果没有指定，使用前5个主机
            host_counts = df.groupby('host')['count'].sum().sort_values(ascending=False)
            df_filtered = df[df['host'].isin(host_counts.head(10).index)]
        
        fig, axes = plt.subplots(2, 2, figsize=(16, 10))
        
        # 延时指标对比
        metrics = ['avg_duration', 'median_duration', 'p95_duration', 'p99_duration']
        metric_names = ['平均延时', '中位数延时', 'P95延时', 'P99延时']
        
        for idx, (metric, name) in enumerate(zip(metrics, metric_names)):
            ax = axes[idx//2, idx%2]
            for host in df_filtered['host'].unique():
                host_data = df_filtered[df_filtered['host'] == host]
                ax.plot(host_data['time_minute'], host_data[metric], 
                       marker='o', markersize=3, linewidth=2, label=host)
            
            ax.set_title(f'{name}对比', fontsize=12, fontweight='bold')
            ax.set_ylabel('延时 (ms)', fontsize=10)
            ax.set_xlabel('时间', fontsize=10)
            ax.xaxis.set_major_formatter(mdates.DateFormatter('%H:%M'))
            ax.legend(fontsize=9)
            ax.grid(True, alpha=0.3)
            plt.setp(ax.xaxis.get_majorticklabels(), rotation=45)
        
        plt.tight_layout()
        return fig
    
    def create_statistical_summary(self, df):
        """创建统计摘要"""
        summary = df.groupby('host').agg({
            'count': 'sum',
            'avg_duration': ['mean', 'std'],
            'p95_duration': 'mean',
            'p99_duration': 'mean',
            'std_duration': 'mean'
        }).round(2)
        
        summary.columns = ['总请求数', '平均延时', '延时标准差', '平均P95', '平均P99', '平均波动']
        summary = summary.sort_values('总请求数', ascending=False)
        
        return summary
    
    def run_analysis(self, specific_hosts=None):
        """运行完整分析"""
        print("正在从ClickHouse获取数据...")
        df = self.fetch_data()
        
        print(f"获取到 {len(df)} 条记录，涉及 {df['host'].nunique()} 个代理主机")
        
        # 显示统计摘要
        summary = self.create_statistical_summary(df)
        print("\n代理服务器统计摘要:")
        print(summary.head(10))
        
        # 创建可视化图表
        print("\n生成可视化图表...")
        
        # 趋势图
        fig1 = self.create_latency_trend_plot(df, top_hosts=20)
        
        # 详细对比图
        if specific_hosts:
            fig2 = self.create_host_comparison_plot(df, specific_hosts)
        else:
            fig2 = self.create_host_comparison_plot(df)
        
        plt.show()
        
        return df, summary

def main():
    # ClickHouse连接配置
    config = {
        'host': 'localhost',
        'port': '9000',
        'user': 'default',
        'password': '',
        'database': 'default'
    }
    
    # 创建可视化器实例
    visualizer = ProxyLatencyVisualizer(**config)
    
    # 运行分析
    # 可以指定特定的主机列表，例如：specific_hosts=['192.119.140.89', '其他IP']
    df, summary = visualizer.run_analysis(specific_hosts=None)
    
    # 可选：保存结果到文件
    # summary.to_csv('proxy_latency_summary.csv')
    # df.to_csv('proxy_latency_detail.csv', index=False)

if __name__ == "__main__":
    main()
